#include "URIEscape.h"
#include <sys/param.h>

#define bufferSize MAXPATHLEN+1 	

#define useLog 0

extern UInt32 gAdditionReferenceCount;

#pragma mark for debug
void showAEDesc(const AppleEvent *ev)
{
	Handle result;
	OSStatus resultStatus;
	resultStatus = AEPrintDescToHandle(ev,&result);
	printf("%s\n",*result);
	DisposeHandle(result);
}

#pragma mark main routine
void safeRelease(CFTypeRef theObj)
{
	if (theObj != NULL) {
		CFRelease(theObj);
	}
}

OSErr getStringValue(const AppleEvent *ev, AEKeyword theKey, CFStringRef *outStr)
{
	OSErr err;
	DescType typeCode;
	DescType returnedType;
    Size actualSize;
	Size dataSize;
	CFStringEncoding encodeKey;
	
	err = AESizeOfParam(ev, theKey, &typeCode, &dataSize);
	if (dataSize == 0) goto bail;
	
	switch (typeCode) {
		case typeChar:
			encodeKey = CFStringGetSystemEncoding();
			break;
		case typeUTF8Text:
			encodeKey = kCFStringEncodingUTF8;
			break;
		default :
			typeCode = typeUnicodeText;
			encodeKey = kCFStringEncodingUnicode;
	}
	
	UInt8 *dataPtr = malloc(dataSize);
	err = AEGetParamPtr (ev, theKey, typeCode, &returnedType, dataPtr, dataSize, &actualSize);
	if (actualSize > dataSize) {
		printf("buffere size is allocated. data:%i actual:%i\n", dataSize, actualSize);
		dataSize = actualSize;
		realloc(dataPtr, dataSize);
		err = AEGetParamPtr (ev, theKey, typeCode, &returnedType, dataPtr, dataSize, &actualSize);
	}
	
	if (err != noErr) {
		free(dataPtr);
		goto bail;
	}
	
	*outStr = CFStringCreateWithBytes(NULL, dataPtr, dataSize, encodeKey, false);
	free(dataPtr);
bail:
		
		return err;
}

OSErr putStringToReply(CFStringRef inStr, CFStringEncoding kEncoding, AppleEvent *reply)
{
	
	OSErr err;
	DescType resultType;
	
	switch (kEncoding) {
		case kCFStringEncodingUTF8 :
			resultType = typeUTF8Text;
			break;
		default :
			resultType = typeUnicodeText;
	}
	
	const char *constBuff = CFStringGetCStringPtr(inStr, kEncoding);
	
	AEDesc resultDesc;
	if (constBuff == NULL) {
		char *buffer;
		CFIndex charLen = CFStringGetLength(inStr);
		CFIndex maxLen = CFStringGetMaximumSizeForEncoding(charLen, kEncoding);
		buffer = malloc(maxLen+1);
		CFStringGetCString(inStr, buffer, maxLen+1, kEncoding);
		err=AECreateDesc(resultType, buffer, strlen(buffer), &resultDesc);
		free(buffer);
	}
	else {
		err=AECreateDesc(resultType, constBuff, strlen(constBuff), &resultDesc);
	}
	
	
	if (err != noErr) goto bail;
	
	err=AEPutParamDesc(reply, keyAEResult, &resultDesc);
	if (err != noErr) {
		AEDisposeDesc(&resultDesc);
	}
	
bail:
		return err;
}

OSErr putFilePathToReply(CFURLRef inURL, AppleEvent *reply)
{
	
	OSErr err;
	char buffer[bufferSize];
	CFURLGetFileSystemRepresentation(inURL, true, (UInt8 *)buffer, bufferSize);
	
	AEDesc resultDesc;

	err=AECreateDesc(typeUTF8Text, buffer, strlen(buffer), &resultDesc);
	
	
	if (err != noErr) goto bail;
	
	err=AEPutParamDesc(reply, keyAEResult, &resultDesc);
	if (err != noErr) {
		AEDisposeDesc(&resultDesc);
	}
	
bail:
		return err;
}
#pragma mark AppleEvent handlers

OSErr unPersentEscape(const AppleEvent *ev, AppleEvent *reply, long refcon)
{
	
	++gAdditionReferenceCount;  // increment the reference count first thing!
	
	OSErr err;

	CFStringRef urlStr = NULL;
	err = getStringValue(ev, keyDirectObject, &urlStr);
	if (urlStr == NULL) goto bail;
		
	CFURLRef theURL = CFURLCreateWithString(NULL, urlStr, NULL);
	
	CFComparisonResult isFileScheme = kCFCompareLessThan;
	CFStringRef theScheme = CFURLCopyScheme(theURL);
	if (theScheme != NULL) {
		isFileScheme = CFStringCompare(theScheme, CFSTR("file"), 0);
		CFRelease(theScheme);
	}
	
	CFComparisonResult isLocalFile = kCFCompareLessThan;
	if (isFileScheme == kCFCompareEqualTo) {
		CFStringRef theHost = CFURLCopyHostName(theURL);
		isLocalFile = CFStringCompare(theHost, CFSTR("localhost"), 0);
	}
	
	if (isLocalFile == kCFCompareEqualTo) {
		err = putFilePathToReply(theURL, reply);
	}
	else {
		CFStringRef unescapedURL = CFURLCreateStringByReplacingPercentEscapes(NULL, urlStr, CFSTR(""));
		err = putStringToReply(unescapedURL, kCFStringEncodingUTF8, reply);
		CFRelease(unescapedURL);
	}
	
	CFRelease(urlStr);
	CFRelease(theURL);

bail:	
	--gAdditionReferenceCount;  // don't forget to decrement the reference count when you leave!
	
	return err;
}

OSErr persentEscape(const AppleEvent *ev, AppleEvent *reply, long refcon)
{
	
	++gAdditionReferenceCount;  // increment the reference count first thing!
	
	OSErr err;
#if useLog	
	showAEDesc(ev);
#endif
	
	CFStringRef originalStr = NULL;
	err = getStringValue(ev, keyDirectObject, &originalStr);
	if (originalStr == NULL) goto bail;
	
	CFStringRef additionalChar = NULL;
	err = getStringValue(ev, kAdditionalCharParam, &additionalChar);
	if (originalStr == NULL) goto bail;

	CFStringRef leavingChar = NULL;
	err = getStringValue(ev, kLeavingCharParam, &leavingChar);
	
	CFStringRef escapedStr = NULL;
	escapedStr = CFURLCreateStringByAddingPercentEscapes(NULL, originalStr, leavingChar, additionalChar, kCFStringEncodingUTF8);
	
	err = putStringToReply(escapedStr, kCFStringEncodingUTF8, reply);
bail:	

	--gAdditionReferenceCount;  // don't forget to decrement the reference count when you leave!
	safeRelease(originalStr);
	safeRelease(escapedStr);
	return err;
}