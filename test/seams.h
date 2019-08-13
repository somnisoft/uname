/**
 * @file
 * @brief test seams
 * @author James Humphrey (humphreyj@somnisoft.com)
 *
 * This software has been placed into the public domain using CC0.
 */
#ifndef UNAME_TEST_SEAMS_H
#define UNAME_TEST_SEAMS_H

#include "test.h"

/*
 * Redefine these functions to internal test seams.
 */
#undef fputs
#undef putchar
#undef uname

/**
 * Inject a test seam to replace fputs.
 */
#define fputs   test_seam_fputs

/**
 * Inject a test seam to replace putchar.
 */
#define putchar test_seam_putchar

/**
 * Inject a test seam to replace uname.
 */
#define uname   test_seam_uname

#endif /* UNAME_TEST_SEAMS_H */

