#pragma once
#ifndef HGUARD__IPC__BASIC_WRAPPER_HH_
#define HGUARD__IPC__BASIC_WRAPPER_HH_ //NOLINT

#include "Common.hh"
#include "ipc/basic_connection.hh"

inline namespace emlsp {
namespace ipc::io {
/****************************************************************************************/


template <typename Connection, typename Packer, typename Unpacker>
      REQUIRES (IsBasicConnectionVariant<Connection>)
class basic_wrapper
{
      using this_type       = basic_wrapper;
      using connection_type = Connection;
      using packer_type     = Packer;
      using unpacker_type   = Unpacker;
      using value_type      = typename packer_type::value_type;

      friend Packer;

    public:
      explicit basic_wrapper(connection_type *con) : con_(con) {}
      virtual ~basic_wrapper() = default;

      basic_wrapper(basic_wrapper const &)                = delete;
      basic_wrapper(basic_wrapper &&) noexcept            = delete;
      basic_wrapper &operator=(basic_wrapper const &)     = delete;
      basic_wrapper &operator=(basic_wrapper &&) noexcept = delete;

      /*----------------------------------------------------------------------------------*/

      class packer_container
      {
            friend Packer;
            Packer *pack_ = nullptr;

          public:
            packer_container() = default;
            explicit packer_container(packer_type *pack) : pack_(pack) {}

            ~packer_container()
            {
                  pack_->clear();
            }

            auto *operator->()   noexcept { return pack_; }
            auto &operator* () & noexcept { return *pack_; }
            auto &operator()() & noexcept { return pack_->pk; }

            auto const *operator->() const   noexcept { return pack_; }
            auto const &operator* () const & noexcept { return *pack_; }
            auto const &operator()() const & noexcept { return pack_->pk; }

            packer_container(packer_container &&other) noexcept
                : pack_(other.pack_)
            {
                  other.pack_ = nullptr;
            }

            packer_container &operator=(packer_container &&other) noexcept
            {
                  this->pack_ = other.pack_;
                  other.pack_ = nullptr;
                  return *this;
            }

            packer_container(packer_container const &)            = delete;
            packer_container &operator=(packer_container const &) = delete;
      };

    /*----------------------------------------------------------------------------------*/

      ND packer_container get_packer() noexcept 
      {
            for (;;) {
                  for (auto &obj : packs_) {
                        if (auto *ret = obj.get_packer())
                              return packer_container{ret};
                  }
                  auto lock = std::unique_lock{packing_mtx_};
                  packing_cond_.wait_for(lock, 5ms);
            }
      }

      ND connection_type *con() { return con_; }

    /*----------------------------------------------------------------------------------*/

      virtual value_type read_object ()              = 0;
      virtual size_t     write_object(packer_type &) = 0;
      virtual size_t     write_object(value_type &)  = 0;

      virtual size_t write_object(packer_type *packer)
      {
            return write_object(*packer);
      }

      virtual size_t write_object(packer_container &ptr)
      {
            return write_object(*ptr);
      }

    /*----------------------------------------------------------------------------------*/

    protected:
      connection_type *con_;

      unpacker_type           unpacker_     = {};
      std::mutex              io_mtx_       = {};
      std::mutex              write_mtx_    = {};
      std::mutex              read_mtx_     = {};
      std::mutex              packing_mtx_  = {};
      std::condition_variable packing_cond_ = {};

#define PACK packer_type{packing_cond_}
      std::array<packer_type, 64> packs_ = {
          PACK, PACK, PACK, PACK, PACK, PACK, PACK, PACK,
          PACK, PACK, PACK, PACK, PACK, PACK, PACK, PACK,
          PACK, PACK, PACK, PACK, PACK, PACK, PACK, PACK,
          PACK, PACK, PACK, PACK, PACK, PACK, PACK, PACK,
          PACK, PACK, PACK, PACK, PACK, PACK, PACK, PACK,
          PACK, PACK, PACK, PACK, PACK, PACK, PACK, PACK,
          PACK, PACK, PACK, PACK, PACK, PACK, PACK, PACK,
          PACK, PACK, PACK, PACK, PACK, PACK, PACK, PACK,
      };
#undef PACK
};


/****************************************************************************************/

namespace detail {

class msgpack_packer
{
      std::condition_variable &cond_;
      std::mutex               mtx_     = {};
      std::atomic_bool         is_free_ = true;

    public:
      using value_type      = msgpack::object_handle;
      using buffer_type     = msgpack::vrefbuffer;
      using marshaller_type = msgpack::packer<buffer_type>;

      msgpack_packer *get_packer() noexcept 
      {
            mtx_.lock();
            bool expect = true;
            is_free_.compare_exchange_strong(expect, false,
                                             std::memory_order_seq_cst,
                                             std::memory_order_acquire);
            auto *ret = expect ? this : nullptr;
            mtx_.unlock();
            return ret;
      }

      void clear()
      {
            mtx_.lock();
            vbuf.clear();
            is_free_.store(true, std::memory_order_seq_cst);
            cond_.notify_all();
            mtx_.unlock();
      }

      explicit msgpack_packer(std::condition_variable &cond) : cond_(cond) {}
      ~msgpack_packer() = default;

      msgpack_packer(msgpack_packer const &)                = delete;
      msgpack_packer(msgpack_packer &&) noexcept            = delete;
      msgpack_packer &operator=(msgpack_packer const &)     = delete;
      msgpack_packer &operator=(msgpack_packer &&) noexcept = delete;

      buffer_type     vbuf = {};
      marshaller_type pk   = {vbuf};
};

} // namespace detail


template <typename Connection>
class msgpack_wrapper
      : public basic_wrapper<Connection, detail::msgpack_packer, msgpack::unpacker>
{
      static constexpr size_t  read_buffer_size = SIZE_C(8'388'608);

      using this_type = msgpack_wrapper<Connection>;
      using base_type = basic_wrapper<Connection,
                                         detail::msgpack_packer, msgpack::unpacker>;

    public:
      using connection_type = Connection;
      using packer_type     = detail::msgpack_packer;
      using unpacker_type   = msgpack::unpacker;

      using value_type  = msgpack::object_handle;
      using buffer_type = msgpack::vrefbuffer;

    /*----------------------------------------------------------------------------------*/

      explicit msgpack_wrapper(connection_type *con) : base_type(con)
      {
            this->unpacker_.m_user_data = this;
            this->unpacker_.reserve_buffer(read_buffer_size);
      }

      ~msgpack_wrapper() override = default;

      msgpack_wrapper(msgpack_wrapper const &)                = delete;
      msgpack_wrapper(msgpack_wrapper &&) noexcept            = default;
      msgpack_wrapper &operator=(msgpack_wrapper const &)     = delete;
      msgpack_wrapper &operator=(msgpack_wrapper &&) noexcept = default;

    /*----------------------------------------------------------------------------------*/

    private:
      using base_type::read_mtx_;
      using base_type::unpacker_;
      using base_type::con_;

    public:
      using base_type::write_object;

      msgpack::object_handle read_object() override
      {
            //auto    lock = std::lock_guard<std::mutex>{this->read_mtx_};
            std::lock_guard lock{read_mtx_};
            auto    obj  = msgpack::object_handle{};
            ssize_t nread;
            errno = 0;

            while (!unpacker_.next(obj)) {
                  if (unpacker_.buffer_capacity() == 0)
                        unpacker_.reserve_buffer();

                  do {
                        // errno = 0;
                        nread = con_->raw_read(unpacker_.buffer(),
                                               unpacker_.buffer_capacity(), 0);
                        if (nread == (-1) || (errno && errno != EAGAIN)) {
                              perror("uhhhhh");
                              fflush(stdout);
                              fflush(stderr);
                              err_nothrow(1, "read()");
                        }
                  } while (nread <= 0);

                  unpacker_.buffer_consumed(nread);
            }

            if (unpacker_.nonparsed_size() > 0)
                  fmt::print(stderr, FC("Warning: {} bytes still unparsed.\n"),
                             unpacker_.nonparsed_size());

            return obj;
      }

      size_t write_object(msgpack::vrefbuffer const &vec)
      {
            auto lock = std::lock_guard<std::mutex>(this->write_mtx_);
            size_t total = 0;

            for (size_t i = 0; i < vec.vector_size(); ++i) {
                  auto const &iov = vec.vector()[i];
                  total += con_->raw_write(iov.iov_base, iov.iov_len);
            }

            return total;
      }

      size_t write_object(packer_type &pack) override
      {
            return write_object(pack.vbuf);
      }

      size_t write_object(msgpack::object_handle &obj) override
      {
            msgpack::vrefbuffer rbuf;
            msgpack::pack(rbuf, obj.get());
            return write_object(rbuf);
      }
};


/****************************************************************************************/
/****************************************************************************************/


namespace detail {

class ms_jsonrpc2_packer
{
      std::condition_variable &cond_;
      std::mutex               mtx_     = {};
      std::atomic_bool         is_free_ = true;

    public:
      using value_type      = rapidjson::Document;
      using marshaller_type = ipc::json::rapid_doc<>;

      ms_jsonrpc2_packer *get_packer() noexcept 
      {
            mtx_.lock();
            bool expect = true;
            is_free_.compare_exchange_strong(expect, false,
                                             std::memory_order_seq_cst,
                                             std::memory_order_acquire);
            auto *ret = expect ? this : nullptr;
            mtx_.unlock();
            return ret;
      }

      void clear()
      {
            mtx_.lock();
            pk.doc().RemoveAllMembers();
            pk.clear();
            is_free_.store(true, std::memory_order_seq_cst);
            cond_.notify_all();
            mtx_.unlock();
      }

      explicit ms_jsonrpc2_packer(std::condition_variable &cond) : cond_(cond) {}
      ~ms_jsonrpc2_packer() = default;

      ms_jsonrpc2_packer(ms_jsonrpc2_packer &&) noexcept            = delete;
      ms_jsonrpc2_packer(ms_jsonrpc2_packer const &)                = delete;
      ms_jsonrpc2_packer &operator=(ms_jsonrpc2_packer &&) noexcept = delete;
      ms_jsonrpc2_packer &operator=(ms_jsonrpc2_packer const &)     = delete;

      marshaller_type pk = {};
};

} // namespace detail



template <typename Connection>
class ms_jsonrpc2_wrapper
      : public basic_wrapper<Connection, detail::ms_jsonrpc2_packer, rapidjson::Document>
{
      using base_type = basic_wrapper<Connection, detail::ms_jsonrpc2_packer, rapidjson::Document>;
      using this_type = ms_jsonrpc2_wrapper<Connection>;

    public:
      using connection_type = Connection;
      using packer_type     = detail::ms_jsonrpc2_packer;
      using unpacker_type   = rapidjson::Document;

      using value_type      = rapidjson::Document;
      using marshaller_type = ipc::json::rapid_doc<>;

      explicit ms_jsonrpc2_wrapper(connection_type *con) : base_type(con)
      {}

      ~ms_jsonrpc2_wrapper() override = default;

      ms_jsonrpc2_wrapper(ms_jsonrpc2_wrapper const &)                = delete;
      ms_jsonrpc2_wrapper(ms_jsonrpc2_wrapper &&) noexcept            = default;
      ms_jsonrpc2_wrapper &operator=(ms_jsonrpc2_wrapper const &)     = delete;
      ms_jsonrpc2_wrapper &operator=(ms_jsonrpc2_wrapper &&) noexcept = default;

    private:
      using base_type::unpacker_;
      using base_type::con_;

      ND size_t get_content_length()
      {
            /* Exactly 64 bytes assuming an 8 byte pointer size. Three cheers for
             * pointless "optimizations". */
            char    buf[47];
            char   *ptr = buf;
            size_t  len;
            uint8_t ch;

            (void)con_->raw_read(buf, 16, MSG_WAITALL); // Discard
            (void)con_->raw_read(&ch, 1, MSG_WAITALL);
            while (::isdigit(ch)) {
                  *ptr++ = static_cast<char>(ch);
                  (void)con_->raw_read(&ch, 1, MSG_WAITALL);
            }

            *ptr = '\0';
            len  = ::strtoull(buf, &ptr, 10);

            /* Some "clever" servers send some extra bullshit after the message length.
             * Just search for the mandatory '\r\n\r\n' sequence to ignore it. */
            for (;;) {
                  while (ch != '\r') [[unlikely]] {
                  fail:
                        (void)con_->raw_read(&ch, 1, MSG_WAITALL);
                  }
                  (void)con_->raw_read(&ch, 1, MSG_WAITALL);
                  if (ch != '\n') [[unlikely]]
                        goto fail;  //NOLINT
                  (void)con_->raw_read(buf, 2, MSG_WAITALL);
                  if (::strncmp(buf, "\r\n", 2) == 0) [[likely]]
                        break;
            }

            if (ptr == buf || len == 0) [[unlikely]] {
                  throw std::runtime_error(
                      fmt::format(FC("Invalid JSON-RPC: no non-zero number found in "
                                     "header at \"{}\"."),
                                  buf));
            }
            return len;
      }

      //std::string write_jsonrpc_header(size_t const len)
      size_t write_jsonrpc_header(size_t const len)
      {
            /* It's safe to go with sprintf because we know for certain that a single
             * 64 bit integer will fit in this buffer along with the format. */
            char buf[60];
            int  buflen = ::sprintf(buf, "Content-Length: %zu\r\n\r\n", len);
            //return {buf, static_cast<size_t>(buflen)};
            return con_->raw_write(buf, static_cast<size_t>(buflen));
      }

    public:
      using base_type::write_object;

      size_t write_object(rapidjson::Document &doc) override
      {
            auto lock = std::lock_guard<std::mutex>(this->write_mtx_);

            rapidjson::MemoryBuffer sbuf;
            rapidjson::Writer       wr{sbuf, &doc.GetAllocator()};
            doc.Accept(wr);

            size_t ret = write_jsonrpc_header(sbuf.GetSize());
            ret       += con_->raw_write(sbuf.GetBuffer(), sbuf.GetSize());
            return ret;
      }

      size_t write_object(packer_type &pack) override
      {
            return write_object(pack.pk.doc());
      }

      size_t write_string(std::string const &str)      { return write_string(str.data(), str.size()); }
      size_t write_string(std::string_view const &str) { return write_string(str.data(), str.size()); }

      size_t write_string(void const *buf, size_t const size)
      {
            auto   lock = std::lock_guard<std::mutex>(this->write_mtx_);
            size_t ret  = write_jsonrpc_header(size);
            ret        += con_->raw_write(buf, size);
            return ret;
      }

      rapidjson::Document read_object() override
      {
            auto lock = std::lock_guard<std::mutex>(this->read_mtx_);
            auto len = get_content_length();
            auto msg = std::make_unique<char[]>(len + 1);

            UNUSED auto const nread = con_->raw_read(msg.get(), len, MSG_WAITALL);
            assert(static_cast<size_t>(nread) == len);

            msg[len] = '\0';
            rapidjson::Document doc;
            doc.Parse(msg.get(), len);
            return doc;
      }
};


/****************************************************************************************/


template <typename T>
concept IsWrapperVariant = std::derived_from<T,
                                             basic_wrapper<typename T::connection_type,
                                                           typename T::packer_type,
                                                           typename T::unpacker_type>>;


/****************************************************************************************/
} // namespace ipc::io
} // namespace emlsp
#endif
