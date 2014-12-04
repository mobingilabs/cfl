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
	std::stack< std::shared_ptr<Expression> > conditions;

public:
	Resource(std::wstring type, std::wstring name, std::vector< std::shared_ptr<Metadata> > metadata) : type(type), name(name), metadata(metadata)
	{

	}

	void AddProperty(std::wstring name, std::shared_ptr<Expression> value)
	{
		properties[name] = value;
	}

	void SetCondition(std::stack< std::shared_ptr<Expression> > conditions)
	{
		this->conditions = conditions;
		//std::wcerr << "Condition size: " << conditions.size() << std::endl;
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

	std::stack< std::shared_ptr<Expression> > getConditions() const
	{
		return conditions;
	}

};

#endif // RESOURCE_H
