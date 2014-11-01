#ifndef RESOURCE_H
#define RESOURCE_H

#include <map>
#include <memory>
#include <string>
#include "Expression.h"

class Resource
{
	std::map<std::wstring, std::shared_ptr<Expression>> properties;
	std::wstring type;
	std::wstring name;

public:
	Resource(std::wstring type, std::wstring name) : type(type), name(name)
	{

	}

	void AddProperty(std::wstring name, std::shared_ptr<Expression> value)
	{
		properties[name] = value;
	}

	std::wstring getName() const
	{
		return name;
	}

	std::wstring getType() const
	{
		return type;
	}

	std::map<std::wstring, std::shared_ptr<Expression>> getProperties() const
	{
		return properties;
	}

};

#endif // RESOURCE_H
