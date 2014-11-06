#ifndef RESOURCE_H
#define RESOURCE_H

#include <map>
#include <memory>
#include <string>
#include "Expression.h"
#include "Metadata.h"

class Resource
{
	std::map< std::wstring, std::shared_ptr<Expression> > properties;
	std::wstring type;
	std::wstring name;
	std::vector< std::shared_ptr<Metadata> > metadata;

public:
	Resource(std::wstring type, std::wstring name, std::vector< std::shared_ptr<Metadata> > metadata) : type(type), name(name), metadata(metadata)
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

	std::vector< std::shared_ptr<Metadata> > getMetadata() const
	{
		return metadata;
	}

	std::map<std::wstring, std::shared_ptr<Expression>> getProperties() const
	{
		return properties;
	}

};

#endif // RESOURCE_H
