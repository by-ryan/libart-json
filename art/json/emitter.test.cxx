#include <art/json/emitter.hxx>

#include <iostream>
#include <sstream>

#define DEFINE_TEST(x) std::cout << x << '\n';
#define TEST_EQUAL(x, y) if ((x) != (y)) return __LINE__;

int
main()
{
  DEFINE_TEST("undefined")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };
    emitter(art::json::variant::undefined_t{});

    TEST_EQUAL(str.str(), "null");
  }

  DEFINE_TEST("boolean: true")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };
    emitter(true);

    TEST_EQUAL(str.str(), "true");
  }

  DEFINE_TEST("boolean: false")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };
    emitter(false);

    TEST_EQUAL(str.str(), "false");
  }

  DEFINE_TEST("signed number")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };
    emitter(-10LL);

    TEST_EQUAL(str.str(), "-10");
  }

  DEFINE_TEST("unsigned number")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };
    emitter(100LL);

    TEST_EQUAL(str.str(), "100");
  }

  DEFINE_TEST("real number")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };
    emitter(1.2L);

    TEST_EQUAL(str.str(), "1.2");
  }

  DEFINE_TEST("string")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };
    emitter(std::string{ "hello" });

    TEST_EQUAL(str.str(), "\"hello\"");
  }

  DEFINE_TEST("array: []")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };
    emitter(std::vector< art::json::variant >{});

    TEST_EQUAL(str.str(), "[]");
  }

  DEFINE_TEST("array: [null]")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };

    emitter(std::vector< art::json::variant >{ art::json::variant{} });

    TEST_EQUAL(str.str(), "[\n  null\n]");
  }

  DEFINE_TEST("object: {}")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };

    emitter(std::map< std::string, art::json::variant >{});

    TEST_EQUAL(str.str(), "{}");
  }

  DEFINE_TEST("object: {...}")
  {
    std::stringstream str;
    art::json::emitter emitter{ str };

    emitter(std::map< std::string, art::json::variant >{ { "", {} } });

    TEST_EQUAL(str.str(), "{\n  \"\": null\n}");
  }

  return 0;
}
