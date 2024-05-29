/* inih -- unit tests
 *
 * This works simply by dumping a bunch of info to standard output, which is
 * redirected to an output file (baseline_*.txt) and checked into the Subversion
 * repository. This baseline file is the test output, so the idea is to check it
 * once, and if it changes -- look at the diff and see which tests failed.
 *
 * See and unittest.sh for how to run this.
*/

#include <stdio.h>
#include <string.h>
#include "../ini_parser.h"

char prev_section[INI_MAX_SECTION];

#if INI_HANDLER_LINENO
int dumper(void *userdata, const char *section, const char *name,
           const char *value, int lineno)
#else
int dumper(void *userdata, const char *section, const char *name,
           const char *value)
#endif
{
    if (strcmp(section, prev_section)) {
        printf("[%s]\n",section);
        ini_strncpy(prev_section, section, sizeof(prev_section));
    }

#if INI_HANDLER_LINENO
    printf("%s=%s ; line %d\n", *name ? name : "null" , *value ? value : "null",
           lineno);
#else
    printf("%s=%s\n", *name ? name : "null" , *value ? value : "null");
#endif

    return strcmp(name, "user")==0 && strcmp(value, "parse_error")==0 ? 0 : 1;
}

void parse(const char* filename) {
    int error;
    FILE *fd;

    fd = fopen(filename, "r");
    if (!fd) {
        fprintf(stderr, "unittest: can't open %s\n", filename);
        return ;
    }

    *prev_section = '\0';
    
    error = ini_parse_stream((ini_reader)fgets, dumper, NULL, fd);
    printf("\n%s: line number with error = %d\n", filename, error);
    fclose(fd);
}

int main(void)
{
    parse("normal.ini");
    printf("\n---------------------------------\n");
    parse("bad_section.ini");
    printf("\n---------------------------------\n");
    parse("bad_section2.ini");
    printf("\n---------------------------------\n");
    parse("bad_section3.ini");
    printf("\n---------------------------------\n");
    parse("bad_comment.ini");
    printf("\n---------------------------------\n");
    parse("user_error.ini");
    printf("\n---------------------------------\n");
    parse("multi_line.ini");
    printf("\n---------------------------------\n");
    parse("duplicate_sections.ini");

    return 0;
}
