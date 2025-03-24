#ifndef art__json__exception_hxx_
#define art__json__exception_hxx_

#include <art/json/diagnostics.hxx>

#include <sstream>
#include <stdexcept>
#include <string>

namespace art::json {

/// Exception-class used to indicated invalid JSON.
class invalid_json : public std::runtime_error {
public:
  invalid_json(diagnostics::location const& loc, std::string const& what);

  diagnostics::location const&
  location() const;

  static std::string
  make_error_string(diagnostics::location const& loc, std::string const& what);

private:
  diagnostics::location location_;
};

/// Exception-class used to indicate an invalid type.
class invalid_type : public std::runtime_error {
public:
  invalid_type();
};

/// Exception class used to indicate invalid JSON pointer syntax.
class invalid_syntax : public std::runtime_error {
public:
  invalid_syntax();
};

/// Exception class used to indicate an unexpected JSON type.
class unexpected_type : public std::runtime_error {
public:
  unexpected_type();
};

/// Exception-class used to indicate an object key.
class invalid_object_key : public std::runtime_error {
public:
  explicit invalid_object_key(std::string const& key);
};

/// Exception-class used to indicate an invalid array index.
class invalid_array_index : public std::runtime_error {
public:
  explicit invalid_array_index(std::string const& index);
};

} // namespace art::json

#endif
