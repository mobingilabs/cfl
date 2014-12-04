#ifndef CFL_H
#define CFL_H

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include <algorithm>
#include <typeinfo>
#include "picojson/picojson.h"

#include "Parser.h"
#include "Scanner.h"
#include "LaunchData.h"
#include "BuiltInStacks.h"

class CFL
{
	BuiltInStacks stacks;

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

	std::wstring zeroPad(int num, int max)
	{
		std::wstringstream maxString;
		maxString << max;

		std::wstringstream numString;
		numString << num;

		std::wstringstream ss;

		for (int i = 0; i < maxString.str().length() - numString.str().length(); i++)
		{
			ss << L"0";
		}
		ss << num;
		return ss.str();
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
		std::map<std::wstring, ParameterPtr> params,
		const Substitution& subs)
	{
		// remove params that have been argumented
		for (auto argkvpair : args)
		{
			std::wstring paramName = argkvpair.first;
			std::shared_ptr<Expression> expression = argkvpair.second;

			if (params.find(paramName) != params.end())
			{
				// check type
				if (expression->getType(subs).compare(params[paramName]->getType()) != 0)
				{
					std::wcerr << "Argument to " << paramName << " has the wrong type. Should be: '" << params[paramName]->getType() << "' but is '" << expression->getType(subs) << "' instead" << std::endl;
					exit(EXIT_FAILURE);
				}

				params.erase(paramName);
			}
			else
			{
				std::wcerr << L"Stack " << stackName << " does not have parameter named " << argkvpair.first << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		// remove params that have defaults
		for(auto iter = params.begin(); iter != params.end(); ) {
			if (iter->second->hasDefault()) {
				iter = params.erase(iter);
			} else {
				++iter;
			}
		}

		// give errors if the params are bad
		if (params.size() > 0)
		{
			std::wcerr << L"Not enough arguments to stack " << stackName << std::endl;

			for (auto param : params)
			{
				std::wcerr << param.second->getName() << " not defined" << std::endl;
			}
			
			exit(EXIT_FAILURE);
		}
	}

	void applyMetadata(
		const std::vector< MetadataPtr >& metadata,
		std::map< std::wstring, MetadataPtr >& dependencies,
		LaunchData& launchData,
		Substitution& subs)
	{
		for (auto& meta : metadata)
		{
			if (meta->getKey().compare(L"DependsOn") == 0 && meta->getExpression()->getForm() == SYMBOL_REFERENCE)
			{
				std::wstring key = meta->getExpression()->asJson(subs).get<picojson::value::object>()[L"Ref"].get<std::wstring>();
				dependencies[key] = meta;
			}

			if (meta->getKey().compare(L"Yum") == 0 && meta->getExpression()->getForm() == STRING_LITERAL)
			{
				StringLiteral* lit = (StringLiteral*)meta->getExpression().get();
				launchData.addYum(lit->getContent());
			}

			if (meta->getKey().compare(L"Gem") == 0 && meta->getExpression()->getForm() == STRING_LITERAL)
			{
				StringLiteral* lit = (StringLiteral*)meta->getExpression().get();
				launchData.addGem(lit->getContent());
			}

			if (meta->getKey().compare(L"RPM") == 0 && meta->getExpression()->getForm() == STRING_LITERAL)
			{
				StringLiteral* lit = (StringLiteral*)meta->getExpression().get();
				launchData.addRPM(lit->getContent());
			}


			if (meta->getKey().compare(L"Service") == 0 && meta->getExpression()->getForm() == STRING_LITERAL)
			{
				StringLiteral* lit = (StringLiteral*)meta->getExpression().get();
				Service service;
				service.name = lit->getContent();
				launchData.addService(service);
			}

			if (meta->getKey().compare(L"Command") == 0 && meta->getExpression()->getForm() == STRING_LITERAL)
			{
				StringLiteral* lit = (StringLiteral*)meta->getExpression().get();
				Command command;
				command.command = lit->getContent();

				auto info = meta->getInfo();

				if (info.find(L"CWD") != info.end() && info[L"CWD"]->getForm() == STRING_LITERAL )
				{
					StringLiteral* lit = (StringLiteral*)info[L"CWD"].get();
					command.cwd = lit->getContent();
				}

				launchData.addCommand(command);

			}

			if (meta->getKey().compare(L"File") == 0 && meta->getExpression()->getForm() == STRING_LITERAL)
			{
				StringLiteral* lit = (StringLiteral*)meta->getExpression().get();

				File file;
				file.path = lit->getContent();

				auto info = meta->getInfo();

				if (info.find(L"Content") != info.end())
				{
					if (info[L"Content"]->getForm() == STRING_LITERAL )
					{
						StringLiteral* lit = (StringLiteral*)info[L"Content"].get();
						file.content = lit->getContent();
					}
					else if (info[L"Content"]->getForm() == FUNCTION_CALL )
					{

						file.content = info[L"Content"]->asJson(subs).get<std::wstring>();
					}
				}

				if (info.find(L"Owner") != info.end())
				{
					if (info[L"Owner"]->getForm() == STRING_LITERAL )
					{
						StringLiteral* lit = (StringLiteral*)info[L"Owner"].get();
						file.owner = lit->getContent();
					}
				}

				if (info.find(L"Group") != info.end())
				{
					if (info[L"Group"]->getForm() == STRING_LITERAL )
					{
						StringLiteral* lit = (StringLiteral*)info[L"Group"].get();
						file.group = lit->getContent();
					}
				}

				if (info.find(L"Mode") != info.end())
				{
					if (info[L"Mode"]->getForm() == STRING_LITERAL )
					{
						StringLiteral* lit = (StringLiteral*)info[L"Mode"].get();
						file.mode = lit->getContent();
					}
				}

				if (info.find(L"Context") != info.end())
				{
					file.context = info[L"Context"]->asJson(subs);
				}

				launchData.addFile(file);
			}
		}
	}

	void addStack(
		std::wstring suffix, 
		StackPtr stack,
		std::vector< MetadataPtr > metadata,
		std::map< std::wstring, Info<ParameterPtr> >& parameters,
		std::map< std::wstring, Info<ResourcePtr> >& resources,
		std::map< std::wstring, Info<ExpressionPtr> >& outputs,
		std::map< std::wstring, StackPtr >& stackMap,
		std::map<std::wstring, picojson::value>& parameterList,
		std::map<std::wstring, picojson::value>& resourceList,
		std::map<std::wstring, picojson::value>& outputList,
		const std::shared_ptr<Tables>& tables,
		Substitution& subs,
		ConditionsTablePtr ctable,
		std::stack< std::shared_ptr<Expression> > outerConditions
		)
	{

		for (auto& kv : stack->getParameters())
		{
			std::wstring keyname = stack->getName() + L"." + kv.first;
			Info<ParameterPtr> paramInfo = {kv.first, stack, kv.second};
			parameters[keyname] = paramInfo;

			subs.AddTypeMapping(kv.first, kv.second->getType());
		}

		for (auto& res : stack->getResources())
		{
			std::wstring awsPrefix(L"AWS::");

			subs.Add(res->getName(), SymbolReference(res->getName() + suffix).asJson(subs, false) );

			// Two cases: one is an AWS Resource, another is a stack
			if ( stackMap.find(res->getType()) != stackMap.end() )
			{
				Substitution newSubs = subs;

				StackPtr calledStack = stackMap[res->getType()];

				// Perform parameter-checking
				checkParameters(calledStack->getName(), res->getProperties(), calledStack->getParameters(), subs);

				// Apply defaults
				for (auto& paramkv : calledStack->getParameters())
				{
					if (paramkv.second->hasDefault())
					{
						newSubs.Add(paramkv.second->getName(), paramkv.second->getDefault()->asJson(subs));
					}

				}

				// Apply params
				for (auto& propkv : res->getProperties())
				{
					newSubs.Add(propkv.first, propkv.second->asJson(subs));
				}

				std::map< std::wstring, Info<ParameterPtr> > parameters;
				std::map< std::wstring, Info<ResourcePtr> > resources;
				std::map< std::wstring, Info<ExpressionPtr> > outputs;

				std::vector< MetadataPtr > newMetadata = metadata;

				// Apply meta to all inner stacks
				for (auto& meta : res->getMetadata())
				{
					newMetadata.push_back(meta);
				}

				std::map<std::wstring, picojson::value> parameterList;
				std::map<std::wstring, picojson::value> outputList;

				std::wstring newSuffix = L"In" + res->getName() + suffix;

				std::stack< std::shared_ptr<Expression> > conditions = res->getConditions();

				std::stack< std::shared_ptr<Expression> > newOuterConditions = outerConditions;
				
				if (conditions.size() > 0)
				{
					std::shared_ptr<OperationList> ol(new OperationList(ctable, conditions.top()));
					conditions.pop();

					while (conditions.size())
					{
						ol->AddExpression(L"and", conditions.top());
						conditions.pop();
					}

					newOuterConditions.push(ol);
				}

				addStack(
					newSuffix,
					stackMap[res->getType()], 
					newMetadata,
					parameters, 
					resources, 
					outputs, 

					stackMap,

					parameterList,
					resourceList,
					outputList,

					tables,
					newSubs,
					ctable,
					newOuterConditions
					);

				for (auto& outputkv : outputs)
				{
					std::wstring outputRef = res->getName() + L"." + outputkv.second.name;
					subs.Add(outputRef, outputkv.second.item->asJson(newSubs));
					subs.AddTypeMapping(outputRef, outputkv.second.item->getType(newSubs));
				}
			}
			else if (res->getType().compare(0, awsPrefix.size(), awsPrefix) == 0)
			{

				std::wstring keyname = res->getName() + suffix;

				Info<ResourcePtr> resInfo = {res->getName(), stack, res};
				resources[keyname] = resInfo;

				subs.AddTypeMapping(res->getName(), L"string");
			}
			else
			{
				std::wcerr << L"No such resource: " << res->getType() << std::endl;
				exit(EXIT_FAILURE);
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

			if (kv.second.item->hasDefault())
			{
				parmInfo[L"Default"] = kv.second.item->getDefault()->asJson(subs);
			}

			parameterList[kv.second.name] = picojson::value(parmInfo);
		}

		for (auto& kv : resources)
		{
			std::stack< std::shared_ptr<Expression> > conditions = kv.second.item->getConditions();
			
			std::map< std::wstring, MetadataPtr > dependencies;
			LaunchData launchData;

			// Apply meta
			applyMetadata(kv.second.item->getMetadata(), dependencies, launchData, subs);
			applyMetadata(metadata, dependencies, launchData, subs);

			// Build object
			std::map<std::wstring, picojson::value> resourceObject;

			resourceObject[L"Type"] = picojson::value(tables->convertToAwsType(kv.second.item->getType()));

			std::map<std::wstring, picojson::value> propertiesObject;
			for (auto& propkv : kv.second.item->getProperties())
			{
				propertiesObject[propkv.first] = propkv.second->asJson(subs);
			}
			resourceObject[L"Properties"] = picojson::value(propertiesObject);

			if (outerConditions.size() + conditions.size() > 0)
			{
				std::stack< std::shared_ptr<Expression> > combinedConditions;

				auto outerConds = outerConditions;

				while (outerConds.size())
				{
					combinedConditions.push(outerConds.top());
					outerConds.pop();
				}

				while (conditions.size())
				{
					combinedConditions.push(conditions.top());
					conditions.pop();
				}

				OperationList ol(ctable, combinedConditions.top());
				combinedConditions.pop();

				while (combinedConditions.size())
				{
					ol.AddExpression(L"and", combinedConditions.top());
					combinedConditions.pop();
				}

				picojson::value condJson = ol.asJson(subs, true);
				picojson::object obj = condJson.get<picojson::object>();

				resourceObject[L"Condition"] = obj[L"Condition"];
			}

			resourceObject[L"Description"] = picojson::value(kv.second.name);

			if (dependencies.size() != 0)
			{
				std::vector< picojson::value > arr;
				for (auto dependencykv : dependencies) 
				{
					arr.push_back(picojson::value(dependencykv.first));
				}
				resourceObject[L"DependsOn"] = picojson::value(arr);
			}


			if (launchData.hasData())
			{
				std::map<std::wstring, picojson::value> metadataObject;
				std::map<std::wstring, picojson::value> init;
				std::map<std::wstring, picojson::value> config;

					std::map<std::wstring, picojson::value> packages;

					if (launchData.getYum().size() > 0)
					{
						std::map<std::wstring, picojson::value> yum;
						for (const std::wstring& package : launchData.getYum())
						{
							std::vector< picojson::value > arr;
							yum[package] = picojson::value(arr);
						}
						packages[L"yum"] = picojson::value(yum);
					}

					if (launchData.getGems().size() > 0)
					{
						std::map<std::wstring, picojson::value> gem;
						for (const std::wstring& package : launchData.getGems())
						{
							std::vector< picojson::value > arr;
							gem[package] = picojson::value(arr);
						}
						packages[L"rubygems"] = picojson::value(gem);
					}

					if (launchData.getRPMs().size() > 0)
					{
						std::map<std::wstring, picojson::value> rpm;
						for (const std::wstring& package : launchData.getRPMs())
						{
							std::vector< picojson::value > arr;
							rpm[package] = picojson::value(arr);
						}
						packages[L"rpm"] = picojson::value(rpm);
					}

					config[L"packages"] = picojson::value(packages);


					std::map<std::wstring, picojson::value> services;
					std::map<std::wstring, picojson::value> sysvinit;

					if (launchData.getServices().size() > 0)
					{
						for (const Service& servname : launchData.getServices())
						{
							std::map<std::wstring, picojson::value> sprops;
							sprops[L"enabled"] = picojson::value(true);
							sprops[L"ensureRunning"] = picojson::value(true);
							sysvinit[servname.name] = picojson::value(sprops);
						}
					}

					services[L"sysvinit"] = picojson::value(sysvinit);
					config[L"services"] = picojson::value(services);



					std::map<std::wstring, picojson::value> commands;

					auto commandList = launchData.getCommands();
					if (commandList.size() > 0)
					{
						for (size_t idx = 0; idx < commandList.size(); idx++)
						{
							std::map<std::wstring, picojson::value> cmd;
							cmd[L"command"] = picojson::value(commandList[idx].command);
							if (commandList[idx].cwd.size() != 0)
							{
								cmd[L"cwd"] = picojson::value(commandList[idx].cwd);
							}
							commands[L"cmd" + zeroPad(idx, commandList.size())] = picojson::value(cmd);
						}
					}

					config[L"commands"] = picojson::value(commands);



					std::map<std::wstring, picojson::value> files;

					auto fileList = launchData.getFiles();
					if (fileList.size() > 0)
					{
						for (size_t idx = 0; idx < fileList.size(); idx++)
						{
							std::map<std::wstring, picojson::value> fileobj;
							File file = fileList[idx];
							fileobj[L"content"] = picojson::value(file.content);
							if (file.owner.size() != 0)
							{
								fileobj[L"owner"] = picojson::value(file.owner);
								if (file.context.is<picojson::value::object>())
								{
									fileobj[L"context"] = file.context;
								}
							}
							if (file.group.size() != 0)
							{
								fileobj[L"group"] = picojson::value(file.group);
							}
							if (file.mode.size() != 0)
							{
								fileobj[L"mode"] = picojson::value(file.mode);
							}
							files[file.path] = picojson::value(fileobj);
						}
					}

					config[L"files"] = picojson::value(files);



				init[L"config"] = picojson::value(config);
				metadataObject[L"AWS::CloudFormation::Init"] = picojson::value(init);
				resourceObject[L"Metadata"] = picojson::value(metadataObject);
			}

			resourceList[kv.first] = picojson::value(resourceObject);


			subs.Add(kv.second.name, SymbolReference(kv.first).asJson(subs, false) );
		}

		for (auto& kv : outputs)
		{
			std::map<std::wstring, picojson::value> outputObject;

			outputObject[L"Description"] = picojson::value(kv.first);
			outputObject[L"Value"] = kv.second.item->asJson(subs);

			outputList[kv.second.name] = picojson::value(outputObject);
		}

	}

	std::string getDirFromFilename(std::wstring filename)
	{
		// get current path
		std::string mbFilename(filename.begin(), filename.end());
		char result[PATH_MAX + 1];
		std::string path = realpath(mbFilename.c_str(), result);
		std::string dir = path.substr(0, path.find_last_of("/"));

		return dir;
	}

	void importFile(
		std::wstring wFilename, 
		std::map< std::wstring, StackPtr >& stackMap,
		std::map< std::wstring, VariablePtr >& variableMap,
		std::map< std::wstring, MappingPtr >& mappingMap,
		ConditionsTablePtr ctable
		)
	{

		std::string filename(wFilename.begin(), wFilename.end());

		std::ifstream ifs(filename);
		std::string content( (std::istreambuf_iterator<char>(ifs)) , (std::istreambuf_iterator<char>()) );

		import(wFilename, content, stackMap, variableMap, mappingMap, ctable);
	}

	void import(
		std::wstring filename,
		std::string cfl, 
		std::map< std::wstring, StackPtr >& stackMap,
		std::map< std::wstring, VariablePtr >& variableMap,
		std::map< std::wstring, MappingPtr >& mappingMap,
		ConditionsTablePtr ctable
		)
	{
		std::shared_ptr<Scanner> s(new Scanner((const unsigned char *)cfl.c_str(), cfl.length()));
		Parser p(s);
		p.ctable = ctable;
		p.Parse();

		for (const StringLiteralPtr str : p.imports)
		{
			chdir(getDirFromFilename(filename).c_str());
			importFile(str->getContent(), stackMap, variableMap, mappingMap, ctable);
		}

		for (const StringLiteralPtr str : p.absoluteImports)
		{
			import(L"", stacks.getStack(str->getContent()), stackMap, variableMap, mappingMap, ctable);
		}

		for (const StackPtr stack : p.stacks)
		{
			stackMap[stack->getName()] = stack;
		}

		for (const VariablePtr var : p.variables)
		{
			variableMap[var->getName()] = var;
		}

		for (const MappingPtr mapping : p.mappings)
		{
			mappingMap[mapping->getName()] = mapping;
		}
	}

public:

	// returns outputs
	picojson::value parse(std::string filename, std::wstring stackName)
	{
		std::map< std::wstring, StackPtr > stackMap;
		std::map< std::wstring, VariablePtr > variableMap;
		std::map< std::wstring, MappingPtr > mappingMap;

		std::map< std::wstring, Info<ParameterPtr> > parameters;
		std::map< std::wstring, Info<ExpressionPtr> > outputs;
		std::map< std::wstring, Info<ResourcePtr> > resources;

		Substitution subs;
		ConditionsTablePtr ctable(new ConditionsTable());

		std::wstring wFilename(filename.begin(), filename.end());
		std::shared_ptr<Scanner> s(new Scanner(wFilename.c_str()));
		Parser p(s);
		p.ctable = ctable;
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

		importFile(wFilename, stackMap, variableMap, mappingMap, ctable);

		if (stackMap.find(stackName) == stackMap.end())
		{
			std::wcerr << "Stack " << stackName << " not defined." << std::endl;
			exit(EXIT_FAILURE);
		}

		for (auto kvpair : variableMap)
		{
			subs.Add(kvpair.first, kvpair.second->getExpr()->asJson(subs));
			subs.AddTypeMapping(kvpair.first, kvpair.second->getExpr()->getType(subs));
		}

		if (mappingMap.size() != 0)
		{
			std::map<std::wstring, picojson::value> mappingList;
			for (auto kvpair : mappingMap)
			{
				mappingList[kvpair.first] = kvpair.second->asJson();
				subs.AddMapping(kvpair.first);
			}
			theTemplate[L"Mappings"] = picojson::value(mappingList);
		}


		std::map<std::wstring, picojson::value> parameterList;
		std::map<std::wstring, picojson::value> resourceList;
		std::map<std::wstring, picojson::value> outputList;
		std::map<std::wstring, picojson::value> conditionList;
		std::vector< MetadataPtr > metadata;
		std::stack< std::shared_ptr<Expression> > outerConditions; 

		addStack(L"", stackMap[stackName], 
			metadata,
			parameters, 
			resources, 
			outputs, 

			stackMap, 

			parameterList, 
			resourceList, 
			outputList,

			tables,
			subs,
			ctable,
			outerConditions
			);

		auto conditions = ctable->GetConditions();
		for (int i=0; i<conditions.size(); i++)
		{
			conditionList[ctable->GetConditionName(i)] = conditions[i];
		}

		if (conditionList.size() != 0)
		{
			theTemplate[L"Conditions"] = picojson::value(conditionList);
		}

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

	CFL()
	{

	}

};

#endif // CFL_H
