#include "URIEscape.h"
#include "AEUtils.h"

#define useLog 0

extern UInt32 gAdditionReferenceCount;

#pragma mark AppleEvent handlers

#define STR(s) _STR(s)
#define _STR(s) #s 

OSErr URIEscapeVersionHandler(const AppleEvent *ev, AppleEvent *reply, SRefCon refcon)
{
#if useLog
	fprintf(stderr, "start URIEscapeVersionHandler\n");
#endif			
	gAdditionReferenceCount++;
	OSErr err;
	err = putStringToEvent(reply, keyAEResult, CFSTR(STR(URIEscape_VERSION)), kCFStringEncodingUnicode);
	gAdditionReferenceCount--;
	return err;
}

OSErr unPersentEscape(const AppleEvent *ev, AppleEvent *reply, SRefCon refcon)
{
	
	++gAdditionReferenceCount;  // increment the reference count first thing!
	
	OSErr err;

	CFURLRef theURL = NULL;
	CFStringRef theScheme = NULL;
	
	CFStringRef urlStr = CFStringCreateWithEvent(ev, keyDirectObject, &err);
	
	if (!urlStr) goto bail;
	if (CFStringGetLength(urlStr) == 0) {
		err = putStringToEvent(reply, keyAEResult, urlStr, kCFStringEncodingUTF8);
		goto bail;
	}
	
	CFStringEncoding encodingID = kCFStringEncodingUTF8;
	CFStringRef encodingName = CFStringCreateWithEvent(ev, kEncodingParam, &err);
	if ((err != noErr) && (err != errAEDescNotFound)) {
		putStringToEvent(reply, keyErrorString, 
						 CFSTR("Failed to get an encoding name with error."), kCFStringEncodingUTF8);
		goto bail;
	}
	if (encodingName) {
		encodingID = CFStringConvertIANACharSetNameToEncoding(encodingName);
		if (kCFStringEncodingInvalidId == encodingID) {
			err = 2000;
			putStringToEvent(reply, keyErrorString, 
							 CFSTR("The encoding name is invalid."), kCFStringEncodingUTF8);
			goto bail;
		}
	}	

	CFComparisonResult isFileScheme = kCFCompareLessThan;
	CFComparisonResult isLocalFile = kCFCompareLessThan;
	
	if (encodingID == kCFStringEncodingUTF8) {
		theURL = CFURLCreateWithString(NULL, urlStr, NULL);
		if (theURL) {
			theScheme = CFURLCopyScheme(theURL);
		}
		if (theScheme) {
			isFileScheme = CFStringCompare(theScheme, CFSTR("file"), 0);
			CFRelease(theScheme);
		
			if (isFileScheme == kCFCompareEqualTo) {
				CFStringRef theHost = CFURLCopyHostName(theURL);
				isLocalFile = kCFCompareEqualTo;
				if (theHost) {
					isLocalFile = CFStringCompare(theHost, CFSTR("localhost"), 0);
				}
			}
		}
	}
	
	if (isLocalFile == kCFCompareEqualTo) {
		err = putFilePathToReply(theURL, reply);
	} else {
		CFStringRef unescapedURL = CFURLCreateStringByReplacingPercentEscapesUsingEncoding(NULL, urlStr, CFSTR(""), encodingID);
		if (unescapedURL) {
			err = putStringToEvent(reply, keyAEResult, unescapedURL, kCFStringEncodingUTF8);
			CFRelease(unescapedURL);
		} else {
			putStringToEvent(reply, keyErrorString, 
							 CFSTR("Failed to dencode persent escapes."), kCFStringEncodingUTF8);
			err = 2001;
		}
	}
	
bail:
	safeRelease(urlStr);
	safeRelease(theURL);
	
	--gAdditionReferenceCount;  // don't forget to decrement the reference count when you leave!
	
	return err;
}

OSErr persentEscape(const AppleEvent *ev, AppleEvent *reply, SRefCon refcon)
{
	
	++gAdditionReferenceCount;  // increment the reference count first thing!
	
	OSErr err;
#if useLog	
	showAEDesc(ev);
#endif
	
	CFStringRef additionalChar = NULL;
	CFStringRef leavingChar = NULL;
	CFStringRef escapedStr = NULL;
	CFStringRef encodingName = NULL;
	CFStringEncoding encodingID = kCFStringEncodingUTF8;
	
	CFStringRef originalStr = CFStringCreateWithEvent(ev, keyDirectObject, &err);
	if (originalStr == NULL) goto bail;
	if (CFStringGetLength(originalStr) == 0) {
		escapedStr = originalStr;
	} else {	
		additionalChar = CFStringCreateWithEvent(ev, kAdditionalCharParam, &err);
		if ((err != noErr) && (err != errAEDescNotFound)) {
			putStringToEvent(reply, keyErrorString, 
							 CFSTR("Failed to get additional characters."), kCFStringEncodingUTF8);
			goto bail;
		}
		leavingChar = CFStringCreateWithEvent(ev, kLeavingCharParam, &err);
		if ((err != noErr) && (err != errAEDescNotFound)) {
			putStringToEvent(reply, keyErrorString, 
							 CFSTR("Failed to get leaving characters."), kCFStringEncodingUTF8);
			goto bail;
		}
		encodingName = CFStringCreateWithEvent(ev, kEncodingParam, &err);
		if ((err != noErr) && (err != errAEDescNotFound)) {
			putStringToEvent(reply, keyErrorString, 
							 CFSTR("Failed to get an encoding name."), kCFStringEncodingUTF8);
			goto bail;
		}
		if (encodingName) {
			encodingID = CFStringConvertIANACharSetNameToEncoding(encodingName);
			if (kCFStringEncodingInvalidId == encodingID) {
				err = 2000;
				putStringToEvent(reply, keyErrorString, 
								 CFSTR("The encoding name is invalid."), kCFStringEncodingUTF8);
				goto bail;
			}
		}
		escapedStr = CFURLCreateStringByAddingPercentEscapes(
							NULL, originalStr, leavingChar, additionalChar, encodingID);
		if (!escapedStr) {
			putStringToEvent(reply, keyErrorString, 
							 CFSTR("Failed to encode persent escapes."), kCFStringEncodingUTF8);
			err = 2002;
			goto bail;			
		}
	}
	err = putStringToEvent(reply, keyAEResult, escapedStr, kCFStringEncodingUTF8);
	if (err != noErr) {
		putStringToEvent(reply, keyErrorString, 
						 CFSTR("Failed to setup reply."), kCFStringEncodingUTF8);		
	}
bail:	

	--gAdditionReferenceCount;  // don't forget to decrement the reference count when you leave!
	safeRelease(originalStr);
	safeRelease(escapedStr);
	safeRelease(additionalChar);
	safeRelease(leavingChar);

	return err;
}