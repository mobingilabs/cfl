#ifndef MAPPINGCALL_H
#define MAPPINGCALL_H

#include <memory>
#include <vector>

#include "Expression.h"

class MappingCall : public Expression
{
	std::wstring record;
	std::shared_ptr<Expression> expr1, expr2;
	
public:
	MappingCall(std::wstring record, std::shared_ptr<Expression> expr1, std::shared_ptr<Expression> expr2) 
	: record(record), expr1(expr1), expr2(expr2)
	{

	}

	virtual picojson::value asJson(const Substitution& subs, bool forConditionSection) const 
	{

		/*
		"Fn::FindInMap" : [ "Region2AZ", { "Ref" : "AWS::Region" }, "AZ" ]
		*/
		
		if (subs.HasMapping(record)) 
		{

			std::map<std::wstring, picojson::value> obj;

			std::vector<picojson::value> arr;

			arr.push_back(picojson::value(record));
			arr.push_back(expr1->asJson(subs, forConditionSection));
			arr.push_back(expr2->asJson(subs, forConditionSection));

			obj[L"Fn::FindInMap"] = picojson::value(arr);

			return picojson::value(obj);
		}

		std::wcerr << "No such mapping: " << record << std::endl;
		exit(1);
	
	}

	virtual ExpressionForm getForm() const
	{
		return MEMBER_CALL;
	}

	virtual std::wstring getType(const Substitution& subs) const
	{
		return L"string";
	}
};

#endif // MAPPINGCALL_H
