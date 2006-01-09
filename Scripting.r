#include <Carbon/Carbon.r>

#define Reserved8   reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved
#define Reserved12  Reserved8, reserved, reserved, reserved, reserved
#define Reserved13  Reserved12, reserved
#define dp_none__   noParams, "", directParamOptional, singleItem, notEnumerated, Reserved13
#define reply_none__   noReply, "", replyOptional, singleItem, notEnumerated, Reserved13
#define synonym_verb__ reply_none__, dp_none__, { }
#define plural__    "", {"", kAESpecialClassProperties, cType, "", reserved, singleItem, notEnumerated, readOnly, Reserved8, noApostrophe, notFeminine, notMasculine, plural}, {}

resource 'aete' (0, "URL Unescape Terminology") {
	0x1,  // major version
	0x0,  // minor version
	english,
	roman,
	{
		"URL Unescape Suite",
		"Unescape percent escape sequences in URL. If the protocoal of the URL is file, return POSIX path in local file system. ",
		'ueUR',
		1,
		1,
		{
			/* Events */

			"URL Unescape",
			"Unescape percent escape sequences in URL. If the protocoal of the URL is file, return POSIX path in local file system. ",
			'ueUR', 'ueUR',
			'TEXT',
			"Unescaped URL String or POSIX path.",
			replyRequired, singleItem, notEnumerated, Reserved13,
			'TEXT',
			"URL",
			directParamRequired,
			singleItem, notEnumerated, Reserved13,
			{

			}
		},
		{
			/* Classes */

		},
		{
			/* Comparisons */
		},
		{
			/* Enumerations */
		}
	}
};
