#ifndef SYMBOLREFERENCE_H
#define SYMBOLREFERENCE_H

#include <memory>
#include <vector>
#include <iostream>

#include "Expression.h"

class SymbolReference : public Expression
{
	std::wstring symbolName;
public:
	SymbolReference(std::wstring symbolName) : symbolName(symbolName)
	{

	}

	std::wstring getSymbolName() const
	{
		return symbolName;
	}
	
	virtual picojson::value asJson(const Substitution& subs) const 
	{
		if (subs.HasSubstitute(symbolName))
		{
			return subs.Substitute(symbolName);
		}
		
		std::map<std::wstring, picojson::value> obj;

		obj[L"Ref"] = picojson::value(symbolName);

		return picojson::value(obj);
	}

	virtual ExpressionForm getForm() const
	{
		return SYMBOL_REFERENCE;
	}
};

#endif // SYMBOLREFERENCE_H
