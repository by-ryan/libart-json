#include <art/json/marshaling.hxx>
#include <art/json/optional.hxx>
#include <art/json/serialize.hxx>

#include <cstdint>
#include <deque>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#define DEFINE_TEST(x) std::cout << x << '\n';
#define TEST_TRUE(x) if (!(x)) return __LINE__;
#define TEST_FALSE(x) if ((x)) return __LINE__;
#define TEST_EQUAL(x, y) if ((x) != (y)) return __LINE__;

using art::json::mapping_t;
using art::json::marshal;
using art::json::marshaling_traits;
using art::json::member_t;
using art::json::unmarshal;

struct name_t
{
  std::string first;
  std::string last;

  using json = mapping_t<
    member_t<"/person/first", &name_t::first>::pointer_t,
    member_t<"/person/last", &name_t::last>::pointer_t
  >;

};

int
main()
{
  DEFINE_TEST("optional<int>{}")
  {
    using traits_type = marshaling_traits<art::json::optional<int>>;

    std::optional<int> model;

    auto j = traits_type::marshal(model, nullptr);

    TEST_TRUE(j.is_undefined());
  }

  DEFINE_TEST("optional<int>{0}")
  {
    using traits_type = marshaling_traits<art::json::optional<int>>;

    std::optional<int> model{0};

    auto j = traits_type::marshal(model, nullptr);

    TEST_FALSE(j.is_undefined());
    TEST_TRUE(j.is_number());
  }

  DEFINE_TEST("string")
  {
    using traits_type = marshaling_traits<std::string>;

    std::string model{"hello, world"};

    auto j = traits_type::marshal(model, nullptr);

    TEST_TRUE(j.is_string());
    TEST_EQUAL(j.get_string(), "hello, world");
  }

  DEFINE_TEST("vector<int>{}")
  {
    using traits_type = marshaling_traits<std::vector<int>>;

    std::vector<int> model1;

    auto j = traits_type::marshal(model1, nullptr);

    TEST_TRUE(j.is_array());

    auto model2 = traits_type::unmarshal(j, nullptr);

    TEST_EQUAL(model1, model2);
  }

  DEFINE_TEST("vector<int>{...}")
  {
    using traits_type = marshaling_traits<std::vector<int>>;

    std::vector<int> model1{ 1, 2, 3, 4 };

    auto j = traits_type::marshal(model1, nullptr);

    TEST_TRUE(j.is_array());

    auto model2 = traits_type::unmarshal(j, nullptr);

    TEST_EQUAL(model1, model2);
  }

  DEFINE_TEST("list<int>{}")
  {
    using traits_type = marshaling_traits<std::list<int>>;

    std::list<int> model1;

    auto j = traits_type::marshal(model1, nullptr);

    TEST_TRUE(j.is_array());

    auto model2 = traits_type::unmarshal(j, nullptr);

    TEST_EQUAL(model1, model2);
  }

  DEFINE_TEST("list<int>{...}")
  {
    using traits_type = marshaling_traits<std::list<int>>;

    std::list<int> model1{ 1, 2, 3, 4 };

    auto j = traits_type::marshal(model1, nullptr);

    TEST_TRUE(j.is_array());

    auto model2 = traits_type::unmarshal(j, nullptr);

    TEST_EQUAL(model1, model2);
  }

  DEFINE_TEST("deque<int>{}")
  {
    using traits_type = marshaling_traits<std::deque<int>>;

    std::deque<int> model1;

    auto j = traits_type::marshal(model1, nullptr);

    TEST_TRUE(j.is_array());

    auto model2 = traits_type::unmarshal(j, nullptr);

    TEST_EQUAL(model1, model2);
  }

  DEFINE_TEST("deque<int>{...}")
  {
    using traits_type = marshaling_traits<std::deque<int>>;

    std::deque<int> model1{ 1, 2, 3, 4 };

    auto j = traits_type::marshal(model1, nullptr);

    TEST_TRUE(j.is_array());

    auto model2 = traits_type::unmarshal(j, nullptr);

    TEST_EQUAL(model1, model2);
  }

  DEFINE_TEST("pointer")
  {
    name_t name1{"Jane", "Doe"};
    auto v = marshal<name_t>(name1);
    auto name2 = unmarshal<name_t>(v);

    TEST_EQUAL(name1.first, name2.first);
    TEST_EQUAL(name1.last, name2.last);
  }

  return 0;
}
