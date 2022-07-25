﻿#pragma once
#ifndef HGUARD__UTIL__UTIL_HH_
#define HGUARD__UTIL__UTIL_HH_  //NOLINT
/*--------------------------------------------------------------------------------------*/

#include "Common.hh"
#include "util/c_util.h"
#include "util/concepts.hh"
#include "util/deleters.hh"
#include "util/exceptions.hh"
#include "util/hackish_templates.hh"
#include "util/strings.hh"

#include "util/recode/recode.hh"

#define BARE_DECLTYPE(obj) std::remove_cvref_t<decltype(obj)>

inline namespace emlsp {
namespace util {
/****************************************************************************************/


namespace hacks {
typedef std::filesystem::path path;  //NOLINT
typedef std::string_view      sview; //NOLINT
} // namespace hacks


namespace constants {
extern bool const coloured_stderr;
} // namespace constants


extern hacks::path get_temporary_directory(char const *prefix = nullptr);
extern hacks::path get_temporary_filename(char const *__restrict prefix = nullptr,
                                          char const *__restrict suffix = nullptr);


extern std::string slurp_file(char const         *fname, bool binary = false);
extern std::string slurp_file(std::string const  &fname, bool binary = false);
extern std::string slurp_file(hacks::path const  &fname, bool binary = false);
extern std::string slurp_file(hacks::sview const &fname, bool binary = false);


ND extern uintmax_t   xatoi(char const *number, bool strict = true);
ND extern std::string char_repr(char ch);
ND extern std::string my_strerror(errno_t errval);
using ::my_strerror;


ND extern std::string demangle(_In_z_ char const *raw_name) __attribute__((__nonnull__));
ND extern std::string demangle(std::type_info const &id);


/* I wouldn't normally check the return of malloc & friends, but MSVC loudly complains
 * about possible null pointers. So I'll make an exception. Damn it Microsoft. */
template <typename T>
ND __forceinline T *
xmalloc(size_t const size = sizeof(T))
{
      //NOLINTNEXTLINE(hicpp-no-malloc, cppcoreguidelines-no-malloc)
      void *ret = malloc(size);
      if (ret == nullptr) [[unlikely]]
            throw std::system_error(std::make_error_code(std::errc::not_enough_memory));
      return static_cast<T *>(ret);
}

template <typename T>
ND __forceinline T *
xcalloc(size_t const nmemb = SIZE_C(1),
        size_t const size  = sizeof(T))
{
      //NOLINTNEXTLINE(hicpp-no-malloc, cppcoreguidelines-no-malloc)
      void *ret = calloc(nmemb, size);
      if (ret == nullptr) [[unlikely]]
            throw std::system_error(std::make_error_code(std::errc::not_enough_memory));
      return static_cast<T *>(ret);
}


constexpr void free_all() {}

/* I can't lie; the primary reason this exists is to silence clang's whining about
 * calling free. */
template <typename T, typename ...Pack>
      REQUIRES(util::concepts::GenericPointer<T>)
constexpr void free_all(T arg, Pack ...pack)
{
      // NOLINTNEXTLINE(hicpp-no-malloc,cppcoreguidelines-no-malloc)
      ::free(arg);
      free_all(pack...);
}

/*--------------------------------------------------------------------------------------*/

namespace detail {
#if defined FIONREAD
constexpr uint64_t ioctl_size_available = FIONREAD;
#elif defined TIOCINQ
constexpr uint64_t ioctl_size_available = TIOCINQ;
#else
# error "Neither TIOCINQ nor FIONREAD exist as ioctl parameters on this system."
#endif
#ifdef _WIN32
using procinfo_t = PROCESS_INFORMATION;
#else
using procinfo_t = pid_t;
#endif
} // namespace detail


extern int  kill_process(detail::procinfo_t &pid) noexcept;
extern void close_descriptor(int &fd) noexcept;
extern void close_descriptor(intptr_t &fd) noexcept;

extern int waitpid(detail::procinfo_t const &pid);

#ifdef _WIN32

extern    void   close_descriptor(HANDLE &fd) noexcept;
extern    void   close_descriptor(SOCKET &fd) noexcept;
ND extern size_t available_in_fd (SOCKET s);
ND extern size_t available_in_fd (HANDLE s);
extern    int    waitpid(HANDLE pid);

#endif

ND extern size_t available_in_fd(int s);

/*--------------------------------------------------------------------------------------*/

#ifndef _WIN32
inline int putenv(char const *str) { return ::putenv(const_cast<char *>(str)); }
#endif

/*--------------------------------------------------------------------------------------*/


ND __attr_access((__write_only__, 1)) __attribute__((__artificial__, __nonnull__))
__forceinline size_t
strlcpy(_Out_z_cap_(size) char       *__restrict dst,
        _In_z_            char const *__restrict src,
        _In_              size_t const           size) noexcept
{
#ifdef HAVE_STRLCPY
      return ::strlcpy(dst, src, size);
#else
      return ::emlsp_strlcpy(dst, src, size);
#endif
}


template <_In_ size_t N>
ND __attr_access((__write_only__, 1)) __attribute__((__artificial__, __nonnull__))
__forceinline size_t
strlcpy(_Out_z_cap_(N) char (&dst)[N],
        _In_z_         char const *__restrict src) noexcept
{
#if defined HAVE_STRLCPY
      return ::strlcpy(dst, src, N);
#elif defined HAVE_STRCPY_S && false  // Apparently strcpy_s doesn't work like strlcpy
      return strcpy_s(dst, src);
#else
      return ::emlsp_strlcpy(dst, src, N);
#endif
}


template <typename T>
ND __attribute__((__const__, __artificial__, __nonnull__))
__forceinline constexpr uintptr_t
ptr_diff(_Notnull_ T const *ptr1, _Notnull_ T const *ptr2) noexcept
{
      return static_cast<uintptr_t>(reinterpret_cast<ptrdiff_t>(ptr1) -
                                    reinterpret_cast<ptrdiff_t>(ptr2));
}


template <typename Elem, size_t N>
__forceinline size_t
fwritel(Elem const (&buffer)[N], FILE *dst)
{
      return ::fwrite(buffer, sizeof(Elem), N - SIZE_C(1), dst);
}


template <typename S, typename... Args>
    REQUIRES (concepts::is_compiled_string_c<S>)
void
eprint(S const &format_str, Args const &...args)
{
#ifdef _WIN32
      //_lock_file(stderr);
#else
      flockfile(stderr);
#endif
      if (constants::coloured_stderr)
            //fwritel("\033[38;2;255;255;0m" MAIN_PROJECT_NAME " (warning):  \033[0m"
            //        "\033[38;2;255;182;193m", stderr);
            fmt::print(stderr, FC("{}\033[38;2;255;182;193m"),
                       styled(MAIN_PROJECT_NAME " (warning):  ",
                              fg(fmt::color::yellow) | fmt::emphasis::bold));
      else
            fwritel(MAIN_PROJECT_NAME " (warning):  ", stderr);
      fmt::print(stderr, format_str, std::forward<Args const &&>(args)...);
      if (constants::coloured_stderr)
            fwritel("\033[0m", stderr);
      ::fflush(stderr);
#ifdef _WIN32
      //_unlock_file(stderr);
#else
      funlockfile(stderr);
#endif
}


/*======================================================================================*/


__forceinline constexpr void
timespec_add(timespec       *const __restrict lval,
             timespec const *const __restrict rval)
{
      lval->tv_nsec += rval->tv_nsec;
      lval->tv_sec  += rval->tv_sec;

      if (lval->tv_nsec >= INT64_C(1000000000)) {
            ++lval->tv_sec;
            lval->tv_nsec -= INT64_C(1000000000);
      }
}


template <typename Rep, typename Period>
__forceinline constexpr timespec
duration_to_timespec(std::chrono::duration<Rep, Period> const &dur)
{
      timespec dur_ts;  //NOLINT(cppcoreguidelines-pro-type-member-init)
      auto const secs  = std::chrono::duration_cast<std::chrono::seconds>(dur);
      auto const nsecs = std::chrono::duration_cast<std::chrono::nanoseconds>(dur) -
                         std::chrono::duration_cast<std::chrono::nanoseconds>(secs);
      dur_ts.tv_sec  = secs.count();
      dur_ts.tv_nsec = nsecs.count();
      return dur_ts;
}


__forceinline constexpr std::chrono::nanoseconds
timespec_to_duration(timespec const *dur_ts)
{
      return std::chrono::seconds(dur_ts->tv_sec) +
             std::chrono::nanoseconds(dur_ts->tv_nsec);
}


constexpr auto &operator+=(timespec &lval, timespec const &rval)
{
      timespec_add(&lval, &rval);
      return lval;
}

constexpr auto &operator+=(timespec &lval, timespec const *const rval)
{
      timespec_add(&lval, rval);
      return lval;
}

template <typename Rep, typename Period>
constexpr auto &operator+=(timespec &lval, std::chrono::duration<Rep, Period> const &rval)
{
      auto const ts = duration_to_timespec(rval);
      timespec_add(&lval, &ts);
      return lval;
}


/*======================================================================================*/


namespace ipc {

#define SOCK_ARGS \
      int dom = AF_UNIX, int type = SOCK_STREAM | SOCK_CLOEXEC, int proto = 0

extern ::socket_t open_new_unix_socket  (char const *path,          SOCK_ARGS);
extern ::socket_t connect_to_unix_socket(char const *path,          SOCK_ARGS);
extern ::socket_t connect_to_unix_socket(::sockaddr_un const *addr, SOCK_ARGS);

#undef SOCK_ARGS

extern ::addrinfo *resolve_addr(char const *server, char const *port, int type = SOCK_STREAM);

} // namespace ipc

#ifdef _WIN32
namespace win32 {

void warning_box_explicit(_In_z_ wchar_t const *msg, _In_ DWORD errval);
void warning_box_message (_In_z_ wchar_t const *msg);
void warning_box_explicit(_In_z_ char const *msg, _In_ DWORD errval);
void warning_box_message (_In_z_ char const *msg);

NORETURN void error_exit_explicit(_In_z_ wchar_t const *msg, _In_ DWORD errval);
NORETURN void error_exit_message (_In_z_ wchar_t const *msg);
NORETURN void error_exit_explicit(_In_z_ char const *msg, _In_ DWORD errval);
NORETURN void error_exit_message (_In_z_ char const *msg);

NORETURN __forceinline void error_exit        (wchar_t const *msg)      { error_exit_explicit(msg,         GetLastError());    }
NORETURN __forceinline void error_exit_wsa    (wchar_t const *msg)      { error_exit_explicit(msg,         WSAGetLastError()); }
NORETURN __forceinline void error_exit_errno  (wchar_t const *msg)      { error_exit_explicit(msg,         errno);             }
NORETURN __forceinline void error_exit        (std::wstring const &msg) { error_exit_explicit(msg.c_str(), GetLastError());    }
NORETURN __forceinline void error_exit_wsa    (std::wstring const &msg) { error_exit_explicit(msg.c_str(), WSAGetLastError()); }
NORETURN __forceinline void error_exit_errno  (std::wstring const &msg) { error_exit_explicit(msg.c_str(), errno);             }
NORETURN __forceinline void error_exit_message(std::wstring const &msg) { error_exit_message (msg.c_str());                    }

NORETURN __forceinline void error_exit        (char const *msg)        { error_exit_explicit(msg,         GetLastError());    }
NORETURN __forceinline void error_exit_wsa    (char const *msg)        { error_exit_explicit(msg,         WSAGetLastError()); }
NORETURN __forceinline void error_exit_errno  (char const *msg)        { error_exit_explicit(msg,         errno);             }
NORETURN __forceinline void error_exit        (std::string const &msg) { error_exit_explicit(msg.c_str(), GetLastError());    }
NORETURN __forceinline void error_exit_wsa    (std::string const &msg) { error_exit_explicit(msg.c_str(), WSAGetLastError()); }
NORETURN __forceinline void error_exit_errno  (std::string const &msg) { error_exit_explicit(msg.c_str(), errno);             }
NORETURN __forceinline void error_exit_message(std::string const &msg) { error_exit_message (msg.c_str());                    }

extern BOOL WINAPI myCtrlHandler(DWORD type);

template <typename Proc = ::FARPROC>
Proc get_proc_address_module(LPCWSTR const module_name, LPCSTR const proc_name)
{
      auto *mod_hand = ::GetModuleHandleW(module_name);
      if (!mod_hand || mod_hand == INVALID_HANDLE_VALUE)
            error_exit(L"GetModuleHandleW()");
      // NOLINTNEXTLINE(clang-diagnostic-cast-function-type)
      auto *routine = reinterpret_cast<Proc>(::GetProcAddress(mod_hand, proc_name));
      if (!routine)
            error_exit(L"GetProcAddress()");
      return routine;
}

} // namespace win32
#endif

/****************************************************************************************/
} // namespace util

using util::operator+=;

} // namespace emlsp


#ifndef _WIN32
using emlsp::util::putenv;  //NOLINT(google-global-names-in-headers)
#endif

#include "util/formatters.hh"


/****************************************************************************************/
#endif
// vim: ft=cpp
