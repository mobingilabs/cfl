#ifndef CONDITIONSTABLE_H
#define CONDITIONSTABLE_H

#include <vector>
#include <sstream>
#include "picojson/picojson.h"

class ConditionsTable
{
	std::map<std::wstring, int> condToIdentifier;
	std::vector< picojson::value > table; 

	static std::wstring replace(std::wstring str, const std::wstring& from, const std::wstring& to) {
		size_t start_pos = str.find(from);
		if(start_pos == std::wstring::npos) { return str; }
		str.replace(start_pos, from.length(), to);
		return str;
	}

public:
	ConditionsTable()
	{
	}

	int AddCondition(picojson::value cond)
	{
		std::wstring key = cond.serialize();
		if (condToIdentifier.find(key) == condToIdentifier.end())
		{
			table.push_back(cond);
			condToIdentifier[key] = table.size() - 1;
		}
		return condToIdentifier[key];
	}

	std::vector<picojson::value> GetConditions() const
	{
		return table;
	}

	std::wstring GetConditionName(int cond) const
	{
		std::wstringstream ss;
		ss << "Condition" << cond;
		return ss.str();
	}

	picojson::value GetConditionByName(std::wstring name) const
	{
		std::wstring str = replace(name, L"Condition", L"");
		int num = std::stoi(str);
		return table[num];
	}

};

#endif // CONDITIONSTABLE_H