#include <art/json/diagnostics.hxx>

namespace art::json {

std::vector< std::pair< diagnostics::location, std::string > > const&
diagnostics::warnings() const
{
  return warnings_;
}

std::vector< std::pair< diagnostics::location, std::string > > const&
diagnostics::errors() const
{
  return errors_;
}

void
diagnostics::warning(location loc, std::string description)
{
  warnings_.emplace_back(std::make_pair(loc, description));
}

void
diagnostics::error(location loc, std::string description)
{
  errors_.emplace_back(std::make_pair(loc, description));
}

std::ostream&
operator<<(std::ostream& o, diagnostics const& d)
{
  for (auto const& j : d.warnings()) {
    o << "warning: " << j.first.line << ':' << j.first.column << ": "
      << j.second << '\n';
  }

  for (auto const& j : d.errors()) {
    o << "error: " << j.first.line << ':' << j.first.column << ": " << j.second
      << '\n';
  }

  return o;
}

} // namespace art::json
