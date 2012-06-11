#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

#define kURLEscapeSuite  'ueUR'

enum eventID { 
	kURLunescapeEvent = 'ueUR' ,
	kURLescapeEvent = 'esUR' ,
	kVersionEvent = 'Vers' };

enum paramID {
	kAdditionalCharParam = 'adCh',
	kLeavingCharParam = 'lvCh',
	kEncodingParam = 'enCO'};

#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_5
typedef SInt32                          SRefCon;
#endif

OSErr URIEscapeVersionHandler(const AppleEvent *ev, AppleEvent *reply, SRefCon refcon);
OSErr unPersentEscape(const AppleEvent *ev, AppleEvent *reply, SRefCon refcon);
OSErr persentEscape(const AppleEvent *ev, AppleEvent *reply, SRefCon refcon);