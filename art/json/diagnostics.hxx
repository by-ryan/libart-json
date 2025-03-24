#ifndef art__json__diagnostics_hxx_
#define art__json__diagnostics_hxx_

#include <cstdint>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace art::json {

class diagnostics
{
public:
  struct location
  {
    std::uint32_t line;
    std::uint32_t column;
  };

  std::vector< std::pair< diagnostics::location, std::string > > const&
  warnings() const;

  std::vector< std::pair< diagnostics::location, std::string > > const&
  errors() const;

  void
  warning(location loc, std::string description);

  void
  error(location loc, std::string description);

private:
  std::vector< std::pair< location, std::string > > warnings_;
  std::vector< std::pair< location, std::string > > errors_;
};

std::ostream&
operator<<(std::ostream& o, diagnostics const& d);

} // namespace art::json

#endif
