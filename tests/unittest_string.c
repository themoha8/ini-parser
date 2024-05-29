/* inih -- unit tests for ini_parse_string() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ini_parser.h"

char prev_section[INI_MAX_SECTION];

int dumper(void* userdata, const char* section, const char* name,
           const char* value)
{
    if (strcmp(section, prev_section)) {
        printf("[%s]\n",section);
        ini_strncpy(prev_section, section, sizeof(prev_section));
    }
    printf("%s=%s\n", *name ? name : "null" , *value ? value : "null");
    return 1;
}

void parse(const char* name, const char* string) {
    int error;

    *prev_section = '\0';
    error = ini_parse_string(string, dumper, NULL);
    printf("\n%s: line number with error = %d\n", name, error);
}

int main(void)
{
    parse("empty string", "");
    printf("\n---------------------------------\n");
    parse("basic", "[section]\nfoo = bar\nbazz = buzz quxx");
    printf("\n---------------------------------\n");
    parse("crlf", "[section]\r\nhello = world\r\nforty_two = 42\r\n");
    printf("\n---------------------------------\n");
    parse("long line", "[sec]\nfoo = 01234567890123456789\nbar=4321\n");
    printf("\n---------------------------------\n");
    parse("long continued", "[sec]\nfoo = 0123456789012bix=1234\n");
    printf("\n---------------------------------\n");
    parse("error", "[s]\na=1\nb\nc=3");
    
    return 0;
}
