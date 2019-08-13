/**
 * @file
 * @brief uname utility
 * @author James Humphrey (humphreyj@somnisoft.com)
 *
 * This software has been placed into the public domain using CC0.
 */

#include <sys/utsname.h>
#include <err.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef TEST
/**
 * Declare some functions with extern linkage, allowing the test suite to call
 * those functions.
 */
# define LINKAGE extern
# include "../test/seams.h"
#else /* !(TEST) */
/**
 * Define all functions as static when not testing.
 */
# define LINKAGE static
#endif /* TEST */

/**
 * @defgroup uname_flag uname flags
 *
 * Option flags when running uname.
 */

/**
 * Print all system information.
 *
 * Corresponds to the (-a) argument.
 *
 * @ingroup uname_flag
 */
#define UNAME_FLAG_ALL       ((unsigned int)(0xFF))

/**
 * Print hardware type.
 *
 * Corresponds to the (-m) argument.
 *
 * @ingroup uname_flag
 */
#define UNAME_FLAG_HWTYPE    ((unsigned int)(1 << 0))

/**
 * Print network name.
 *
 * Corresponds to the (-n) argument.
 *
 * @ingroup uname_flag
 */
#define UNAME_FLAG_NETNAME   ((unsigned int)(1 << 1))

/**
 * Print operating system release version.
 *
 * Corresponds to the (-r) argument.
 *
 * @ingroup uname_flag
 */
#define UNAME_FLAG_OSRELEASE ((unsigned int)(1 << 2))

/**
 * Print operating system name.
 *
 * Corresponds to the (-s) argument.
 *
 * @ingroup uname_flag
 */
#define UNAME_FLAG_OSNAME    ((unsigned int)(1 << 3))

/**
 * Print operating system version.
 *
 * Corresponds tot he (-v) argument.
 *
 * @ingroup uname_flag
 */
#define UNAME_FLAG_OSVER     ((unsigned int)(1 << 4))

/**
 * uname program context.
 */
struct uname_ctx{
  /**
   * See @ref uname_flag.
   */
  unsigned int flags;

  /**
   * Exit status set to one of the following values.
   *   - EXIT_SUCCESS
   *   - EXIT_FAILURE
   */
  int status_code;

  /**
   * Set to true if already prined at least one system info section.
   *
   * Used to check when to add a space character in between sections.
   */
  bool has_printed;

  /**
   * Alignment padding.
   */
  char pad[3];
};

/**
 * Print an error message to STDERR and set an error status code.
 *
 * @param[in,out] uname_ctx See @ref uname_ctx.
 * @param[in]     errno_msg Include a standard message describing errno.
 * @param[in]     fmt       Format string used by vwarn.
 */
static void
uname_warn(struct uname_ctx *const uname_ctx,
           const bool errno_msg,
           const char *const fmt, ...){
  va_list ap;

  uname_ctx->status_code = EXIT_FAILURE;
  va_start(ap, fmt);
  if(errno_msg){
    vwarn(fmt, ap);
  }
  else{
    vwarnx(fmt, ap);
  }
  va_end(ap);
}

/**
 * Print a section of text if set in @ref uname_ctx::flags.
 *
 * Each section separated by a space character.
 *
 * @param[in,out] uname_ctx See @ref uname_ctx.
 * @param[in]     flag      One section selected from @ref uname_flag.
 * @param[in]     text      Text to print.
 */
static void
uname_print(struct uname_ctx *const uname_ctx,
            const unsigned int flag,
            const char *const text){
  if(uname_ctx->flags & flag){
    if(uname_ctx->has_printed){
      if(putchar(' ') == EOF){
        uname_warn(uname_ctx, true, "putchar");
      }
    }
    if(fputs(text, stdout) == EOF){
      uname_warn(uname_ctx, true, "fputs");
    }
    uname_ctx->has_printed = true;
  }
}

/**
 * Main entry point for uname utility.
 *
 * Usage:
 * uname [-amnrsv]
 *
 * @param[in]     argc         Number of arguments in @p argv.
 * @param[in,out] argv         Argument list.
 * @retval        EXIT_SUCCESS Successful.
 * @retval        EXIT_FAILURE Error occurred.
 */
LINKAGE int
uname_main(int argc,
           char *argv[]){
  int c;
  struct uname_ctx uname_ctx;
  struct utsname utsname;

  memset(&uname_ctx, 0, sizeof(uname_ctx));
  while((c = getopt(argc, argv, "amnrsv")) != -1){
    switch(c){
      case 'a':
        uname_ctx.flags |= UNAME_FLAG_ALL;
        break;
      case 'm':
        uname_ctx.flags |= UNAME_FLAG_HWTYPE;
        break;
      case 'n':
        uname_ctx.flags |= UNAME_FLAG_NETNAME;
        break;
      case 'r':
        uname_ctx.flags |= UNAME_FLAG_OSRELEASE;
        break;
      case 's':
        uname_ctx.flags |= UNAME_FLAG_OSNAME;
        break;
      case 'v':
        uname_ctx.flags |= UNAME_FLAG_OSVER;
        break;
      default:
        uname_ctx.status_code = EXIT_FAILURE;
        break;
    }
  }
  argc -= optind;
  argv += optind;

  if(uname_ctx.status_code == 0){
    if(argc > 0){
      uname_warn(&uname_ctx, false, "too many operands: %s", argv[0]);
    }
    else{
      if(uname(&utsname) < 0){
        uname_warn(&uname_ctx, true, "uname");
      }
      else{
        if(uname_ctx.flags == 0){
          uname_ctx.flags |= UNAME_FLAG_OSNAME;
        }
        uname_print(&uname_ctx, UNAME_FLAG_OSNAME, utsname.sysname);
        uname_print(&uname_ctx, UNAME_FLAG_NETNAME, utsname.nodename);
        uname_print(&uname_ctx, UNAME_FLAG_OSRELEASE, utsname.release);
        uname_print(&uname_ctx, UNAME_FLAG_OSVER, utsname.version);
        uname_print(&uname_ctx, UNAME_FLAG_HWTYPE, utsname.machine);
        if(putchar('\n') == EOF){
          uname_warn(&uname_ctx, true, "putchar");
        }
      }
    }
  }
  return uname_ctx.status_code;
}

#ifndef TEST
/**
 * Main program entry point.
 *
 * @param[in]     argc See @ref uname_main.
 * @param[in,out] argv See @ref uname_main.
 */
int
main(int argc,
     char *argv[]){
  return uname_main(argc, argv);
}
#endif /* TEST */

