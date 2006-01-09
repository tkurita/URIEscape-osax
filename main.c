#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

#define useLog 0

#define kURLunescapeSuite  'ueUR'
#define kURLunescapeEvent	'ueUR'

UInt32			gAdditionReferenceCount = 0;
CFBundleRef		gAdditionBundle;

// =============================================================================
// == Entry points.

static OSErr InstallMyEventHandlers();
static void RemoveMyEventHandlers();
//void SATerminate();

OSErr SAInitialize(CFBundleRef theBundle)
{
#if useLog
	printf("start SAInitialize\n");
#endif	
	gAdditionBundle = theBundle;  // no retain needed.
	
	return InstallMyEventHandlers();
}

void SATerminate()
{
	RemoveMyEventHandlers();
}

Boolean SAIsBusy()
{
	return gAdditionReferenceCount != 0;
}

OSErr MyEventHandler(const AppleEvent *ev, AppleEvent *reply, long refcon)
{
	
	++gAdditionReferenceCount;  // increment the reference count first thing!
	
	OSErr err;

#if useLog
	Handle result;
	OSStatus resultStatus;
	resultStatus = AEPrintDescToHandle(ev,&result);
	printf("%s\n",*result);
#endif
	
	AEDesc givenDesc;
	err = AEGetParamDesc (ev, keyDirectObject, typeUnicodeText, &givenDesc);
	
	Size theLength = AEGetDescDataSize(&givenDesc);
	UInt8 *theData = malloc(theLength);
	if (theLength != 0) {
		err = AEGetDescData(&givenDesc, theData, theLength);
	}
	
	if (err != noErr) {
		free(theData);
		--gAdditionReferenceCount;
		return err;
	}
	CFStringRef urlStr = CFStringCreateWithBytes(NULL, theData, theLength, kCFStringEncodingUnicode, false);
	CFURLRef theURL = CFURLCreateWithString(NULL, urlStr, NULL);
	
	CFStringRef theScheme = CFURLCopyScheme(theURL);
	CFComparisonResult isFileScheme = CFStringCompare (theScheme, CFSTR("file"), 0);
	CFRelease(theScheme);
		
#define bufferSize 4096	
	char buffer[bufferSize];	
	if (isFileScheme == 0) {
		CFURLGetFileSystemRepresentation(theURL, true, (UInt8 *)buffer, bufferSize);
#if useLog
		printf("%s:%i\n",buffer,strlen(buffer));
#endif
	}
	else {
		CFStringRef unescapedURL = CFURLCreateStringByReplacingPercentEscapes(NULL, urlStr, CFSTR(""));
		CFStringGetCString(unescapedURL, buffer, bufferSize, kCFStringEncodingUTF8);
		CFRelease(unescapedURL);
	}
	
	free(theData);
	CFRelease(urlStr);
	CFRelease(theURL);

	AEDesc resultDesc;
	err=AECreateDesc (typeUTF8Text, buffer, strlen(buffer), &resultDesc);
	if (err != noErr) {
		--gAdditionReferenceCount;
		return err;
	}
	
	err=AEPutParamDesc(reply, keyAEResult,&resultDesc);
	if (err != noErr) {
		AEDisposeDesc(&resultDesc);
	}
	
	--gAdditionReferenceCount;  // don't forget to decrement the reference count when you leave!
	
	return err;
}

struct AEEventHandlerInfo {
	FourCharCode			evClass, evID;
	AEEventHandlerProcPtr	proc;
};
typedef struct AEEventHandlerInfo AEEventHandlerInfo;

static const AEEventHandlerInfo gEventInfo[] = {
	{ kURLunescapeSuite, kURLunescapeEvent, MyEventHandler }
	// Add more suite/event/handler triplets here if you define more than one command.
};

#define kEventHandlerCount  (sizeof(gEventInfo) / sizeof(AEEventHandlerInfo))

static AEEventHandlerUPP gEventUPPs[kEventHandlerCount];

static OSErr InstallMyEventHandlers()
{
	OSErr		err;
	size_t		i;
	
	for (i = 0; i < kEventHandlerCount; ++i) {
		if ((gEventUPPs[i] = NewAEEventHandlerUPP(gEventInfo[i].proc)) != NULL)
			err = AEInstallEventHandler(gEventInfo[i].evClass, gEventInfo[i].evID, gEventUPPs[i], 0, true);
		else
			err = memFullErr;
		
		if (err != noErr) {
			SATerminate();  // Call the termination function ourselves, because the loader won't once we fail.
			return err;
		}
	}
	
	return noErr; 
}

static void RemoveMyEventHandlers()
{
	OSErr		err;
	size_t		i;
	//printf("start RemoveMyEventHandlers\n");
	for (i = 0; i < kEventHandlerCount; ++i) {
		//printf("%i\n",i);
		if (gEventUPPs[i] &&
			(err = AERemoveEventHandler(gEventInfo[i].evClass, gEventInfo[i].evID, gEventUPPs[i], true)) == noErr)
		{
			DisposeAEEventHandlerUPP(gEventUPPs[i]);
			gEventUPPs[i] = NULL;
		}
	}
	//printf("end RemoveMyEventHandlers\n");
}

int main(int argc, char *argv[]) // for debugging
{
	InstallMyEventHandlers();
    RunApplicationEventLoop();
	RemoveMyEventHandlers();
	return 0;
}
