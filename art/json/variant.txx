namespace art::json {

template< typename ArithmeticType >
ArithmeticType
variant::get_number() const
{
  if constexpr (std::is_arithmetic_v< ArithmeticType >) {
    if (std::holds_alternative< long long int >(value_))
      return static_cast< ArithmeticType >(std::get< long long int >(value_));
    else if (std::holds_alternative< unsigned long long int >(value_))
      return static_cast< ArithmeticType >(std::get< unsigned long long int >(value_));
    else if (std::holds_alternative< long double >(value_))
      return static_cast< ArithmeticType >(std::get< long double >(value_));
  }

  throw invalid_type{};
}

} // namespace art::json
