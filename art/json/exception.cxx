#include <art/json/exception.hxx>

namespace art::json {

invalid_json::invalid_json(diagnostics::location const& loc,
                           std::string const& what)
  : std::runtime_error{ make_error_string(loc, what) }, location_{ loc }
{}

diagnostics::location const&
invalid_json::location() const
{
  return location_;
}

std::string
invalid_json::make_error_string(diagnostics::location const& loc,
                                std::string const& what)
{
  std::ostringstream str;
  str << loc.column << " " << loc.line << ": " << what;
  return str.str();
}

invalid_type::invalid_type() : std::runtime_error{ "invalid type" }
{}

invalid_syntax::invalid_syntax() : std::runtime_error{ "invalid syntax" }
{}

unexpected_type::unexpected_type()
  : std::runtime_error{ "unexpected JSON type" }
{}

invalid_object_key::invalid_object_key(std::string const& key)
  : std::runtime_error{ "invalid key '" + key + "'" }
{}

invalid_array_index::invalid_array_index(std::string const& index)
  : std::runtime_error{ "invalid key '" + index + "'" }
{}

} // namespace art::json
