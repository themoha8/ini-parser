#!/bin/bash

gcc -Wall unittest.c ../ini_parser.c -o unittest_default
./unittest_default > baseline_default.txt
rm -f unittest_default

gcc -Wall unittest.c ../ini_parser.c -DINI_MAX_LINE=20 \
-o unittest_max_line_twenty
./unittest_max_line_twenty > baseline_max_line_twenty.txt
rm -f unittest_max_line_twenty

gcc -Wall unittest.c ../ini_parser.c -DINI_ALLOW_MULTILINE=0 \
-o unittest_disallow_multiline
./unittest_disallow_multiline > baseline_disallow_multiline.txt
rm -f unittest_disallow_multiline

gcc -Wall unittest.c ../ini_parser.c -DINI_ALLOW_INLINE_COMMENTS=0 \
-o unittest_disallow_inline_comments
./unittest_disallow_inline_comments > baseline_disallow_inline_comments.txt
rm -f unittest_disallow_inline_comments

gcc -Wall unittest.c ../ini_parser.c -DINI_STOP_ON_FIRST_ERROR=1 \
-o unittest_stop_on_first_error
./unittest_stop_on_first_error > baseline_stop_on_first_error.txt
rm -f unittest_stop_on_first_error

gcc -Wall unittest_string.c ../ini_parser.c -DINI_MAX_LINE=20 \
-DINI_HANDLER_LINENO=0 -o unittest_string
./unittest_string > baseline_unittest_string.txt
rm -f unittest_string
