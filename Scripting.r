#include <Carbon/Carbon.r>

#define Reserved8   reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved
#define Reserved12  Reserved8, reserved, reserved, reserved, reserved
#define Reserved13  Reserved12, reserved
#define dp_none__   noParams, "", directParamOptional, singleItem, notEnumerated, Reserved13
#define reply_none__   noReply, "", replyOptional, singleItem, notEnumerated, Reserved13
#define synonym_verb__ reply_none__, dp_none__, { }
#define plural__    "", {"", kAESpecialClassProperties, cType, "", reserved, singleItem, notEnumerated, readOnly, Reserved8, noApostrophe, notFeminine, notMasculine, plural}, {}

resource 'aete' (0, "URI Escape Terminology") {
	0x1,  // major version
	0x0,  // minor version
	english,
	roman,
	{
		"URI Escape Suite",
		"Unescape/Adding percent escape sequences in URL. ",
		'ueUR',
		1,
		1,
		{
			/* Events */

			"URI Unescape",
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
				"encoding", 'enCO', 'TEXT',
				"The encoding to use when interpreting percent escapes. The encoding name is specified with  IANA “charset” name (http://www.iana.org/assignments/character-sets). ",
				optional,
				singleItem, notEnumerated, Reserved13
			},

			"URI Escape",
			"add percent escapes to string",
			'ueUR', 'esUR',
			'TEXT',
			"",
			replyRequired, singleItem, notEnumerated, Reserved13,
			'TEXT',
			"text to add persent escapes",
			directParamRequired,
			singleItem, notEnumerated, Reserved13,
			{
				"additional", 'adCh', 'TEXT',
				"Additional characters to escape.",
				optional,
				singleItem, notEnumerated, Reserved13,
				"leaving", 'lvCh', 'TEXT',
				"Characters to leave unescaped",
				optional,
				singleItem, notEnumerated, Reserved13,
				"encoding", 'enCO', 'TEXT',
				"The encoding to use when interpreting percent escapes. The encoding name is specified with  IANA “charset” name (http://www.iana.org/assignments/character-sets). ",
				optional,
				singleItem, notEnumerated, Reserved13
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
