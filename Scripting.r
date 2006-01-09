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
		"Convert file URL to unescaped file path.",
		'ueUR',
		1,
		1,
		{
			/* Events */

			"URL Unescape",
			"convert file URL to file path.",
			'ueUR', 'ueUR',
			'TEXT',
			"Unescaped file path.",
			replyRequired, singleItem, notEnumerated, Reserved13,
			'TEXT',
			"file URL",
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
