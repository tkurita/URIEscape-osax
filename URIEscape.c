#include "URIEscape.h"
#include "AEUtils.h"

#define useLog 0

extern UInt32 gAdditionReferenceCount;

#pragma mark AppleEvent handlers

OSErr unPersentEscape(const AppleEvent *ev, AppleEvent *reply, long refcon)
{
	
	++gAdditionReferenceCount;  // increment the reference count first thing!
	
	OSErr err;

	CFStringRef urlStr = NULL;
	err = getStringValue(ev, keyDirectObject, &urlStr);
	if (urlStr == NULL) goto bail;
	if (CFStringGetLength(urlStr) == 0) {
		err = putStringToReply(urlStr, kCFStringEncodingUTF8, reply);
		goto bail;
	}

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
		isLocalFile = kCFCompareEqualTo;
		if (theHost != NULL) {
			isLocalFile = CFStringCompare(theHost, CFSTR("localhost"), 0);
		}
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
	CFStringRef additionalChar = NULL;
	CFStringRef leavingChar = NULL;
	CFStringRef escapedStr = NULL;
	CFStringRef encodingName = NULL;
	CFStringEncoding encodingID = kCFStringEncodingUTF8;
	err = getStringValue(ev, keyDirectObject, &originalStr);
	if (originalStr == NULL) goto bail;
	if (CFStringGetLength(originalStr) == 0) {
		escapedStr = originalStr;
	}
	else {	
		err = getStringValue(ev, kAdditionalCharParam, &additionalChar);
		if (err != noErr) fprintf(stderr, "fail to get additional char with error :%d\n",err);
		err = getStringValue(ev, kLeavingCharParam, &leavingChar);
		if (err != noErr) fprintf(stderr, "fail to get leaving char with error :%d\n",err);
		err = getStringValue(ev, kEncodingParam, &encodingName)
		if (CFStringGetLength(encodingName)) {
			encodingID = CFStringConvertIANACharSetNameToEncoding(encodingName);
			if (kCFStringEncodingInvalidId == encodingID) {
				fprintf(stderr, "encoding name is invalid\n",);
			}
		}
		escapedStr = CFURLCreateStringByAddingPercentEscapes(
			NULL, originalStr, leavingChar, additionalChar, encodingID);
	}
	err = putStringToReply(escapedStr, kCFStringEncodingUTF8, reply);
	if (err != noErr) fprintf(stderr, "fail to setup reply with error :%d\n",err);
bail:	

	--gAdditionReferenceCount;  // don't forget to decrement the reference count when you leave!
	safeRelease(originalStr);
	safeRelease(escapedStr);
	safeRelease(additionalChar);
	safeRelease(leavingChar);

	return err;
}