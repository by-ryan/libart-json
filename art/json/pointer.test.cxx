#include <art/json/pointer.hxx>

#include <string>
#include <iostream>

#define DEFINE_TEST(x) std::cout << x << '\n';
#define TEST_TRUE(x) if (!(x)) return __LINE__;
#define TEST_EQUAL(x, y) if ((x) != (y)) return __LINE__;

int
main()
{
  auto resolve = [](std::string const& data, char const* path)
  {
    art::json::pointer ptr{std::string{path}};
    return ptr.read(art::json::read(data));
  };

  DEFINE_TEST("simple object")
  {
    std::string json = R"JSON(
      { "haystack": "needle" }
    )JSON";

    auto variant = resolve(json, "/haystack");

    TEST_TRUE(variant->is_string());
    TEST_EQUAL(variant->get_string(), "needle");
  }

  DEFINE_TEST("simple array")
  {
    std::string json = R"JSON(
      [ "needle" ]
    )JSON";

    auto variant = resolve(json, "/0");

    TEST_TRUE(variant->is_string());
    TEST_EQUAL(variant->get_string(), "needle");
  }

  DEFINE_TEST("complex")
  {
    std::string json = R"JSON(
      {
        "employees": [
          {
            "name": "Doe, John",
            "departments": ["HQ"],
            "age": 37
          },
          {
            "name": "Doe, Jane",
            "departments": ["Software", "Finance"],
            "age": 29
          }
        ]
      }
    )JSON";

    auto emp_0_name = resolve(json, "/employees/0/name");
    auto emp_0_department_0 = resolve(json, "/employees/0/departments/0");
    auto emp_0_age = resolve(json, "/employees/0/age");

    auto emp_1_name = resolve(json, "/employees/1/name");
    auto emp_1_department_0 = resolve(json, "/employees/1/departments/0");
    auto emp_1_department_1 = resolve(json, "/employees/1/departments/1");
    auto emp_1_age = resolve(json, "/employees/1/age");

    TEST_TRUE(emp_0_name->is_string());
    TEST_TRUE(emp_0_department_0->is_string());
    TEST_TRUE(emp_0_age->is_number());
    TEST_TRUE(emp_0_age->is_unsigned());

    TEST_TRUE(emp_1_name->is_string());
    TEST_TRUE(emp_1_department_0->is_string());
    TEST_TRUE(emp_1_department_1->is_string());
    TEST_TRUE(emp_1_age->is_number());
    TEST_TRUE(emp_1_age->is_unsigned());

    TEST_EQUAL(emp_0_name->get_string(), "Doe, John");
    TEST_EQUAL(emp_0_department_0->get_string(), "HQ");
    TEST_EQUAL(emp_0_age->get_number< signed int >(), 37);
    TEST_EQUAL(emp_0_age->get_number< unsigned int >(), 37);

    TEST_EQUAL(emp_1_name->get_string(), "Doe, Jane");
    TEST_EQUAL(emp_1_department_0->get_string(), "Software");
    TEST_EQUAL(emp_1_department_1->get_string(), "Finance");
    TEST_EQUAL(emp_1_age->get_number< signed int >(), 29);
    TEST_EQUAL(emp_1_age->get_number< unsigned int >(), 29);
  }

  return 0;
}
