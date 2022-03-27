#pragma once

#include "Common.hh"
#include "util/exceptions.hh"

#include <msgpack.hpp>

inline namespace emlsp {
namespace util::mpack {

class dumper
{
      static constexpr unsigned width = 4;

      std::ostream &strm_;
      unsigned      depth_      = 0;
      bool          put_nl_     = true;
      bool          put_indent_ = true;

      void put_nl();
      void put_indent();

    public:
      explicit dumper(msgpack::object const *val, std::ostream &out = std::cout)
          : strm_(out)
      {
            dump_object(val);
      }

      explicit dumper(msgpack::object const &val, std::ostream &out = std::cout)
          : dumper(&val, out)
      {}

      void dump_object(msgpack::object const *val);
      void dump_array(msgpack::object_array const *arr);
      void dump_dict(msgpack::object_map const *dict);
};

} // namespace util::mpack
} // namespace emlsp
