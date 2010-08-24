#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

#define kURLunescapeSuite  'ueUR'
#define kURLunescapeEvent	'ueUR'
#define kURLescapeEvent 'esUR'
#define kAdditionalCharParam 'adCh'
#define kLeavingCharParam 'lvCh'
#define kEncodingParam 'enCO'

#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_5
typedef SInt32                          SRefCon;
#endif

OSErr unPersentEscape(const AppleEvent *ev, AppleEvent *reply, SRefCon refcon);
OSErr persentEscape(const AppleEvent *ev, AppleEvent *reply, SRefCon refcon);