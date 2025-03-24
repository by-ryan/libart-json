#ifndef art__json__serialize_hxx_
#define art__json__serialize_hxx_

#include <art/json/marshaling.hxx>

#include <art/json/read.hxx>
#include <art/json/write.hxx>

#include <istream>

namespace art::json
{

  template<typename T>
  void
  serialize(std::ostream& o, T const& model)
  {
    write(o, marshal(model));
  }

  template<typename T>
  std::string
  serialize(T const& model)
  {
    std::ostringstream str;
    serialize(str, model);
    return str.str();
  }

  template<typename T>
  T
  deserialize(diagnostics& d,
              std::istream& i,
              marshaling_context_t* context = nullptr)
  {
    return unmarshal<T>(read(d, i), context);
  }

  template<typename T>
  T
  deserialize(std::istream& i, marshaling_context_t* context = nullptr)
  {
    return unmarshal<T>(read(i), context);
  }

  template<typename T>
  T
  deserialize(diagnostics& d,
              std::istream&& i,
              marshaling_context_t* context = nullptr)
  {
    return deserialize<T>(d, i, context);
  }

  template<typename T>
  T
  deserialize(std::istream&& i, marshaling_context_t* context = nullptr)
  {
    return deserialize<T>(i, context);
  }

  template<typename T>
  T
  deserialize(diagnostics& d,
              std::string const& str,
              marshaling_context_t* context = nullptr)
  {
    // TODO use std::string overload of read
    return deserialize<T>(
      d, std::istringstream{ str, std::ios::in | std::ios::binary }, context);
  }

  template<typename T>
  T
  deserialize(std::string const& str, marshaling_context_t* context = nullptr)
  {
    // TODO use std::string overload of read
    return deserialize<T>(
      std::istringstream{ str, std::ios::in | std::ios::binary }, context);
  }

} // namespace art::json

#endif
