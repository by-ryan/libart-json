#ifndef art__json__write_hxx_
#define art__json__write_hxx_

#include <art/json/emitter.hxx>

#include <ostream>
#include <sstream>
#include <string>

namespace art::json {

class variant;

inline void
write(std::ostream& os, variant const& v)
{
  visit(emitter{ os }, v);
  os << '\n';
}

inline std::string
write(variant const& json)
{
  std::stringstream str;
  write(str, json);
  return str.str();
}

} // namespace art::json

#endif
