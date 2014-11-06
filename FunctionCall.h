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

		// TODO: gotta tidy this up into a table of intrinsics
		if (funcName.compare(L"In::RefName") == 0)
		{
			if (params.size() != 1)
			{
				std::wcerr << "In::RefName only takes one parameter: a variable which ultimately references a resource" << std::endl;
				exit(1);
			}

			if (params[0]->getForm() == SYMBOL_REFERENCE)
			{
				std::wstring str = params[0]->asJson(subs).get<picojson::value::object>()[L"Ref"].get<std::wstring>();
				return picojson::value(str);
			}

			std::wcerr << "In::RefName variable does not reference a resource!" << std::endl;
			exit(1);
		}

		// all other functions
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

	virtual ExpressionForm getForm() const
	{
		return FUNCTION_CALL;
	}
};

#endif // MEMBERCALL_H
