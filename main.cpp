#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <typeinfo>
#include "picojson/picojson.h"

#include "Parser.h"
#include "Scanner.h"

template<typename T>
struct Info
{
	std::wstring name;
	StackPtr stack;
	T item;
};

bool replaceOne(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void replace(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
	while (replaceOne(str, from, to)) { }
}

class UnknownTypeException { };

class Tables
{
	std::map<std::wstring, std::wstring> awsTypes;

public:
	Tables()
	{
		awsTypes[L"string"] = L"String";
		awsTypes[L"number"] = L"Number";
	}

	std::wstring convertToAwsType(std::wstring type)
	{
		if (awsTypes.find(type) == awsTypes.end())
		{
			return type;
		}

		return awsTypes[type];
	}
};


void checkParameters(
	std::wstring stackName,
	std::map<std::wstring, std::shared_ptr<Expression>> args, 
	std::map<std::wstring, ParameterPtr> params)
{
	// remove params that have been argumented
	for (auto argkvpair : args)
	{
		if (params.find(argkvpair.first) != params.end())
		{
			params.erase(argkvpair.first);
		}
		else
		{
			std::wcerr << L"Stack " << stackName << " does not have parameter named " << argkvpair.first << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	if (params.size() > 0)
	{
		std::wcerr << L"Not enough arguments to stack " << stackName << std::endl;
		exit(EXIT_FAILURE);
	}
}

void addStack(
	std::wstring suffix, 
	StackPtr stack, 
	std::map< std::wstring, Info<ParameterPtr> >& parameters,
	std::map< std::wstring, Info<ResourcePtr> >& resources,
	std::map< std::wstring, Info<ExpressionPtr> >& outputs,
	std::map< std::wstring, StackPtr >& stackMap,
	std::map<std::wstring, picojson::value>& parameterList,
	std::map<std::wstring, picojson::value>& resourceList,
	std::map<std::wstring, picojson::value>& outputList,
	const std::shared_ptr<Tables>& tables,
	Substitution& subs
	)
{

	for (auto& kv : stack->getParameters())
	{
		std::wstring keyname = stack->getName() + L"." + kv.first;
		Info<ParameterPtr> paramInfo = {kv.first, stack, kv.second};
		parameters[keyname] = paramInfo;
	}

	for (auto& res : stack->getResources())
	{
		// Two cases: one is an AWS Resource, another is a stack
		if ( stackMap.find(res->getType()) != stackMap.end() )
		{
			Substitution newSubs = subs;

			StackPtr calledStack = stackMap[res->getType()];

			// Perform parameter-checking
			checkParameters(calledStack->getName(), res->getProperties(), calledStack->getParameters());

			for (auto& propkv : res->getProperties())
			{
				newSubs.Add(propkv.first, propkv.second->asJson(subs));
			}

			std::map< std::wstring, Info<ParameterPtr> > parameters;
			std::map< std::wstring, Info<ResourcePtr> > resources;
			std::map< std::wstring, Info<ExpressionPtr> > outputs;

			std::map<std::wstring, picojson::value> parameterList;
			std::map<std::wstring, picojson::value> outputList;

			std::wstring newSuffix = L"In" + res->getName() + suffix;

			addStack(
				newSuffix,
				stackMap[res->getType()], 
				parameters, 
				resources, 
				outputs, 

				stackMap,

				parameterList,
				resourceList,
				outputList,

				tables,
				newSubs
				);

			for (auto& outputkv : outputs)
			{
				std::wstring outputRef = res->getName() + L"." + outputkv.second.name;
				subs.Add(outputRef, outputkv.second.item->asJson(newSubs));
			}
		}
		else
		{
			std::wstring keyname = res->getName() + suffix;

			Info<ResourcePtr> resInfo = {res->getName(), stack, res};
			resources[keyname] = resInfo;
		}
	}

	for (auto& kv : stack->getOutputs())
	{
		std::wstring keyname = stack->getName() + L"." + kv.first;
		Info<ExpressionPtr> outputInfo = {kv.first, stack, kv.second};
		outputs[keyname] = outputInfo;
	}

	for (auto& kv : parameters)
	{
		std::map<std::wstring, picojson::value> parmInfo;

		parmInfo[L"Type"] = picojson::value(tables->convertToAwsType(kv.second.item->getType()));
		parmInfo[L"Description"] = picojson::value(kv.first);

		parameterList[kv.second.name] = picojson::value(parmInfo);
	}


	for (auto& kv : resources)
	{
		std::map<std::wstring, picojson::value> resourceObject;

		resourceObject[L"Type"] = picojson::value(tables->convertToAwsType(kv.second.item->getType()));

		std::map<std::wstring, picojson::value> propertiesObject;
		for (auto& propkv : kv.second.item->getProperties()) {

			propertiesObject[propkv.first] = propkv.second->asJson(subs);
		}
		resourceObject[L"Properties"] = picojson::value(propertiesObject);

		resourceObject[L"Description"] = picojson::value(kv.second.name);

		resourceList[kv.first] = picojson::value(resourceObject);

		subs.Add(kv.second.name, SymbolReference(kv.first).asJson(subs) );
	}

	for (auto& kv : outputs)
	{
		std::map<std::wstring, picojson::value> outputObject;

		outputObject[L"Description"] = picojson::value(kv.first);
		outputObject[L"Value"] = kv.second.item->asJson(subs);

		outputList[kv.second.name] = picojson::value(outputObject);
	}

}

// returns outputs

picojson::value parse(std::string filename, std::wstring stackName)
{
	std::wstring wFilename(filename.begin(), filename.end());
	std::shared_ptr<Scanner> s(new Scanner(wFilename.c_str()));
	Parser p(s.get());

	p.Parse();

	std::map<std::wstring, picojson::value> theTemplate;

	replace(p.description, L"\n---\n", L"");
	replace(p.description, L"\r\n---\r\n", L"");
	replace(p.description, L"---\n", L"");
	replace(p.description, L"---\r\n", L"");

	theTemplate[L"AWSTemplateFormatVersion"] = picojson::value(L"2010-09-09");
	theTemplate[L"Description"] = picojson::value(p.description);

	std::shared_ptr<Tables> tables(new Tables());

	if (p.stacks.size() == 1)
	{
		stackName = p.stacks[0]->getName();
	}

	if (p.stacks.size() == 0)
	{
			std::cerr << "No stacks!" << std::endl;
			exit(EXIT_FAILURE);
	}
	else if (p.stacks.size() > 1)
	{
		if (stackName.compare(L"") == 0)
		{
			std::cerr << "You need to declare the main stack if you have more than 1 stack in the cfl" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	std::map< std::wstring, StackPtr > stackMap;
	std::map< std::wstring, VariablePtr > variableMap;

	std::map< std::wstring, Info<ParameterPtr> > parameters;
	std::map< std::wstring, Info<ExpressionPtr> > outputs;
	std::map< std::wstring, Info<ResourcePtr> > resources;

	Substitution subs;


	for (const StackPtr stack : p.stacks)
	{
		stackMap[stack->getName()] = stack;
	}

	for (const VariablePtr var : p.variables)
	{
		variableMap[var->getName()] = var;
	}

	if (stackMap.find(stackName) == stackMap.end())
	{
		std::wcerr << "Stack " << stackName << " not defined in cfl" << std::endl;
		exit(EXIT_FAILURE);
	}

	for (auto kvpair : variableMap)
	{
		if ( auto expr = std::dynamic_pointer_cast< StringLiteral >( kvpair.second->getExpr() ) )
		{
			subs.Add(kvpair.first, expr->asJson(subs));
		}
	}

	std::map<std::wstring, picojson::value> parameterList;
	std::map<std::wstring, picojson::value> resourceList;
	std::map<std::wstring, picojson::value> outputList;

	addStack(L"", stackMap[stackName], 
		parameters, 
		resources, 
		outputs, 

		stackMap, 

		parameterList, 
		resourceList, 
		outputList,

		tables,
		subs);

	if (parameters.size() != 0)
	{
		theTemplate[L"Parameters"] = picojson::value(parameterList);
	}

	theTemplate[L"Resources"] = picojson::value(resourceList);

	if (outputs.size() != 0)
	{
		theTemplate[L"Outputs"] = picojson::value(outputList);
	}

	return picojson::value(theTemplate);

}

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

	picojson::value v = parse(argv[1], stackName);

	std::wcout << v.serialize(true) << std::endl;

	return EXIT_SUCCESS;
}
