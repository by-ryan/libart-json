namespace art::json {

inline variant::variant() : value_{ undefined_t{} }
{}

inline variant::variant(undefined_t) : value_{ undefined_t{} }
{}

inline variant::variant(bool value) : value_{ value }
{}

inline variant::variant(short int value)
  : value_{ static_cast< long long int >(value) }
{}

inline variant::variant(int value)
  : value_{ static_cast< long long int >(value) }
{}

inline variant::variant(long int value)
  : value_{ static_cast< long long int >(value) }
{}

inline variant::variant(long long int value) : value_{ value }
{}

inline variant::variant(unsigned short int value)
  : value_{ static_cast< unsigned long long int >(value) }
{}

inline variant::variant(unsigned int value)
  : value_{ static_cast< unsigned long long int >(value) }
{}

inline variant::variant(unsigned long int value)
  : value_{ static_cast< unsigned long long int >(value) }
{}

inline variant::variant(unsigned long long int value) : value_{ value }
{}

inline variant::variant(float value)
  : value_{ static_cast< long double >(value) }
{}

inline variant::variant(double value)
  : value_{ static_cast< long double >(value) }
{}

inline variant::variant(long double value) : value_{ value }
{}

inline variant::variant(std::string value) : value_{ value }
{}

inline variant::variant(char const* value) : value_{ std::string{ value } }
{}

inline variant::variant(std::vector< variant > value) : value_{ std::move(value) }
{}

inline variant::variant(std::map< std::string, variant > value) : value_{ std::move(value) }
{}

inline variant::variant(diagnostics::location location, undefined_t)
  : location_{ std::move(location) }, value_{ undefined_t{} }
{}

inline variant::variant(diagnostics::location location, bool value)
  : location_{ std::move(location) }, value_{ value }
{}

inline variant::variant(diagnostics::location location, short int value)
  : location_{ std::move(location) },
    value_{ static_cast< long long int >(value) }
{}

inline variant::variant(diagnostics::location location, int value)
  : location_{ std::move(location) },
    value_{ static_cast< long long int >(value) }
{}

inline variant::variant(diagnostics::location location, long int value)
  : location_{ std::move(location) },
    value_{ static_cast< long long int >(value) }
{}

inline variant::variant(diagnostics::location location, long long int value)
  : location_{ std::move(location) }, value_{ value }
{}

inline variant::variant(diagnostics::location location,
                        unsigned short int value)
  : location_{ std::move(location) },
    value_{ static_cast< unsigned long long int >(value) }
{}

inline variant::variant(diagnostics::location location, unsigned int value)
  : location_{ std::move(location) },
    value_{ static_cast< unsigned long long int >(value) }
{}

inline variant::variant(diagnostics::location location, unsigned long int value)
  : location_{ std::move(location) },
    value_{ static_cast< unsigned long long int >(value) }
{}

inline variant::variant(diagnostics::location location,
                        unsigned long long int value)
  : location_{ std::move(location) }, value_{ value }
{}

inline variant::variant(diagnostics::location location, float value)
  : location_{ std::move(location) },
    value_{ static_cast< long double >(value) }
{}

inline variant::variant(diagnostics::location location, double value)
  : location_{ std::move(location) },
    value_{ static_cast< long double >(value) }
{}

inline variant::variant(diagnostics::location location, long double value)
  : location_{ std::move(location) }, value_{ value }
{}

inline variant::variant(diagnostics::location location, std::string value)
  : location_{ std::move(location) }, value_{ std::move(value) }
{}

inline variant::variant(diagnostics::location location, char const* value)
  : location_{ std::move(location) }, value_{ std::string{ value } }
{}

inline variant::variant(diagnostics::location location, std::vector< variant > value)
  : location_{ std::move(location) }, value_{ std::move(value) }
{}

inline variant::variant(diagnostics::location location, std::map< std::string, variant > value)
  : location_{ std::move(location) }, value_{ std::move(value) }
{}

inline bool
variant::is_undefined() const
{
  return std::holds_alternative< undefined_t >(value_);
}

inline bool
variant::is_boolean() const
{
  return std::holds_alternative< bool >(value_);
}

inline bool
variant::get_boolean() const
{
  if (std::holds_alternative< bool >(value_))
    return std::get< bool >(value_);

  throw invalid_type{};
}

inline bool
variant::is_number() const
{
  return is_signed() || is_unsigned() || is_real();
}

inline bool
variant::is_signed() const
{
  return std::holds_alternative< long long int >(value_);
}

inline bool
variant::is_unsigned() const
{
  return std::holds_alternative< unsigned long long int >(value_);
}

inline bool
variant::is_real() const
{
  return std::holds_alternative< long double >(value_);
}

inline bool
variant::is_string() const
{
  return std::holds_alternative< std::string >(value_);
}

inline std::string const&
variant::get_string() const
{
  if (std::holds_alternative< std::string >(value_))
    return std::get< std::string >(value_);

  throw invalid_type{};
}

inline bool
variant::is_array() const
{
  return std::holds_alternative< std::vector< variant > >(value_);
}

inline bool
variant::empty() const
{
  if (std::holds_alternative< std::vector< variant > >(value_))
    return std::get< std::vector< variant > >(value_).empty();

  throw invalid_type{};
}

inline std::size_t
variant::size() const
{
  if (std::holds_alternative< std::vector< variant > >(value_))
    return std::get< std::vector< variant > >(value_).size();

  throw invalid_type{};
}

inline variant::iterator
variant::begin()
{
  if (std::holds_alternative< std::vector< variant > >(value_))
    return std::get< std::vector< variant > >(value_).begin();

  throw invalid_type{};
}

inline variant::const_iterator
variant::begin() const
{
  if (std::holds_alternative< std::vector< variant > >(value_))
    return std::get< std::vector< variant > >(value_).begin();

  throw invalid_type{};
}

inline variant::const_iterator
variant::cbegin() const
{
  if (std::holds_alternative< std::vector< variant > >(value_))
    return std::get< std::vector< variant > >(value_).cbegin();

  throw invalid_type{};
}

inline variant::iterator
variant::end()
{
  if (std::holds_alternative< std::vector< variant > >(value_))
    return std::get< std::vector< variant > >(value_).end();

  throw invalid_type{};
}

inline variant::const_iterator
variant::end() const
{
  if (std::holds_alternative< std::vector< variant > >(value_))
    return std::get< std::vector< variant > >(value_).end();

  throw invalid_type{};
}

inline variant::const_iterator
variant::cend() const
{
  if (std::holds_alternative< std::vector< variant > >(value_))
    return std::get< std::vector< variant > >(value_).cend();

  throw invalid_type{};
}

inline variant&
variant::get(std::size_t index)
{
  if (std::holds_alternative< std::vector< variant > >(value_))
    return std::get< std::vector< variant > >(value_)[index];

  throw invalid_type{};
}

inline variant const&
variant::get(std::size_t index) const
{
  if (std::holds_alternative< std::vector< variant > >(value_))
    return std::get< std::vector< variant > >(value_)[index];

  throw invalid_type{};
}

inline variant const&
variant::cget(std::size_t index) const
{
  return get(index);
}

inline void
variant::push_back(variant v)
{
  if (!std::holds_alternative< std::vector< variant > >(value_))
    throw invalid_type{};

  std::get< std::vector< variant > >(value_).push_back(std::move(v));
}

inline void
variant::erase(std::size_t index)
{
  if (!std::holds_alternative< std::vector< variant > >(value_))
    throw invalid_type{};

  auto& v = std::get< std::vector< variant > >(value_);

  if (index >= v.size())
    throw std::out_of_range{"index"};

  v.erase(v.begin() + index);
}

inline bool
variant::is_object() const
{
  return std::holds_alternative< std::map< std::string, variant > >(value_);
}

inline bool
variant::contains(std::string const& key) const
{
  if (std::holds_alternative< std::map< std::string, variant > >(value_))
    return std::get< std::map< std::string, variant > >(value_).count(key) > 0;

  throw invalid_type{};
}

inline std::set< std::string >
variant::keys() const
{
  if (std::holds_alternative< std::map< std::string, variant > >(value_)) {
    std::set< std::string > keys;

    for (auto const& j : std::get< std::map< std::string, variant > >(value_))
      keys.emplace(j.first);

    return keys;
  }

  throw invalid_type{};
}

inline variant&
variant::get(std::string const& key)
{
  if (std::holds_alternative< std::map< std::string, variant > >(value_))
    return std::get< std::map< std::string, variant > >(value_).at(key);

  throw invalid_type{};
}

inline variant const&
variant::get(std::string const& key) const
{
  if (std::holds_alternative< std::map< std::string, variant > >(value_))
    return std::get< std::map< std::string, variant > >(value_).at(key);

  throw invalid_type{};
}

inline variant const&
variant::cget(std::string const& key) const
{
  if (std::holds_alternative< std::map< std::string, variant > >(value_))
    return std::get< std::map< std::string, variant > >(value_).at(key);

  throw invalid_type{};
}

inline void
variant::set(std::string const& key, variant v)
{
  if (!std::holds_alternative< std::map< std::string, variant > >(value_))
    throw invalid_type{};

  std::get< std::map< std::string, variant > >(value_)[key] = std::move(v);
}

inline void
variant::erase(std::string const& key)
{
  if (!std::holds_alternative< std::map< std::string, variant > >(value_))
    throw invalid_type{};

  std::get< std::map< std::string, variant > >(value_).erase(key);
}

inline diagnostics::location const&
variant::location() const
{
  return location_;
}

} // namespace art::json
