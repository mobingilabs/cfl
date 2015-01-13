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

	const int TRUE_VALUE = -1;
	const int FALSE_VALUE = -2;

public:
	ConditionsTable()
	{
	}

	int AddCondition(picojson::value cond)
	{
		if (cond.is<bool>())
		{
			return cond.get<bool>() ? TRUE_VALUE : FALSE_VALUE;
		}

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
		if (cond == TRUE_VALUE)
		{
			return L"true";
		}

		if (cond == FALSE_VALUE)
		{
			return L"false";
		}

		std::wstringstream ss;
		ss << "Condition" << cond;
		return ss.str();
	}

	picojson::value GetConditionByNum(int cond) const
	{
		if (cond == TRUE_VALUE)
		{
			return picojson::value(true);
		}

		if (cond == FALSE_VALUE)
		{
			return picojson::value(false);
		}

		return table[cond];
	}

	picojson::value GetConditionByName(std::wstring name) const
	{
		if (name.compare(L"true") == 0)
		{
			return picojson::value(true);
		}

		if (name.compare(L"false") == 0)
		{
			return picojson::value(false);
		}
		
		std::wstring str = replace(name, L"Condition", L"");
		int num = std::stoi(str);
		return table[num];
	}

};

#endif // CONDITIONSTABLE_H