#pragma once
#ifndef HGUARD__RAPID_H_
#define HGUARD__RAPID_H_

#ifdef GetObject
# undef GetObject
#endif

#define P00_PASTE_2_HELPER_2(a, b) a ## b
#define P00_PASTE_2_HELPER_1(a, b) P00_PASTE_2_HELPER_2(a, b)
#define PP_PASTE_2(a, b) P00_PASTE_2_HELPER_1(a, b)

#define P00_STRINGIFY_HELPER(x) #x
#define PP_STRINGIFY(x) P00_STRINGIFY_HELPER(x)

#define RAPIDJSON_ASSERT(x)
#define RAPIDJSON_NOEXCEPT_ASSERT(x)
#if 0
#define RAPIDJSON_ASSERT_THROWS
#define RAPIDJSON_NOEXCEPT_ASSERT(x) assert(x)
#define RAPIDJSON_ASSERT(x)                     \
      do {                                      \
            if (!(x)) [[unlikely]] {            \
                  throw std::runtime_error(#x); \
            }                                   \
      } while (0)
#endif
#define RAPIDJSON_SSE2 1
#define RAPIDJSON_SSE42 1
#define RAPIDJSON_USE_MEMBERSMAP 1
#define RAPIDJSON_HAS_CXX11_TYPETRAITS 1
#define RAPIDJSON_HAS_CXX11_RANGE_FOR  1
#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/rapidjson.h>

#include <rapidjson/document.h>
#include <rapidjson/memorybuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <rapidjson/allocators.h>
#include <rapidjson/cursorstreamwrapper.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/encodings.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/fwd.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/pointer.h>
#include <rapidjson/reader.h>
#include <rapidjson/schema.h>
#include <rapidjson/writer.h>

#endif
