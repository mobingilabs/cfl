#ifndef CONDITIONSTABLE_H
#define CONDITIONSTABLE_H

#include <vector>
#include <sstream>
#include "picojson/picojson.h"

class ConditionsTable
{
	std::map<std::wstring, int> condToIdentifier;
	std::vector< picojson::value > table; 
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

};

#endif // CONDITIONSTABLE_H