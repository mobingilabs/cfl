#ifndef FUNCTIONCALL_H
#define FUNCTIONCALL_H

#include <memory>
#include <vector>

#include "Expression.h"

class FunctionCall : public Expression
{
	std::wstring funcName;
	std::vector< std::shared_ptr<Expression> > params;
	
public:
	FunctionCall(std::wstring funcName, std::vector< std::shared_ptr<Expression> > params) : funcName(funcName), params(params)
	{

	}

	virtual picojson::value asJson(const Substitution& subs) const 
	{
		std::map<std::wstring, picojson::value> obj;

		if (params.size() > 1)
		{
			std::vector<picojson::value> parmArr;
			for (const std::shared_ptr<Expression>& p : params)
			{
				parmArr.push_back(p->asJson(subs));
			}
			obj[funcName] = picojson::value(parmArr);
		}
		else
		{
			obj[funcName] = picojson::value((*params.begin())->asJson(subs));
		}

		return picojson::value(obj);
	}
};

#endif // MEMBERCALL_H
