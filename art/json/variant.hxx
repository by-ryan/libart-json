#ifndef art__json__variant_hxx_
#define art__json__variant_hxx_

#include <art/json/diagnostics.hxx>
#include <art/json/exception.hxx>

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class emitter;

namespace art::json {

//! Variant holding any valid JSON value.
class variant {
public:
  //! Array iterator type.
  using iterator = std::vector< variant >::iterator;

  //! Array iterator type.
  using const_iterator = std::vector< variant >::const_iterator;

  //! Type used to denote an undefined variant.
  struct undefined_t { };
  static constexpr undefined_t undefined{};

  //! Construct an undefined variant.
  variant();

  //! Construct an undefined variant.
  variant(undefined_t);

  //! Construct a variant holding a boolean value.
  variant(bool value);

  //! Construct a variant holding a signed number.
  variant(short int value);

  //! Construct a variant holding a signed number.
  variant(int value);

  //! Construct a variant holding a signed number.
  variant(long int value);

  //! Construct a variant holding a signed number.
  variant(long long int value);

  //! Construct a variant holding an unsigned number.
  variant(unsigned short int value);

  //! Construct a variant holding an unsigned number.
  variant(unsigned int value);

  //! Construct a variant holding an unsigned number.
  variant(unsigned long int value);

  //! Construct a variant holding an unsigned number.
  variant(unsigned long long int value);

  //! Construct a variant holding a real number.
  variant(float value);

  //! Construct a variant holding a real number.
  variant(double value);

  //! Construct a variant holding a real number.
  variant(long double value);

  //! Construct a variant holding a string.
  variant(std::string value);

  //! Construct a variant holding a string.
  variant(char const* value);

  //! Construct a variant holding an array.
  variant(std::vector< variant > value);

  //! Construct a variant holding an object.
  variant(std::map< std::string, variant > value);

  variant(diagnostics::location location, undefined_t);

  variant(diagnostics::location location, bool value);

  variant(diagnostics::location location, short int value);

  variant(diagnostics::location location, int value);

  variant(diagnostics::location location, long int value);

  variant(diagnostics::location location, long long int value);

  variant(diagnostics::location location, unsigned short int value);

  variant(diagnostics::location location, unsigned int value);

  variant(diagnostics::location location, unsigned long int value);

  variant(diagnostics::location location, unsigned long long int value);

  variant(diagnostics::location location, float value);

  variant(diagnostics::location location, double value);

  variant(diagnostics::location location, long double value);

  variant(diagnostics::location location, std::string value);

  variant(diagnostics::location location, char const* value);

  variant(diagnostics::location location, std::vector< variant > value);

  variant(diagnostics::location location, std::map< std::string, variant > value);

  //! Check if this variant is undefined.
  bool
  is_undefined() const;

  //! Check if this variant holds a boolean.
  bool
  is_boolean() const;

  //! Get boolean value.
  bool
  get_boolean() const;

  //! Check if this variant holds a number value.
  bool
  is_number() const;

  //! Check if this variant holds a signed value.
  bool
  is_signed() const;

  //! Check if this variant holds an unsigned value.
  bool
  is_unsigned() const;

  //! Check if this variant holds a real value.
  bool
  is_real() const;

  //! Get number value.
  template< typename ArithmeticType >
  ArithmeticType
  get_number() const;

  //! Check if this variant holds a string.
  bool
  is_string() const;

  //! Get string value.
  std::string const&
  get_string() const;

  //! Check if this variant holds an array.
  bool
  is_array() const;

  //! Check if empty.
  bool
  empty() const;

  //! Get array size.
  std::size_t
  size() const;

  //! Get array iterator.
  iterator
  begin();

  //! Get array iterator.
  const_iterator
  begin() const;

  //! Get array iterator.
  const_iterator
  cbegin() const;

  //! Get array iterator.
  iterator
  end();

  //! Get array iterator.
  const_iterator
  end() const;

  //! Get array iterator.
  const_iterator
  cend() const;

  //! Get value at array index.
  variant&
  get(std::size_t index);

  //! Get value at array index.
  variant const&
  get(std::size_t index) const;

  //! Get value at array index.
  variant const&
  cget(std::size_t index) const;

  //! Push value to back of array.
  void
  push_back(variant v);

  //! Remove element from array.
  void
  erase(std::size_t index);

  //! Check if this variant holds an object.
  bool
  is_object() const;

  //! Check if object contains key.
  bool
  contains(std::string const& key) const;

  //! Get object keys.
  std::set< std::string >
  keys() const;

  //! Get object value.
  variant&
  get(std::string const& key);

  //! Get object value.
  variant const&
  get(std::string const& key) const;

  //! Get object value.
  variant const&
  cget(std::string const& key) const;

  //! Set object value.
  void
  set(std::string const& key, variant v);

  //! Remove element from object.
  void
  erase(std::string const& key);

  //! Get the location of this value.
  diagnostics::location const&
  location() const;

  template< typename Visitor >
  friend void
  visit(Visitor&& visitor, variant const& value)
  {
    std::visit(std::forward< Visitor >(visitor), value.value_);
  }


private:
  friend class emitter;

  //! Internal variant type.
  using variant_type = std::variant< undefined_t,
                                     bool,
                                     long long int,
                                     unsigned long long int,
                                     long double,
                                     std::string,
                                     std::vector< variant >,
                                     std::map< std::string, variant > >;

  //! Optional source location of this variant.
  diagnostics::location location_;

  //! The value.
  variant_type value_;
};

} // namespace art::json

#include <art/json/variant.ixx>
#include <art/json/variant.txx>

#endif
