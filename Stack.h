#ifndef STACK_H
#define STACK_H

#include <string>
#include <vector>
#include <map>

#include "Resource.h"
#include "Expression.h"
#include "Parameter.h"

class Stack
{
	std::wstring name;
	std::map< std::wstring, std::shared_ptr<Parameter> > parameters;
	std::vector< std::shared_ptr<Resource> > resources;
	std::map< std::wstring, std::shared_ptr<Expression> > outputs;

public:
	Stack(std::wstring name) : name(name)
	{

	}

	void AddParameter(std::shared_ptr<Parameter> parameter)
	{
		parameters[parameter->getName()] = parameter;
	}

	void AddResource(std::shared_ptr<Resource> resource)
	{
		resources.push_back(resource);
	}

	void AddOutput(std::wstring name, std::shared_ptr<Expression> expr)
	{
		outputs[name] = expr;
	}

	std::wstring getName() const
	{
		return name;
	}

	std::map< std::wstring, std::shared_ptr<Parameter> > getParameters() const
	{
		return parameters;
	}

	std::vector< std::shared_ptr<Resource> > getResources() const
	{
		return resources;
	}

	std::map< std::wstring, std::shared_ptr<Expression> > getOutputs() const
	{
		return outputs;
	}
};

#endif // STACK_H