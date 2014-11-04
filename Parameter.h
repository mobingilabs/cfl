#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>

class Parameter
{
	std::wstring name;
	std::wstring type;

public:
	Parameter(std::wstring name, std::wstring type) : name(name), type(type)
	{

	}

	std::wstring getName() const
	{
		return name;
	}

	std::wstring getType() const
	{
		return type;
	}
};

#endif // PARAMETER_H
