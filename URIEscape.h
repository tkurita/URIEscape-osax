#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

#define kURLunescapeSuite  'ueUR'
#define kURLunescapeEvent	'ueUR'
#define kURLescapeEvent 'esUR'
#define kAdditionalCharParam 'adCh'
#define kLeavingCharParam 'lvCh'

OSErr unPersentEscape(const AppleEvent *ev, AppleEvent *reply, long refcon);
OSErr persentEscape(const AppleEvent *ev, AppleEvent *reply, long refcon);