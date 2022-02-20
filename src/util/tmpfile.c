// ReSharper disable CppInconsistentNaming
#ifndef __USE_ISOC99
# define __USE_ISOC99 1 //NOLINT
#endif
#ifndef __USE_ISOC11
# define __USE_ISOC11 1 //NOLINT
#endif

#include "Common.hh"
#include "util/c_util.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef DUMB_TEMPNAME_NUM_CHARS
# define NUM_RANDOM_CHARS DUMB_TEMPNAME_NUM_CHARS
#else
# define NUM_RANDOM_CHARS 6
#endif

#ifdef _WIN32
# define FILESEP_CHAR '\\'
# define FILESEP_STR  "\\"
# define CHAR_IS_FILESEP(ch) ((ch) == '\\' || (ch) == '/')
# ifndef PATH_MAX
#  if WDK_NTDDI_VERSION >= NTDDI_WIN10_RS1
#   define PATH_MAX 4096
#  else
#   define PATH_MAX MAX_PATH
#  endif
# endif
#else
# define FILESEP_CHAR '/'
# define FILESEP_STR  "/"
# define CHAR_IS_FILESEP(ch) ((ch) == '/')
#endif

#define RAND() cxx_random_engine_get_random_val_32()

static char *get_random_chars(char *buf);
static bool  file_exists(char const *fname);

/*======================================================================================*/

size_t
braindead_tempname(_Notnull_   char       *restrict const buf,
                   _Notnull_   char const *restrict const dir,
                   _Maybenull_ char const *restrict const prefix,
                   _Maybenull_ char const *restrict const suffix)
{
      /* Microsoft's libc doesn't include stpcpy, and I can't bring myself to use strcat,
       * so this is about the best way I can think of to do this. Here's hoping the
       * compiler is smart enough to work out what's going on. */

      size_t len = strlen(dir);
      memcpy(buf, dir, len + 1);

      if (len > 0 && !CHAR_IS_FILESEP(buf[len - 1]))
            buf[len++] = FILESEP_CHAR;

      char *ptr = buf + len;

      if (prefix) {
            len = strlen(prefix);
            memcpy(ptr, prefix, len);
            ptr += len;
      }

      char *const backup_ptr = ptr;
      if (suffix)
            len = strlen(suffix);

      do {
            ptr = get_random_chars(backup_ptr);

            if (suffix) {
                  memcpy(ptr, suffix, len);
                  ptr += len;
            }

            *ptr = '\0';
      } while (file_exists(buf));

      return ptr - buf;
}

static char *
get_random_chars(char *buf)
{
      char *ptr = buf;

      for (int i = 0; i < NUM_RANDOM_CHARS; ++i) {
            uint32_t const tmp = RAND();

            if ((tmp & 0x0F) < 2U)
                  *ptr++ = (char)((RAND() % 10) + '0');
            else
                  *ptr++ = (char)((RAND() % 26) + ((tmp & 1) ? 'A' : 'a'));
      }

      return ptr;
}

static bool
file_exists(char const *fname)
{
      struct stat st;
      errno = 0;
      stat(fname, &st);
      return errno != ENOENT;
}
