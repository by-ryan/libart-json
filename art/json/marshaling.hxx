#ifndef art__json__marshaling_hxx_
#define art__json__marshaling_hxx_

#include <art/json/optional.hxx>
#include <art/json/pointer.hxx>
#include <art/json/traits.hxx>
#include <art/json/variant.hxx>

#include <chrono>
#include <ctime>
#include <deque>
#include <iomanip>
#include <list>
#include <locale>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <iostream> // fixme: remove

namespace art::json
{

  class marshaling_context_t
  {
  protected:
    virtual ~marshaling_context_t() = default;

  };

  template<typename T>
  struct marshaling_traits;

  template<std::size_t N>
  struct member_name_t
  {
    constexpr member_name_t(char const (&str)[N])
    {
      std::copy_n(str, N, name);
    }

    std::string const
    str() const
    {
      return name;
    }

    operator std::string const() const
    {
      return str();
    }

    char name[N];

  };

  template<typename>
  struct member_traits;

  template<typename T, typename M>
  struct member_traits<M T::*>
  {
    using class_type = T;
    using member_type = M;

  };

  template<typename, typename = std::void_t<>>
  struct has_validator
    : std::false_type
  {};

  template<typename T>
  struct has_validator<
      T,
      std::void_t<
        decltype(std::declval<T const&>().validate(std::declval<marshaling_context_t*>()))
      >
    >
    : std::true_type
  {};

  template<member_name_t Name, auto Member, typename Validator = void>
  struct member_t
  {
    using T = member_traits<decltype(Member)>::class_type;
    using M = member_traits<decltype(Member)>::member_type;
    using V = Validator;

    static constexpr bool is_optional{
      is_optional_v<M>
    };

    static
    void
    marshal(variant& v, T const& instance, marshaling_context_t* context)
    {
      v.set(Name, marshaling_traits<M>::marshal(instance.*Member, context));
    }

    static
    void
    unmarshal(T& instance, variant const& v, marshaling_context_t* context)
    {
      if (!v.contains(Name.str())) {
        if (is_optional) {
          return;
        }

        throw std::runtime_error{"missing field '" + Name.str() + "'"};
      }

      M value = marshaling_traits<M>::unmarshal(v.get(Name), context);

      if constexpr (!std::is_same_v<V, void>) {
        V::validate(value, context);
      }

      instance.*Member = std::move(value);
    }

    struct pointer_t
    {
      using T = member_traits<decltype(Member)>::class_type;
      using M = member_traits<decltype(Member)>::member_type;

      static pointer const&
      ptr()
      {
        static pointer ptr{Name};
        return ptr;
      }

      static
      void
      marshal(variant& v, T const& instance, marshaling_context_t* context)
      {
        ptr().write(v, marshaling_traits<M>::marshal(instance.*Member, context));
      }

      static
      void
      unmarshal(T& instance, variant const& v, marshaling_context_t* context)
      {
        auto ptr_v = ptr().read(v);

        if (!ptr_v) {
          if (is_optional) {
            return;
          }

          throw std::runtime_error{ "missing field '" + to_string(ptr()) + "'" };
        }

        M value = marshaling_traits<M>::unmarshal(*ptr_v, context);

        if constexpr (!std::is_same_v<V, void>) {
          V::validate(value, context);
        }

        instance.*Member = std::move(value);
      }

    };

  };

  template<typename First, typename... Members>
  struct mapping_t
  {
    using T = First::T;

    static
    void
    do_marshal(variant& v, T const& instance, marshaling_context_t* context)
    {
      First::marshal(v, instance, context);
      ((Members::marshal(v, instance, context)), ...);
    }

    static
    variant
    marshal(T const& instance, marshaling_context_t* context)
    {
      variant v{std::map<std::string, variant>{}};
      do_marshal(v, instance, context);
      return v;
    }

    static
    void
    do_unmarshal(T& instance, variant const& v, marshaling_context_t* context)
    {
      First::unmarshal(instance, v, context);
      ((Members::unmarshal(instance, v, context)), ...);

      if constexpr (has_validator<T>::value) {
        instance.validate(context);
      }
    }

    static
    T
    unmarshal(variant const& v, marshaling_context_t* context)
    {
      T instance;
      do_unmarshal(instance, v, context);
      return instance;
    }

    template<typename... Bases>
    struct inherit_t
    {
      static_assert(sizeof...(Bases) > 0, "at least one base must be specified");

      static
      void
      do_marshal(variant& v, T const& instance, marshaling_context_t* context)
      {
        ((Bases::json::do_marshal(v, instance, context)), ...);
        First::marshal(v, instance, context);
        ((Members::marshal(v, instance, context)), ...);
      }

      static
      variant
      marshal(T const& instance, marshaling_context_t* context)
      {
        variant v{std::map<std::string, variant>{}};
        do_marshal(v, instance, context);
        return v;
      }

      static
      void
      do_unmarshal(T& instance, variant const& v, marshaling_context_t* context)
      {
        ((Bases::json::do_unmarshal(instance, v, context)), ...);
        First::unmarshal(instance, v, context);
        ((Members::unmarshal(instance, v, context)), ...);

        if constexpr (has_validator<T>::value) {
          instance.validate(context);
        }
      }

      static
      T
      unmarshal(variant const& v, marshaling_context_t* context)
      {
        T instance;
        do_unmarshal(instance, v, context);
        return instance;
      }

    };

  };

  template<member_name_t Name, typename Type>
  struct emplace_member_t
  {
    using T = Type;

    static constexpr bool is_optional{
      is_optional_v<T>
    };

    static
    T
    unmarshal(variant const& v, marshaling_context_t* context)
    {
      if (!v.contains(Name.str())) {
        if constexpr (is_optional) {
          return std::nullopt;
        }

        throw std::runtime_error{"missing field '" + Name.str() + "'"};
      }

      return marshaling_traits<T>::unmarshal(v.get(Name), context);
    }

  };

  template<typename T, typename... Members>
  struct emplace_t
  {
    static
    T
    unmarshal(variant const& v, marshaling_context_t* context)
    {
      return T{
        Members::unmarshal(v, context)...
      };
    }

  };

  template<typename T>
  struct marshaling_traits
  {
    static
    variant
    marshal(T const& instance, marshaling_context_t* context)
    {
      return T::json::marshal(instance, context);
    }

    static
    T
    unmarshal(variant const& v, marshaling_context_t* context)
    {
      return T::json::unmarshal(v, context);
    }

  };

  template<>
  struct marshaling_traits<variant>
  {
    static
    variant
    marshal(variant const& v, marshaling_context_t* context)
    {
      return v;
    }

    static
    variant
    unmarshal(variant const& v, marshaling_context_t* context)
    {
      return v;
    }

  };

  template<typename T>
  struct marshaling_traits<optional<T>>
  {
    static
    variant
    marshal(optional<T> const& model, marshaling_context_t* context)
    {
      if (model) {
        return marshaling_traits<T>::marshal(*model, context);
      }

      return {};
    }

    static optional<T>
    unmarshal(variant const& value, marshaling_context_t* context)
    {
      if (value.is_undefined()) {
        return std::nullopt;
      }

      return marshaling_traits<T>::unmarshal(value, context);
    }
  };

  template<typename... T>
  struct marshaling_traits<std::variant<T...>>
  {
    static
    variant
    marshal(std::variant<T...> const& model, marshaling_context_t* context)
    {
      variant result;

      auto visitor = [&result, context](const auto& obj)
      {
        using type = std::decay_t<decltype(obj)>;
        result = marshaling_traits<type>::marshal(obj, context);
        result.set("$type", std::decay_t<decltype(obj)>::type_identifier);
      };

      std::visit(visitor, model);

      return result;
    }

    template<typename Current, typename... Next>
    struct unmarshaler
    {
      static
      std::variant<T...>
      unmarshal(variant const& value, marshaling_context_t* context)
      {
        if (!value.contains("$type"))
          throw std::invalid_argument{"variant missing '$type' identifier"};

        if (value.get("$type").get_string() == Current::type_identifier)
          return marshaling_traits<Current>::unmarshal(value, context);

        if constexpr (sizeof...(Next)> 0)
          return unmarshaler<Next...>::unmarshal(value, context);

        throw std::invalid_argument{"couldn't unmarshal value"};
      }
    };

    static
    std::variant<T...>
    unmarshal(variant const& value, marshaling_context_t* context)
    {
      return unmarshaler<T...>::unmarshal(value, context);
    }
  };

  template<typename T>
  struct marshaling_traits<std::vector<T>>
  {
    static
    variant
    marshal(std::vector<T> const& model, marshaling_context_t* context)
    {
      std::vector<variant> a;

      for (auto const& j : model)
        a.emplace_back(marshaling_traits<T>::marshal(j, context));

      return a;
    }

    static
    std::vector<T>
    unmarshal(variant const& value, marshaling_context_t* context)
    {
      if (!value.is_array())
        throw std::runtime_error{"not an array"};

      std::vector<T> model;

      for (auto const& j : value)
        model.emplace_back(marshaling_traits<T>::unmarshal(j, context));

      return model;
    }
  };

  template<typename T>
  struct marshaling_traits<std::list<T>>
  {
    static
    variant
    marshal(std::list<T> const& model, marshaling_context_t* context)
    {
      std::vector<variant> a;

      for (auto const& j : model)
        a.emplace_back(marshaling_traits<T>::marshal(j, context));

      return a;
    }

    static
    std::list<T>
    unmarshal(variant const& value, marshaling_context_t* context)
    {
      if (!value.is_array())
        throw std::runtime_error{"not an array"};

      std::list<T> model;

      for (auto const& j : value)
        model.emplace_back(marshaling_traits<T>::unmarshal(j, context));

      return model;
    }
  };

  template<typename T>
  struct marshaling_traits<std::deque<T>>
  {
    static
    variant
    marshal(std::deque<T> const& model, marshaling_context_t* context)
    {
      std::vector<variant> a;

      for (auto const& j : model)
        a.emplace_back(marshaling_traits<T>::marshal(j, context));

      return a;
    }

    static
    std::deque<T>
    unmarshal(variant const& value, marshaling_context_t* context)
    {
      if (!value.is_array())
        throw std::runtime_error{"not an array"};

      std::deque<T> model;

      for (auto const& j : value)
        model.emplace_back(marshaling_traits<T>::unmarshal(j, context));

      return model;
    }
  };

  template<typename T>
  struct marshaling_traits<std::set<T>>
  {
    static
    variant
    marshal(std::set<T> const& model, marshaling_context_t* context)
    {
      std::vector<variant> a;

      for (auto const& j : model)
        a.emplace_back(marshaling_traits<T>::marshal(j, context));

      return a;
    }

    static
    std::set<T>
    unmarshal(variant const& value, marshaling_context_t* context)
    {
      if (!value.is_array())
        throw std::runtime_error{"not an array"};

      std::set<T> model;

      for (auto const& j : value)
        model.emplace(marshaling_traits<T>::unmarshal(j, context));

      return model;
    }
  };

  template<>
  struct marshaling_traits<bool>
  {
    static
    variant
    marshal(bool model, marshaling_context_t*)
    {
      return model;
    }

    static
    bool
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_boolean())
        throw std::runtime_error{"not a boolean"};

      return value.get_boolean();
    }
  };

  template<>
  struct marshaling_traits<short int> {
    static
    variant
    marshal(short int model, marshaling_context_t*)
    {
      return model;
    }

    static
    short int
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_number())
        throw std::runtime_error{"not a number"};

      return value.get_number<short int>();
    }
  };

  template<>
  struct marshaling_traits<int>
  {
    static
    variant
    marshal(int model, marshaling_context_t*)
    {
      return model;
    }

    static
    int
    unmarshal(variant const& v, marshaling_context_t*)
    {
      if (!v.is_number()) {
        throw std::runtime_error{"not a number"};
      }

      return v.get_number<int>();
    }

  };

  template<>
  struct marshaling_traits<long int> {
    static
    variant
    marshal(long int model, marshaling_context_t*)
    {
      return model;
    }

    static
    long int
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_number())
        throw std::runtime_error{"not a number"};

      return value.get_number<long int>();
    }
  };

  template<>
  struct marshaling_traits<long long int> {
    static
    variant
    marshal(long long int model, marshaling_context_t*)
    {
      return model;
    }

    static
    long long int
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_number())
        throw std::runtime_error{"not a number"};

      return value.get_number<long long int>();
    }
  };

  template<>
  struct marshaling_traits<unsigned short int> {
    static
    variant
    marshal(unsigned short int model, marshaling_context_t*)
    {
      return model;
    }

    static
    unsigned short int
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_number())
        throw std::runtime_error{"not a number"};

      return value.get_number<unsigned short int>();
    }
  };

  template<>
  struct marshaling_traits<unsigned int> {
    static
    variant
    marshal(unsigned int model, marshaling_context_t*)
    {
      return model;
    }

    static
    unsigned int
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_number())
        throw std::runtime_error{"not a number"};

      return value.get_number<unsigned int>();
    }
  };

  template<>
  struct marshaling_traits<unsigned long int> {
    static
    variant
    marshal(unsigned long int model, marshaling_context_t*)
    {
      return model;
    }

    static
    unsigned long int
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_number())
        throw std::runtime_error{"not a number"};

      return value.get_number<unsigned long int>();
    }
  };

  // unsigned long long int

  template<>
  struct marshaling_traits<unsigned long long int> {
    static
    variant
    marshal(unsigned long long int model, marshaling_context_t*)
    {
      return model;
    }

    static
    unsigned long long int
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_number())
        throw std::runtime_error{"not a number"};

      return value.get_number<unsigned long long int>();
    }
  };

  template<>
  struct marshaling_traits<float> {
    static
    variant
    marshal(float model, marshaling_context_t*)
    {
      return model;
    }

    static
    float
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_number())
        throw std::runtime_error{"not a number"};

      return value.get_number<float>();
    }
  };

  template<>
  struct marshaling_traits<double> {
    static
    variant
    marshal(double model, marshaling_context_t*)
    {
      return model;
    }

    static
    double
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_number())
        throw std::runtime_error{"not a number"};

      return value.get_number<double>();
    }
  };

  template<>
  struct marshaling_traits<long double> {
    static
    variant
    marshal(long double model, marshaling_context_t*)
    {
      return model;
    }

    static
    long double
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_number())
        throw std::runtime_error{"not a number"};

      return value.get_number<long double>();
    }
  };

  template<>
  struct marshaling_traits<std::string>
  {
    static
    variant
    marshal(std::string const& model, marshaling_context_t*)
    {
      return model;
    }

    static
    std::string
    unmarshal(variant const& v, marshaling_context_t*)
    {
      if (!v.is_string()) {
        throw std::runtime_error{"not a string"};
      }

      return v.get_string();
    }

  };

  template<>
  struct marshaling_traits<std::chrono::system_clock::time_point>
  {
    using model_type = std::string;

    static constexpr const char time_format[] = "%a, %d %b %Y %H:%M:%S GMT";

    static
    variant
    marshal(std::chrono::system_clock::time_point const& model, marshaling_context_t*)
    {
      std::time_t now_c = std::chrono::system_clock::to_time_t(model);

      struct std::tm tm_buf;

      std::stringstream str;
      str.imbue(std::locale{});

#ifdef _MSC_VER
      ::gmtime_s(&tm_buf, &now_c); // Stupid Microsoft.
      str << std::put_time(&tm_buf, time_format);
#else
      ::gmtime_r(&now_c, &tm_buf);
      str << std::put_time(&tm_buf, time_format);
#endif

      return str.str();
    }

    static
    std::chrono::system_clock::time_point
    unmarshal(variant const& value, marshaling_context_t*)
    {
      if (!value.is_string())
        throw std::runtime_error{ "not a string" };

      std::tm tm{};

      std::istringstream str{ value.get_string() };
      str.imbue(std::locale{});

      str >> std::get_time(&tm, time_format);

      if (str.fail())
        return {};

#ifdef _MSC_VER
      auto localtime = _mkgmtime(&tm);
      return std::chrono::system_clock::from_time_t(localtime);
#else
      auto localtime = timegm(&tm);
      return std::chrono::system_clock::from_time_t(localtime);
#endif
    }
  };

  template<typename T>
  variant
  marshal(T const& model, marshaling_context_t* context = nullptr)
  {
    return marshaling_traits<T>::marshal(model, context);
  }

  template<typename T>
  T
  unmarshal(variant const& v, marshaling_context_t* context = nullptr)
  {
    return marshaling_traits<T>::unmarshal(v, context);
  }

} // namespace art::json

#endif
