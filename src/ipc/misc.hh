#pragma once
#ifndef HGUARD__IPC__MISC_HH_
#define HGUARD__IPC__MISC_HH_ //NOLINT

#include "Common.hh"
#ifndef _WIN32
#  include <netdb.h>
#  include <netinet/in.h>
#endif

inline namespace emlsp {
namespace ipc { //NOLINT(modernize-concat-nested-namespaces)
/****************************************************************************************/


namespace except {

class connection_closed final : public std::runtime_error
{
      std::string text_;
      connection_closed(std::string const &message, char const *function)
          : std::runtime_error("Connection closed")
      {
            text_ = message + " : "s + function;
      }
      connection_closed(char const *message, char const *function)
          : connection_closed(std::string(message), function)
      {}
    public:
      connection_closed() : connection_closed("Connection closed"s, FUNCTION_NAME)
      {}
      explicit connection_closed(char const *message)
          : connection_closed(message, FUNCTION_NAME)
      {}
      explicit connection_closed(std::string const &message)
          : connection_closed(message, FUNCTION_NAME)
      {}
      ND char const *what() const noexcept override { return text_.c_str(); }
};

} // namespace except


namespace detail {

template<typename T>
concept IsInetSockaddr = std::same_as<T, sockaddr_in> ||
                         std::same_as<T, sockaddr_in6>;

template <typename T>
concept StdMutexVariant = std::derived_from<T, std::mutex> ||
                          std::derived_from<T, std::recursive_mutex> ||
                          std::derived_from<T, std::timed_mutex> ||
                          std::derived_from<T, std::recursive_timed_mutex>;

} // namespace detail


/****************************************************************************************/
} // namespace ipc
} // namespace emlsp
#endif // misc.hh
