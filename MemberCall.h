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

		std::map<std::wstring, picojson::value> obj;

		std::vector<picojson::value> arr;

		arr.push_back(picojson::value(record));
		arr.push_back(picojson::value(member));

		obj[L"Fn::GetAtt"] = picojson::value(arr);

		return picojson::value(obj);
	}
};

#endif // MEMBERCALL_H
