// ReSharper disable CppInconsistentNaming
#include "myerr.h"

#include "c_util.h"

#define SHUTUPGCC __attribute__((__unused__)) \
      ssize_t const P99_PASTE(show_stacktrace_macro_variable_, __COUNTER__, _, __LINE__) =

#ifdef HAVE_EXECINFO_H
#  include <execinfo.h>
#define SHOW_STACKTRACE()                                  \
      __extension__({                                      \
            void     *arr[128];                            \
            int const num = backtrace(arr, 128);           \
            SHUTUPGCC write(2, SLS("<<< FATAL ERROR >>>\n" \
                                   "    STACKTRACE:\n"));  \
            backtrace_symbols_fd(arr, num, 2);             \
            SHUTUPGCC write(2, "\n", 1);                   \
            fsync(2);                                      \
      })
#else
#  define SHOW_STACKTRACE(...)
#endif
#ifndef _Printf_format_string_
#  define _Printf_format_string_
#endif

extern mtx_t util_c_error_write_mutex;
mtx_t util_c_error_write_mutex;

INITIALIZER_HACK(mutex_init)
{
      mtx_init(&util_c_error_write_mutex, mtx_plain);
}

static __inline void dump_error(int const errval)
{
      char        buf[128];
      char const *estr = my_strerror(errval, buf, 128);

      fprintf(stderr, ": %s\n", estr);

}

void
my_err_(int  const           status,
        bool const           print_err,
        char const *restrict file,
        int  const           line,
        char const *restrict func,
        _Printf_format_string_ _Notnull_
        char const *restrict format,
        ...)
{
      va_list   ap;
      int const e = errno;
      mtx_lock(&util_c_error_write_mutex);

      fprintf(stderr, "%s: (%s %d - %s): ", MAIN_PROJECT_NAME, file, line, func);
      va_start(ap, format);
      vfprintf(stderr, format, ap);
      va_end(ap);

      if (print_err)
            dump_error(e);
      else
            fputc('\n', stderr);

      fflush(stderr);
      SHOW_STACKTRACE();

      mtx_unlock(&util_c_error_write_mutex);
      exit(status);
}

void
my_warn_(       bool const           print_err,
         UNUSED bool const           force,
                char const *restrict file,
                int  const           line,
                char const *restrict func,
                _Printf_format_string_ _Notnull_
                char const *restrict format,
                ...)
{
#ifdef NDEBUG
      if (!force)
            return;
#endif

      va_list   ap;
      int const e = errno;
      mtx_lock(&util_c_error_write_mutex);

      fprintf(stderr, "%s: (%s %d - %s): ", MAIN_PROJECT_NAME, file, line, func);
      va_start(ap, format);
      vfprintf(stderr, format, ap);
      va_end(ap);

      if (print_err)
            dump_error(e);
      else
            fputc('\n', stderr);

      fflush(stderr);
      mtx_unlock(&util_c_error_write_mutex);
}
