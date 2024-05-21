#ifndef INI_H_SENTRY
#define INI_H_SENTRY

/*
enum {
	INI_MAX_LINE = 128,
	INI_MAX_SECTION = 64,
	INI_MAX_NAME = 128
};
*/

enum {
	INI_MAX_LINE = 128,
	INI_MAX_SECTION = 64,
	INI_MAX_NAME = 128
};

/* Nonzero if ini_handler callback should accept lineno parameter. */
#ifndef INI_HANDLER_LINENO
#define INI_HANDLER_LINENO 1
#endif

#if INI_HANDLER_LINENO
#define HANDLER(u, s, n, v) handler(u, s, n, v, lineno)
#else
#define HANDLER(u, s, n, v) handler(u, s, n, v)
#endif

/* Typedef for prototype of handler function. */
#if INI_HANDLER_LINENO
typedef int (*ini_handler)(void *user, const char *section,
							const char *name, const char *value,
							int lineno);
#else
typedef int (*ini_handler)(void *user, const char *section,
                           const char *name, const char *value);
#endif

/* Typedef for prototype of fgets-style reader function. */
typedef char* (*ini_reader)(char *str, int num, void *stream);

int ini_parse_stream(ini_reader reader, ini_handler handler, void* userdata,
						void *stream);

char *ini_strncpy(char *dest, const char *src, int size);

int ini_strlen(const char *s);

#endif
