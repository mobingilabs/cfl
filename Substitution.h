#ifndef SUBSTITUTION_H
#define SUBSTITUTION_H

#include <iostream>
#include "picojson/picojson.h"

class Substitution
{
	std::map<std::wstring, picojson::value> subs;
	std::map<std::wstring, picojson::value> condSubs;
	std::map<std::wstring, std::wstring> mappings;

	std::map<std::wstring, std::wstring> symbolTypes;

	class NoSubstitute {};
	class NotDefined {};

public:
	Substitution()
	{

	}

	void Add(std::wstring str, picojson::value json, picojson::value jsonForCondition)
	{
		subs[str] = json;
		condSubs[str] = jsonForCondition;
	}

	picojson::value Substitute(std::wstring str, bool forCondition) const
	{
		if (HasSubstitute(str))
		{
			if (forCondition)
			{
				return condSubs.find(str)->second;
			}
			return subs.find(str)->second;
		}

		throw NoSubstitute();
	}

	bool HasSubstitute(std::wstring str) const
	{
		return subs.find(str) != subs.end();
	}

	void AddMapping(std::wstring str)
	{
		mappings[str] = str;
	}

	bool HasMapping(std::wstring str) const
	{
		return mappings.find(str) != mappings.end();
	}

	void AddTypeMapping(std::wstring str, std::wstring type)
	{
		//std::wcerr << "typed " << str << " as " << type << std::endl;
		symbolTypes[str] = type;
	}

	bool HasTypeMapping(std::wstring str) const
	{
		return symbolTypes.find(str) != symbolTypes.end();
	}

	std::wstring GetType(std::wstring str) const
	{
		if (HasTypeMapping(str))
		{
			return symbolTypes.find(str)->second;
		}

		throw NotDefined();
	}
};

#endif // SUBSTITUTION_H
