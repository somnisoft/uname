/**
 * @file
 * @brief test seams
 * @author James Humphrey (humphreyj@somnisoft.com)
 *
 * This software has been placed into the public domain using CC0.
 */

#include <errno.h>
#include <stdbool.h>

#include "test.h"

/**
 * Error counter for @ref test_seam_fputs.
 */
int g_test_seam_err_ctr_fputs = -1;

/**
 * Error counter for @ref test_seam_putchar.
 */
int g_test_seam_err_ctr_putchar = -1;

/**
 * Error counter for @ref test_seam_uname.
 */
int g_test_seam_err_ctr_uname = -1;

/**
 * Decrement an error counter until it reaches -1.
 *
 * After a counter reaches -1, it will return a true response. This gets
 * used by the test suite to denote when to cause a function to fail. For
 * example, the unit test might need to cause the malloc() function to fail
 * after calling it a third time. In that case, the counter should initially
 * get set to 2 and will get decremented every time this function gets called.
 *
 * @param[in,out] err_ctr Error counter to decrement.
 * @retval        true    The counter has reached -1.
 * @retval        false   The counter has been decremented, but did not reach
 *                        -1 yet.
 */
static bool
test_seam_dec_err_ctr(int *const err_ctr){
  bool reached_end;

  reached_end = false;
  if(*err_ctr >= 0){
    *err_ctr -= 1;
    if(*err_ctr < 0){
      reached_end = true;
    }
  }
  return reached_end;
}

/**
 * Control when fputs() fails.
 *
 * @param[in]     s      Write this string to @p stream.
 * @param[in,out] stream Write @p s to this file stream.
 * @retval        >=0    Successfully wrote @p s to @p stream.
 * @retval        EOF    Failed to write @p s to @p stream.
 */
int
test_seam_fputs(const char *s,
                FILE *stream){
  int rc;

  if(test_seam_dec_err_ctr(&g_test_seam_err_ctr_fputs)){
    rc = EOF;
    errno = EAGAIN;
  }
  else{
    rc = fputs(s, stream);
  }
  return rc;
}

/**
 * Control when putchar() fails.
 *
 * @param[in] c   Character to write.
 * @retval    c   Successfully wrote @p c to STDOUT.
 * @retval    EOF Failed to write @p c.
 */
int
test_seam_putchar(int c){
  int rc;

  if(test_seam_dec_err_ctr(&g_test_seam_err_ctr_putchar)){
    rc = EOF;
    errno = EAGAIN;
  }
  else{
    rc = putchar(c);
  }
  return rc;
}

/**
 * Control when uname() fails.
 *
 * @param[out] name Buffer to store system information.
 * @retval     >=0  Stored system information in @p name.
 * @retval     -1   Failed to get system information.
 */
int
test_seam_uname(struct utsname *name){
  int rc;

  if(test_seam_dec_err_ctr(&g_test_seam_err_ctr_uname)){
    rc = -1;
    errno = EFAULT;
  }
  else{
    rc = uname(name);
  }
  return rc;
}

