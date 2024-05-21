/* Parse given INI-style file. May have [section]s, name=value pairs
 * (whitespace stripped), and comments starting with ';' (semicolon).
 * name:value pairs are also supported as a concession to Python's 
 * configparser.
 *
 * For each name=value pair parsed, call handler function with given user
 * pointer as well as section, name, and value (data only valid for duration
 * of handler call).
 */
#include "ini_parser.h"

/* Stop parsing on first error (default is to keep parsing). */
#ifndef INI_STOP_ON_FIRST_ERROR
#define INI_STOP_ON_FIRST_ERROR 0
#endif

/* Nonzero to allow inline comments. Set to 0 to turn off and match Python 3.2+ 
 * configparser behaviour.
 */
#ifndef INI_ALLOW_INLINE_COMMENTS
#define INI_ALLOW_INLINE_COMMENTS 1
#endif
#ifndef INI_INLINE_COMMENT_PREFIXES
#define INI_INLINE_COMMENT_PREFIXES ";"
#endif

/* Nonzero to allow multi-line value parsing, in the style of Python's
 * configparser. If allowed, ini_parse() will call the handler with the same
 * name for each subsequent line parsed.
 */
#ifndef INI_ALLOW_MULTILINE
#define INI_ALLOW_MULTILINE 1
#endif

enum {
	NULL = 0
};

/* Locate character in string */
static char *ini_strchr(const char *s, char c)
{
	while (*s != '\0') {
		if (*s == c)
			return (char*)s;
		s++;
	}

	return (char*)NULL;
}

/* Return length of string variable s */
int ini_strlen(const char *s)
{
	int i;

	for (i = 0; s[i] != '\0'; i++)
		;

	return i;
}

/* Strip whitespace chars off end of given string. Return s. */
static char *ini_strip_wschars_from_end(char *s)
{
	char *p = s + ini_strlen(s)-1;

	while (p > s && (*p == ' ' || *p == '\t' || *p == '\n' ||
					 *p == '\v' || *p == '\f' || *p == '\r')) {
		*p = '\0';
		p--;
	}

	return s;
}

/* Return pointer to first non-whitespace char in given string. */
static char *ini_skip_wschars_from_start(const char *s)
{

    while (*s != '\0' && (*s == ' ' || *s == '\t' || *s == '\n' ||
				  *s == '\v' || *s == '\f' || *s == '\r'))
        s++;
    return (char*)s;
}

/* Similar to strncpy, but ensures dest (size bytes) is NUL-terminated, and 
 * doesn't pad with NULs.
 */
char *ini_strncpy(char *dest, const char *src, int size)
{
    int i;
    for (i = 0; i < size - 1 && src[i]; i++)
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;
}

/* Return pointer to first char (of chars) or inline comment in given string,
 * or pointer to NUL at end of string if neither found. Inline comment must
 * be prefixed by a whitespace character to register as a comment.
 */
static char *ini_find_chars_or_comment(const char *s, const char *chars)
{
#if INI_ALLOW_INLINE_COMMENTS
	/* if chars == NULL then don't search in string, but search a comment. */
	while (*s != '\0' && (!chars || !ini_strchr(chars, *s)) && 
		   !(ini_strchr(INI_INLINE_COMMENT_PREFIXES, *s))) {
		s++; 
	}
#else
	while (*s != '\0' && !ini_strchr(chars, *s))
		s++;
#endif

    return (char *)s;
}

/* Same as ini_parse(), but takes an ini_reader function pointer instead of
 * filename. Used for implementing custom or string-based I/O (see also
 * ini_parse_string). Return 0 on success, line number of first error on parse 
 * error
 */
int ini_parse_stream(ini_reader reader, ini_handler handler, void *userdata,
						void *stream)
{
	char line[INI_MAX_LINE], section[INI_MAX_SECTION];
	char prev_name[INI_MAX_NAME];
	char *start, *end, *name, *value;
	int lineno = 0, error = 0, max_line = INI_MAX_LINE;

	*prev_name = '\0';
	*section = '\0';
	
	while (reader(line, (int)max_line, stream) != NULL) {
		lineno++;
		start = line;

		start = ini_skip_wschars_from_start(start);

		if (*start == ';' || *start == '#') {
			/* Start-of-line comment */
			continue;
		}
#if INI_ALLOW_MULTILINE
		else if (*prev_name && *start != '\0' && start > line) {
#if INI_ALLOW_INLINE_COMMENTS
			end = ini_find_chars_or_comment(start, (const char*)NULL);
			if (*end)
				*end = '\0';
			ini_strip_wschars_from_end(start);
#endif
			/* Non-blank line with leading whitespace, treat as continuation
			 * of previous name's value (as per Python configparser).
			 *
			 * !error needed to return the first line where a error
		     * occured, not the last
		     */
			if (!HANDLER(userdata, section, prev_name, start) && !error)
				error = lineno;
		}
#endif
		else if (*start == '[') {
			/* If there is an error in a section, indicate 
			 * the line number correctly
			 */
			lineno--;
			/* A "[section]" line */
			start = ini_skip_wschars_from_start(start+1);
			end = ini_strchr(start+1, ']');
			if (*end == ']') {
				*end = '\0';
				end = ini_strip_wschars_from_end(start);
				ini_strncpy(section, start, sizeof(section));
				*prev_name = '\0';
			}
			else if (!error)
				/* No ']' found on section line */
				error = lineno;
		}
		else if (*start) {
			/* Not a comment, must be a name[=:]value pair */
			end = ini_find_chars_or_comment(start, "=:");
			if (*end == '=' || *end == ':') {
				*end = '\0';
				name = ini_strip_wschars_from_end(start);
				value = end + 1;

#if INI_ALLOW_INLINE_COMMENTS
				end = ini_find_chars_or_comment(value, (const char*)NULL);
				/* if comment found */
				if (*end)
					*end = '\0';
#endif
				value = ini_skip_wschars_from_start(value);
				ini_strip_wschars_from_end(value);

				/* Valid name[=:]value pair found, call handler */
				ini_strncpy(prev_name, name, sizeof(prev_name));
				/* !error needed to return the first line where a error
				 * occured, not the last */
				if (!HANDLER(userdata, section, name, value) && !error)
					error = lineno;
			}
			else
				/* No '=' or ':' found on name[=:]value line */
				error = lineno;
		}
#if INI_STOP_ON_FIRST_ERROR
		/* stop on first error */
        if (error)
            break;
#endif
	}
	return error;
}
