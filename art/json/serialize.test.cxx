#include <art/json/marshaling.hxx>
#include <art/json/serialize.hxx>

#include <iostream>

#define DEFINE_TEST(x) std::cout << x << '\n';
#define TEST_TRUE(x) if (!(x)) return __LINE__;
#define TEST_FALSE(x) if ((x)) return __LINE__;
#define TEST_EQUAL(x, y) if ((x) != (y)) return __LINE__;

using art::json::mapping_t;
using art::json::member_t;
using art::json::serialize;
using art::json::deserialize;

struct person_name {
  std::string first;
  std::string last;

  using json = mapping_t<
    member_t<"first", &person_name::first>,
    member_t<"last", &person_name::last>
  >;

};

struct person {
  person_name name;
  int age;

  using json = mapping_t<
    member_t<"name", &person::name>,
    member_t<"age", &person::age>
  >;

};

int
main()
{
  DEFINE_TEST("round trip")
  {
    person p1{{"Jane", "Doe"}, 37};

    auto p2 = deserialize<person>(serialize(p1));

    TEST_EQUAL(p1.name.first, p2.name.first);
    TEST_EQUAL(p1.name.last, p2.name.last);
    TEST_EQUAL(p1.age, p2.age);
  }

  return 0;
}
