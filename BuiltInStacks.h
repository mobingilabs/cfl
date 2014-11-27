#ifndef BUILTINSTACKS_H
#define BUILTINSTACKS_H

#include <map>
#include <memory>

class BuiltInStacks
{
	std::map<std::wstring, std::string> map;

public:

	BuiltInStacks()
	{
		
	}

	std::string getStack(std::wstring stackName)
	{
		return this->map[stackName];
	}

};

#endif // BUILTINSTACKS_H
