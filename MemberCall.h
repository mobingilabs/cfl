#ifndef MEMBERCALL_H
#define MEMBERCALL_H

#include <memory>
#include <vector>

#include "Expression.h"

class MemberCall : public Expression
{
	std::wstring record;
	std::wstring member;
	
public:
	MemberCall(std::wstring record, std::wstring member) : record(record), member(member)
	{

	}

	virtual picojson::value asJson(const Substitution& subs) const 
	{

		/*
		{
            "Fn::GetAtt":[
               "ElasticLoadBalancer",
               "DNSName"
            ]
         }*/

		std::wstring keyname = record + L"." + member;
		
		if (subs.HasSubstitute(keyname)) 
		{
			return subs.Substitute(keyname);
		}

		std::wstring resolvedName = record;

		if (subs.HasSubstitute(record))
		{
			picojson::value sub = subs.Substitute(record);
			if (sub.is<picojson::value::object>())
			{
				auto& obj = sub.get<picojson::value::object>();
				if (obj.find(L"Ref") != obj.end() && obj.find(L"Ref")->second.is<std::wstring>())
				{
					resolvedName = obj[L"Ref"].get<std::wstring>();
				}
			}
		}

		std::map<std::wstring, picojson::value> obj;

		std::vector<picojson::value> arr;

		arr.push_back(picojson::value(resolvedName));
		arr.push_back(picojson::value(member));

		obj[L"Fn::GetAtt"] = picojson::value(arr);

		return picojson::value(obj);
	}

	virtual ExpressionForm getForm() const
	{
		return MEMBER_CALL;
	}

	virtual std::wstring getType(const Substitution& subs) const
	{
		std::wstring keyname = record + L"." + member;

		if (subs.HasTypeMapping(keyname)) 
		{
			return subs.GetType(keyname);
		}

		std::wcerr << keyname << " is not defined" << std::endl;

		exit(EXIT_FAILURE);
	}
};

#endif // MEMBERCALL_H
