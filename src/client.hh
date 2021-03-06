// ReSharper disable CppInconsistentNaming
#pragma once
#ifndef HGUARD_d_CLIENT_HH_
#define HGUARD_d_CLIENT_HH_
/****************************************************************************************/

#include "Common.hh"
#include "util/exceptions.hh"

namespace emlsp
{

/**
 * TODO: Documentation of some kind...
 */
class base_client
{
    public:
      /* Only gcc insists that this have an initializer. Neither clang nor MSVC care. */
      static constexpr struct info_s {
            static constexpr uint8_t major    = MAIN_PROJECT_VERSION_MAJOR;
            static constexpr uint8_t minor    = MAIN_PROJECT_VERSION_MINOR;
            static constexpr uint8_t patch    = MAIN_PROJECT_VERSION_PATCH;
            static constexpr char    string[] = MAIN_PROJECT_VERSION_STRING;
            static constexpr char    name[]   = MAIN_PROJECT_NAME;
      } info = {};

      enum class client_type {
            NONE, LSP,
      };

    private:
      client_type type_;

    public:
      explicit base_client(client_type const ty) : type_(ty) {}
      virtual ~base_client() = default;

      ND client_type get_client_type() const { return type_; }

      virtual int foo() = 0;

      DEFAULT_COPY_CTORS(base_client);
      DEFAULT_MOVE_CTORS(base_client);
};

namespace rpc {

/****************************************************************************************/

namespace detail {

#ifdef _WIN32
using procinfo_t = PROCESS_INFORMATION;
#else
using procinfo_t = pid_t;
#endif

class base_connection_interface
{
    protected:
      enum class sink_type {
            DEFAULT,
            DEVNULL,
            FD,
            FILENAME
      } err_sink_type_ = sink_type::DEFAULT;

      std::string fname_ = {};
      int         fd     = (-1);

#ifdef _WIN32
      ND HANDLE get_err_handle() const;
#endif

    public:
      base_connection_interface()  = default;
      virtual ~base_connection_interface()
      {
            switch (err_sink_type_)
            {
                  case sink_type::FILENAME:
                  case sink_type::FD:
                  case sink_type::DEVNULL:
                  case sink_type::DEFAULT:
                        break;
            }
      }

      virtual procinfo_t do_spawn_connection(size_t argc, char **argv) = 0;
      virtual ssize_t read(void *buf, size_t nbytes)                   = 0;
      virtual ssize_t read(void *buf, size_t nbytes, int flags)        = 0;
      virtual ssize_t write(void const *buf, size_t nbytes)            = 0;
      virtual ssize_t write(void const *buf, size_t nbytes, int flags) = 0;

      void set_stderr_default() { err_sink_type_ = sink_type::DEFAULT; }
      void set_stderr_devnull() { err_sink_type_ = sink_type::DEVNULL; }
      void set_stderr_filename(std::string const &fname)
      {
            fname_         = fname;
            err_sink_type_ = sink_type::FILENAME;
      }

      DEFAULT_MOVE_CTORS(base_connection_interface);
      DEFAULT_COPY_CTORS(base_connection_interface);
};

/*--------------------------------------------------------------------------------------*/

template <typename AddrType>
class socket_connection_base_impl : public base_connection_interface
{
#ifdef _WIN32
      static int close_socket(socket_t const sock) { return closesocket(sock); }
#else
      static int close_socket(socket_t const sock) { return close(sock); }
#endif

    protected:
      socket_t root_      = static_cast<socket_t>(-1);
      socket_t accepted_  = static_cast<socket_t>(-1);
      socket_t connected_ = static_cast<socket_t>(-1);
      AddrType addr_      = {};

      virtual socket_t open_new_socket()   = 0;
      virtual socket_t connect_to_socket() = 0;

    public:
      socket_connection_base_impl() = default;
      ~socket_connection_base_impl() override
      {
            if (connected_ != (-1))
                  close_socket(connected_);
            if (accepted_ != (-1))
                  close_socket(accepted_);
            if (root_ != (-1))
                  close_socket(root_);
      }

      ssize_t read (void       *buf, size_t const nbytes) final { return read(buf, nbytes, 0); }
      ssize_t write(void const *buf, size_t const nbytes) final { return write(buf, nbytes, 0); }

      ssize_t read(void *buf, size_t const nbytes, int const flags) final
      {
            size_t total = 0, n;
            do {
                  n = ::recv(accepted_, static_cast<char *>(buf) + total, nbytes - total, flags);
            } while ((n != (-1) /* || WSAGetLastError() == 10035L */) && (total += n) < static_cast<ssize_t>(nbytes));

            if (n == (-1) /* && WSAGetLastError() != 10035L*/)
#if defined _WIN32
                  util::win32::error_exit(L"recv()");
#else
                  err(1, "send()");
#endif

            return total;
      }

      ssize_t write(void const *buf, size_t const nbytes, int const flags) final
      {
            ssize_t total = 0, n;
            do {
                  n = ::send(accepted_, static_cast<char const *>(buf) + total, nbytes - total, flags);
            } while (n != (-1) && (total += n) < static_cast<ssize_t>(nbytes));

            if (n == (-1))
#if defined _WIN32
                  util::win32::error_exit(L"send()");
#else
                  err(1, "send()");
#endif

            return total;
      }

      ND socket_t const &operator()() const { return accepted_; }
      ND socket_t const &base()       const { return root_; }
      ND socket_t const &accepted()   const { return accepted_; }
      ND AddrType const &addr()       const { return addr_; }

      DELETE_COPY_CTORS(socket_connection_base_impl);
      DEFAULT_MOVE_CTORS(socket_connection_base_impl);
};

class unix_socket_connection_impl final : public socket_connection_base_impl<sockaddr_un>
{
      std::string path_;

    protected:
      socket_t open_new_socket()   final;
      socket_t connect_to_socket() final;

    public:
      unix_socket_connection_impl()        = default;
      ~unix_socket_connection_impl() final = default;

      procinfo_t do_spawn_connection(size_t argc, char **argv) final;

      DELETE_COPY_CTORS(unix_socket_connection_impl);
      DEFAULT_MOVE_CTORS(unix_socket_connection_impl);
};

/*--------------------------------------------------------------------------------------*/

class pipe_connection_impl final : public base_connection_interface
{
    private:
      int read_  = (-1);
      int write_ = (-1);

    public:
      pipe_connection_impl() = default;
      ~pipe_connection_impl() final
      {
            if (read_ != (-1))
                  close(read_);
            if (write_ != (-1))
                  close(write_);
      }

      procinfo_t do_spawn_connection(size_t argc, char **argv) final;

      ssize_t read (void       *buf, size_t const nbytes) final { return read(buf, nbytes, 0); }
      ssize_t write(void const *buf, size_t const nbytes) final { return write(buf, nbytes, 0); }

      ssize_t read(void *buf, size_t const nbytes, UNUSED int flags) final
      {
            size_t total = 0, n;
            do {
                  n = ::read(read_, static_cast<char *>(buf) + total, nbytes - total);
            } while (n != SIZE_C(-1) && (total += n) < nbytes);

            return static_cast<ssize_t>(total);
      }

      ssize_t write(void const *buf, size_t const nbytes, UNUSED int flags) final
      {
            size_t total = 0, n;
            do {
                  n = ::write(write_, static_cast<char const *>(buf) + total, nbytes - total);
            } while (n != SIZE_C(-1) && (total += n) < nbytes);

            return static_cast<ssize_t>(total);
      }

      DELETE_COPY_CTORS(pipe_connection_impl);
      DEFAULT_MOVE_CTORS(pipe_connection_impl);
};

/*--------------------------------------------------------------------------------------*/

#ifdef _WIN32

class win32_named_pipe_impl final : public base_connection_interface
{
    private:
      HANDLE      pipe_ = nullptr;
      std::string fname_;

    public:
      //win32_named_pipe_impl() = default;
      win32_named_pipe_impl()
      {
            throw emlsp::except::not_implemented("FIXME: Not functional.");
      }
      ~win32_named_pipe_impl() final
      {
            if (pipe_)
                  CloseHandle(pipe_);
      }

      procinfo_t do_spawn_connection(size_t argc, char **argv) final;

      ssize_t read(void        *buf, size_t const nbytes) final { return read(buf, nbytes, 0); }
      ssize_t write(void const *buf, size_t const nbytes) final { return write(buf, nbytes, 0); }

      ssize_t read(void *buf, size_t const nbytes, UNUSED int const flags) final
      {
            size_t total = 0;
            DWORD  n     = 0;
            int    ret;
            do {
                  ret = ReadFile(pipe_, static_cast<char *>(buf) + total, nbytes - total, &n, nullptr);;
            } while (n != UINT32_C(-1) && (total += n) < nbytes);

            auto const e = GetLastError();
            if (!ret && e != ERROR_MORE_DATA)
                  util::win32::error_exit(L"ReadFile");

            return static_cast<ssize_t>(total);
      }

      ssize_t write(void const *buf, size_t const nbytes, UNUSED int const flags) final
      {
            size_t total = 0;
            DWORD  n     = 0;
            int    ret;
            do {
                  ret = WriteFile(pipe_, static_cast<char const *>(buf) + total, nbytes - total, &n, nullptr);;
            } while (n != UINT32_C(-1) && (total += n) < nbytes);

            if (!ret)
                  util::win32::error_exit(L"WriteFile");

            return static_cast<ssize_t>(total);
      }

      DELETE_COPY_CTORS(win32_named_pipe_impl);
      DEFAULT_MOVE_CTORS(win32_named_pipe_impl);
};

#endif

} // namespace detail


/****************************************************************************************/


template <typename T>
concept ConnectionImplVariant =    std::same_as<T, detail::unix_socket_connection_impl>
                                || std::same_as<T, detail::pipe_connection_impl>
#ifdef _WIN32
                                || std::same_as<T, detail::win32_named_pipe_impl>
#endif
;

using util::Stringable;

/**
 * TODO: Documentation of some kind...
 */
template <ConnectionImplVariant ConnectionImpl>
class base_connection
{
    public:
      using procinfo_t  = detail::procinfo_t;
      using cstring_ptr = std::unique_ptr<char[]>;

    private:
      procinfo_t     pid_        = {};
      ConnectionImpl connection_ = {};

    public:
      base_connection()  = default;

      virtual ~base_connection()
      {
#ifdef _WIN32
            TerminateProcess(pid_.hProcess, 0);
            CloseHandle(pid_.hThread);
            CloseHandle(pid_.hProcess);
#else
            if (pid_) {
                  kill(pid_, SIGTERM);
                  waitpid(pid_, nullptr, 0);
            }
#endif
      }

      ND ConnectionImpl &connection() { return connection_; }
      ND ConnectionImpl &operator()() { return connection_; }
      ND procinfo_t const &pid() const { return pid_; }

      procinfo_t spawn_connection(size_t argc, char **argv)
      {
            pid_ = connection_.do_spawn_connection(argc, argv);
            return pid_;
      }

      procinfo_t spawn_connection(size_t const argc, char const **argv)
      {
            return spawn_connection(argc, const_cast<char **>(argv));
      }

      procinfo_t spawn_connection(char **const argv)
      {
            char **p = argv;
            while (*p++)
                  ;
            return spawn_connection(p - argv, argv);
      }

      procinfo_t spawn_connection(char const **const argv)
      {
            return spawn_connection(const_cast<char **>(argv));
      }

      procinfo_t spawn_connection(char const *const *const argv)
      {
            return spawn_connection(const_cast<char **>(argv));
      }

      procinfo_t spawn_connection(std::vector<char *> &vec)
      {
            if (vec[vec.size() - 1] != nullptr)
                  vec.emplace_back(nullptr);
            return spawn_connection(vec.size(), vec.data());
      }

      procinfo_t spawn_connection(std::vector<char const *> &vec)
      {
            if (vec[vec.size() - 1] != nullptr)
                  vec.emplace_back(nullptr);
            return spawn_connection(vec.size(), const_cast<char **>(vec.data()));
      }

      /**
       * To be used much like execl. All arguments must be `char const *`. Unlike execl,
       * no null pointer is required as a sentinel.
       */
      template <Stringable... Types>
      procinfo_t spawn_connection_l(Types &&...args)
      {
            const char *const pack[] = {args..., nullptr};
            constexpr size_t  argc   = (sizeof(pack) / sizeof(pack[0])) - SIZE_C(1);
            return spawn_connection(argc, const_cast<char **>(pack));
      }

      /**
       * Directly call the connection_'s `read()` method.
       * Params: void *, size_t, int
       */
      template <typename ...Args>
      ssize_t raw_read(Args &&...args)
      {
            return connection_.read(args...);
      }

      /**
       * Directly call the connection_'s `write()` method.
       * Params: void *, size_t, int
       */
      template <typename ...Args>
      ssize_t raw_write(Args &&...args)
      {
            return connection_.write(args...);
      }

      template <size_t N>
      void write_message_l(char const (&msg)[N])
      {
            write_message(msg, sizeof(msg) - SIZE_C(1));
      }

      virtual void write_message(char const *)              = 0;
      virtual void write_message(void const *, size_t)      = 0;
      virtual void write_message(std::string const &)       = 0;
      virtual void write_message(std::vector<char> const &) = 0;

      virtual size_t            read_message(void **)       = 0;
      virtual size_t            read_message(char **)       = 0;
      virtual size_t            read_message(char *&)       = 0;
      virtual size_t            read_message(cstring_ptr &) = 0;
      virtual std::vector<char> read_message()              = 0;
      virtual std::string       read_message_string()       = 0;

      virtual size_t discard_message() = 0;

      DELETE_COPY_CTORS(base_connection);
      DEFAULT_MOVE_CTORS(base_connection);
};

} // namespace rpc
} // namespace emlsp

/****************************************************************************************/
#endif // client.hh
