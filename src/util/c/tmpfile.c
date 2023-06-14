// ReSharper disable CppInconsistentNaming
// ReSharper disable StringLiteralTypo
#ifndef __USE_ISOC99
# define __USE_ISOC99 1 //NOLINT
#endif
#ifndef __USE_ISOC11
# define __USE_ISOC11 1 //NOLINT
#endif

#include "Common.hh"
#include "util/c/c_util.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef DUMB_TEMPNAME_NUM_CHARS
# define NUM_RANDOM_CHARS DUMB_TEMPNAME_NUM_CHARS
#else
# define NUM_RANDOM_CHARS 8
#endif

#ifdef _WIN32
# define FILESEP_CHAR '\\'
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
# define CHAR_IS_FILESEP(ch) ((ch) == '/')
#endif

#define RAND() emlsp_cxx_random_engine_get_random_val_32()

static char *get_random_chars(char *buf);
static bool  file_exists(char const *fname);

#ifdef _WIN32
static wchar_t *get_random_wide_chars(wchar_t *buf);
static bool     file_exists_wide(wchar_t const *fname);
#endif

/*======================================================================================*/

size_t
braindead_tempname(_Out_ _Post_z_ char       *__restrict buf,
                   _In_z_         char const *__restrict dir,
                   _In_opt_z_     char const *__restrict prefix,
                   _In_opt_z_     char const *__restrict suffix)
{
      /* Microsoft's libc doesn't include stpcpy, and I can't bring myself to use strcat,
       * so this is about the best way I can think of to do this. Here's hoping the
       * compiler is smart enough to work out what's going on. */

      size_t len = strlen(dir);
      if (len > 0) {
            memcpy(buf, dir, len);
            if (!CHAR_IS_FILESEP(buf[len - 1]))
                  buf[len++] = FILESEP_CHAR;
      }
      buf[len] = '\0';  /* Paranoia */

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
      static char const repertoir[] =
          "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
      char *ptr = buf;

      for (int i = 0; i < NUM_RANDOM_CHARS; ++i)
            *ptr++ = repertoir[RAND() % (sizeof(repertoir) - SIZE_C(1))];

      return ptr;
}

#ifdef _WIN32
static bool
file_exists(char const *fname)
{
      DWORD const dwAttrib = GetFileAttributesA(fname);
      return dwAttrib != INVALID_FILE_ATTRIBUTES;
}
#else
static bool
file_exists(char const *fname)
{
      struct stat st;
      errno = 0;
      (void)stat(fname, &st);
      return errno != ENOENT;
}
#endif


#ifdef _WIN32
size_t
braindead_wide_tempname(_Out_ _Post_z_ wchar_t       *__restrict buf,
                        _In_z_         wchar_t const *__restrict dir,
                        _In_opt_z_     wchar_t const *__restrict prefix,
                        _In_opt_z_     wchar_t const *__restrict suffix)
{
      /* Microsoft's libc doesn't include stpcpy, and I can't bring myself to use strcat,
       * so this is about the best way I can think of to do this. Here's hoping the
       * compiler is smart enough to work out what's going on. */

      size_t len = wcslen(dir);
      if (len > 0) {
            memcpy(buf, dir, len * sizeof(wchar_t));
            if (!CHAR_IS_FILESEP(buf[len - 1]))
                  buf[len++] = FILESEP_CHAR;
      }
      buf[len] = L'\0';  /* Paranoia */

      wchar_t *ptr = buf + len;

      if (prefix) {
            len = wcslen(prefix);
            memcpy(ptr, prefix, len * sizeof(wchar_t));
            ptr += len;
      }

      wchar_t *const backup_ptr = ptr;
      if (suffix)
            len = wcslen(suffix);

      do {
            ptr = get_random_wide_chars(backup_ptr);

            if (suffix) {
                  memcpy(ptr, suffix, len * sizeof(wchar_t));
                  ptr += len;
            }

            *ptr = L'\0';
      } while (file_exists_wide(buf));

      return ptr - buf;
}

static wchar_t *
get_random_wide_chars(wchar_t *buf)
{
      static wchar_t const repertoir[] =
          L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
      wchar_t *ptr = buf;

      for (int i = 0; i < NUM_RANDOM_CHARS; ++i)
            *ptr++ = repertoir[RAND() % ((sizeof(repertoir) / sizeof(wchar_t)) - SIZE_C(1))];

      return ptr;
}

static bool
file_exists_wide(wchar_t const *fname)
{
      DWORD const dwAttrib = GetFileAttributesW(fname);
      return dwAttrib != INVALID_FILE_ATTRIBUTES;
}
#endif
