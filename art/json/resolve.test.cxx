#include <art/json/pointer.hxx>
#include <art/json/write.hxx>

#include <string>
#include <iostream>

int
main(int argc, char* argv[])
{
#if 0
	std::stringstream str;
	str << std::cin.rdbuf();

	auto var = art::json::read(str.str());

	std::cout << art::json::write(var);

	art::json::resolve(str.str(), argv[1]);


  return 0;
#endif
}
