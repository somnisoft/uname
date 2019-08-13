/**
 * @file
 * @brief test suite
 * @author James Humphrey (humphreyj@somnisoft.com)
 *
 * This software has been placed into the public domain using CC0.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "test.h"

/**
 * Generate the test output of the uname utility to this file.
 */
#define PATH_TMP_FILE "build/test-uname.txt"

/**
 * Number of arguments in @ref g_argv.
 */
static int
g_argc;

/**
 * Argumnet list to pass to utility.
 */
static char **
g_argv;

/**
 * Call @ref uname_main with the given arguments.
 *
 * @param[in] a                  See @ref uname_main.
 * @param[in] m                  See @ref uname_main.
 * @param[in] n                  See @ref uname_main.
 * @param[in] r                  See @ref uname_main.
 * @param[in] s                  See @ref uname_main.
 * @param[in] v                  See @ref uname_main.
 * @param[in] invalid_arg        Invalid argument.
 * @param[in] extra_operand      Add an operand.
 * @param[in] expect_ref_file    Reference file containing expected uname
 *                               output.
 * @param[in] expect_exit_status Expected exit status code.
 */
static void
test_uname_main(const bool a,
                const bool m,
                const bool n,
                const bool r,
                const bool s,
                const bool v,
                const bool invalid_arg,
                const bool extra_operand,
                const char *const expect_ref_file,
                const int expect_exit_status){
  pid_t pid;
  int status;
  int exit_status;
  int cmp_exit_status;
  FILE *new_stdout;
  char cmp_cmd[1000];

  g_argc = 1;
  if(a){
    strcpy(g_argv[g_argc++], "-a");
  }
  if(m){
    strcpy(g_argv[g_argc++], "-m");
  }
  if(n){
    strcpy(g_argv[g_argc++], "-n");
  }
  if(r){
    strcpy(g_argv[g_argc++], "-r");
  }
  if(s){
    strcpy(g_argv[g_argc++], "-s");
  }
  if(v){
    strcpy(g_argv[g_argc++], "-v");
  }
  if(invalid_arg){
    strcpy(g_argv[g_argc++], "-z");
  }
  if(extra_operand){
    strcpy(g_argv[g_argc++], "operand");
  }

  pid = fork();
  assert(pid >= 0);
  if(pid == 0){
    new_stdout = freopen(PATH_TMP_FILE, "w", stdout);
    assert(new_stdout);
    exit_status = uname_main(g_argc, g_argv);
    exit(exit_status);
  }
  assert(waitpid(pid, &status, 0) == pid);
  assert(WIFEXITED(status));
  assert(WEXITSTATUS(status) == expect_exit_status);
  if(expect_ref_file){
    sprintf(cmp_cmd, "cmp '%s' '%s'", PATH_TMP_FILE, expect_ref_file);
    cmp_exit_status = system(cmp_cmd);
    assert(cmp_exit_status == 0);
  }
}

/**
 * Run all test cases for the uname utility.
 */
static void
test_all(void){
  int i;

  /* No arguments given. */
  test_uname_main(false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  "build/s.txt",
                  EXIT_SUCCESS);

  /* -a */
  test_uname_main(true,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  "build/a.txt",
                  EXIT_SUCCESS);

  /* -m */
  test_uname_main(false,
                  true,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  "build/m.txt",
                  EXIT_SUCCESS);
  /* -n */
  test_uname_main(false,
                  false,
                  true,
                  false,
                  false,
                  false,
                  false,
                  false,
                  "build/n.txt",
                  EXIT_SUCCESS);
  /* -r */
  test_uname_main(false,
                  false,
                  false,
                  true,
                  false,
                  false,
                  false,
                  false,
                  "build/r.txt",
                  EXIT_SUCCESS);
  /* -s */
  test_uname_main(false,
                  false,
                  false,
                  false,
                  true,
                  false,
                  false,
                  false,
                  "build/s.txt",
                  EXIT_SUCCESS);
  /* -v */
  test_uname_main(false,
                  false,
                  false,
                  false,
                  false,
                  true,
                  false,
                  false,
                  "build/v.txt",
                  EXIT_SUCCESS);

  /* Invalid argument. */
  test_uname_main(false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  true,
                  false,
                  NULL,
                  EXIT_FAILURE);

  /* Too many operands. */
  test_uname_main(false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  true,
                  NULL,
                  EXIT_FAILURE);

  /* fputs() fail. */
  g_test_seam_err_ctr_fputs = 0;
  test_uname_main(true,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  NULL,
                  EXIT_FAILURE);
  g_test_seam_err_ctr_fputs = -1;

  /* putchar() fail. */
  for(i = 0; i < 5; i++){
    g_test_seam_err_ctr_putchar = i;
    test_uname_main(true,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false,
                    NULL,
                    EXIT_FAILURE);
    g_test_seam_err_ctr_putchar = -1;
  }

  /* uname() fail. */
  g_test_seam_err_ctr_uname = 0;
  test_uname_main(true,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  false,
                  NULL,
                  EXIT_FAILURE);
  g_test_seam_err_ctr_uname = -1;
}

/**
 * Test uname utility.
 *
 * Usage: test
 *
 * @retval 0 All tests passed.
 */
int
main(void){
  const size_t MAX_ARGS = 20;
  const size_t MAX_ARG_LEN = 255;
  size_t i;

  g_argv = malloc(MAX_ARGS * sizeof(g_argv));
  assert(g_argv);
  for(i = 0; i < MAX_ARGS; i++){
    g_argv[i] = malloc(MAX_ARG_LEN * sizeof(*g_argv));
    assert(g_argv[i]);
  }
  g_argc = 0;
  strcpy(g_argv[g_argc++], "uname");
  test_all();
  for(i = 0; i < MAX_ARGS; i++){
    free(g_argv[i]);
  }
  free(g_argv);
  return 0;
}

