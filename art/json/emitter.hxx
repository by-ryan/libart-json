#ifndef art__json__emitter_hxx_
#define art__json__emitter_hxx_

#include <art/json/variant.hxx>

#include <ostream>
#include <stack>

namespace art::json {

class emitter {
public:
  explicit emitter(std::ostream& output);

  std::ostream&
  output();

  void
  operator()(variant::undefined_t const& value);

  void
  operator()(bool const& value);

  void
  operator()(long long int const& value);

  void
  operator()(unsigned long long int const& value);

  void
  operator()(long double const& value);

  void
  operator()(std::string const& value);

  void
  operator()(std::vector< variant > const& value);

  void
  operator()(std::map< std::string, variant > const& value);

private:
  void
  do_indent();

  std::ostream& output_;
  std::size_t indent_{ 0 };
};

} // namespace art::json

#include <art/json/emitter.ixx>

#endif
