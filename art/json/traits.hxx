#ifndef art__json__traits_hxx_
#define art__json__traits_hxx_

#include <art/json/optional.hxx>

#include <type_traits>

namespace art::json {

// is_optional<T>.
//

template< typename >
struct is_optional : std::false_type {
};

template< typename T >
struct is_optional< optional< T > > : std::true_type {
};

template< typename T >
constexpr bool is_optional_v = is_optional< T >::value;

// function_traits<T>.
//

template< typename, typename = std::void_t< > >
struct function_traits;

template< typename Ret, typename... Args >
struct function_traits< Ret(Args...) > {
  static constexpr std::size_t arity = sizeof...(Args);
  using return_type = std::decay_t< Ret >;
  using argument_tuple = std::tuple< std::decay_t< Args >... >;
};

template< typename Ret, typename Class, typename... Args >
struct function_traits< Ret (Class::*)(Args...) > {
  static constexpr std::size_t arity = sizeof...(Args);
  using return_type = std::decay_t< Ret >;
  using class_type = Class;
  using argument_tuple = std::tuple< std::decay_t< Args >... >;
};

template< typename Ret, typename Class, typename... Args >
struct function_traits< Ret (Class::*)(Args...) const > {
  static constexpr std::size_t arity = sizeof...(Args);
  using return_type = std::decay_t< Ret >;
  using class_type = Class;
  using argument_tuple = std::tuple< std::decay_t< Args >... >;
};

template< typename Ret, typename... Args >
struct function_traits< Ret(*)(Args...) > {
  static constexpr std::size_t arity = sizeof...(Args);
  using return_type = std::decay_t< Ret >;
  using argument_tuple = std::tuple< std::decay_t< Args >... >;
};

template< typename Callable >
struct function_traits<
  Callable,
  std::void_t<
    decltype(&Callable::operator())
  >
>
: function_traits<decltype(&Callable::operator())> {
};

} // namespace art::json

#endif
