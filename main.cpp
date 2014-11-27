#include "CFL.h"

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "USAGE: cfl <template1.cfl> [stackname]" << std::endl;
		return EXIT_FAILURE;
	}

	std::wstring stackName;

	if (argc == 3)
	{
		std::string name(argv[2]);
		stackName = std::wstring(name.begin(), name.end());
	}

	CFL cfl;

	picojson::value v = cfl.parse(argv[1], stackName);

	std::wcout << v.serialize() << std::endl;

	return EXIT_SUCCESS;
}
