/**
 * @file
 * @brief test suite
 * @author James Humphrey (humphreyj@somnisoft.com)
 *
 * This software has been placed into the public domain using CC0.
 */
#ifndef UNAME_TEST_H
#define UNAME_TEST_H

#include <sys/utsname.h>
#include <stdio.h>

int
uname_main(int argc,
           char *argv[]);

int
test_seam_fputs(const char *s,
                FILE *stream);
int
test_seam_putchar(int c);

int
test_seam_uname(struct utsname *name);

extern int g_test_seam_err_ctr_fputs;
extern int g_test_seam_err_ctr_putchar;
extern int g_test_seam_err_ctr_uname;

#endif /* UNAME_TEST_H */

