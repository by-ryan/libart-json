#include <art/json/variant.hxx>
#include <art/json/write.hxx>

#include <iostream>
#include <sstream>

#define DEFINE_TEST(x) std::cout << x << '\n';
#define TEST_EQUAL(x, y) if ((x) != (y)) return __LINE__;

int
main()
{
  DEFINE_TEST("write")
  {
    auto document = std::map< std::string, art::json::variant >{
      { { "title", "Coca-Cola Regular 1.5L" },
                                    { "gtin13", "5449000139306" } }
    };

    std::stringstream output;
    art::json::write(output, document);

    TEST_EQUAL(output.str(), R"({
  "gtin13": "5449000139306",
  "title": "Coca-Cola Regular 1.5L"
}
)");
  }

  return 0;
}
