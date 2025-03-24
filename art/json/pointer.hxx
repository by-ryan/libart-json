#ifndef art__json__pointer_hxx_
#define art__json__pointer_hxx_

#include <art/json/exception.hxx>
#include <art/json/optional.hxx>
#include <art/json/read.hxx>
#include <art/json/variant.hxx>

#include <queue>
#include <string>

namespace art::json {

class pointer {
public:
  explicit
  pointer(std::string const& expression);

  optional< variant >
  read(variant const& source) const;

  void
  write(variant& target, variant value) const;

  friend
  std::string
  to_string(pointer const& ptr);

private:
  std::queue< std::string >
  parse_refs(std::string const& expression);

  std::queue< std::string > refs_;
};

std::string
to_string(pointer const& ptr);

} // namespace art::json

#endif
