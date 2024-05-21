#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ini_parser.h"
#include <limits.h>

struct conf_t {
	int version;
	char *name;
	int s_name;
	char *email;
	int s_email;
};

static int check_overflow(int a, int b)
{
	if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b))
		return 1;
	/*
	if (a >= 0) {
		if (INT_MAX - a < b)
			return 1;
	}	
	else if (INT_MIN - b < a)
			return 1;
	*/
	return 0;
}

static int handler(void *userdata, const char *section, const char *name,
					const char *value, int lineno)
{
	char *tmp;
	int ntmp;
	struct conf_t *config = (struct conf_t*) userdata;

	if (strcmp(section, "protocol") == 0 && strcmp(name, "version") == 0) {
		if (config->version) {
			ntmp = atoi(value);
			if (check_overflow(config->version, ntmp))
					config->version = 0; /* overflow */
			else
				config->version += ntmp;
		}
		else
			config->version = atoi(value);
	}
	else if ((strcmp(section, "user") == 0 && strcmp(name, "name") == 0)) {
		config->s_name = ini_strlen(value);
		if (config->name) {
			if (config->s_name + ini_strlen(value) > INI_MAX_NAME) {
				fprintf(stderr, "ini_parse: too long value in strings %d\n", 
						lineno);
				return 0;
			}
			tmp = config->name + strlen(config->name);
			*tmp++ = ' ';
			ini_strncpy(tmp, value, INI_MAX_NAME);
		}
		else {
			if (config->s_name > INI_MAX_NAME) {
				fprintf(stderr, "ini_parse: too long value in string %d\n", 
						lineno);
				return 0;
			}
			/* config->name = strndup(value, INI_MAX_NAME); */
			config->name = (char*) malloc(INI_MAX_NAME);
			ini_strncpy(config->name, value, INI_MAX_NAME);
		}
	}
	else if ((strcmp(section, "user") == 0 && strcmp(name, "email") == 0)) {
		config->s_email = ini_strlen(value);
		if (config->email) {
			if (config->s_email + ini_strlen(value) > INI_MAX_NAME) {
				fprintf(stderr, "ini_parse: too long value in strings %d\n", 
						lineno);
				return 0;
			}
			tmp = config->name + strlen(config->name);
			*tmp++ = ' ';
			ini_strncpy(tmp, value, INI_MAX_NAME);
		}
		else {
			if (config->s_email > INI_MAX_NAME) {
				fprintf(stderr, "ini_parse: too long value in string %d\n", 
						lineno);
				return 0;
			}
			/* config->email = strndup(value, INI_MAX_NAME); */
			config->email = (char*) malloc(INI_MAX_NAME);
			ini_strncpy(config->email, value, INI_MAX_NAME);
		}
	}
	else
		return 0; /* unknown section/name, error */

	return 1;
}

int main(int argc, char *argv[])
{
	int lineno;
	FILE *fd;
	struct conf_t config;
	config.version = 0;
	config.name = NULL;
	config.email = NULL;

	if (argc < 2) {
		fprintf(stderr, "ini_parser: usage: ini_parser filename\n");
		return 1;
	}

	fd = fopen(argv[1], "r");
	if (!fd) {
		fprintf(stderr, "ini_parser: can't open %s\n", argv[1]);
		return 2;
	}

	lineno = ini_parse_stream((ini_reader)fgets, handler, &config, fd);
	if (lineno) {
		fprintf(stderr, "ini_parser: error while parsing file on line %d\n", lineno);
		//if (config.name)
		//	free(config.name);
		//if (config.email)
		//	free(config.email);
		//fclose(fd);
		//return lineno;
	}

	printf("Config loaded from %s: version=%d, name=%s, email=%s\n", argv[1],
		   config.version, config.name, config.email);

	if (config.name)
		free(config.name);
	if (config.email)
		free(config.email);
	fclose(fd);

	return 0;
}
