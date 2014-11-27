#ifndef NUMBERLITERAL_H
#define NUMBERLITERAL_H

#include "Expression.h"

class NumberLiteral : public Expression
{
	double number;

public:
	NumberLiteral(double number) : number(number)
	{
	}

	virtual picojson::value asJson(const Substitution& subs) const 
	{
		return picojson::value(number);
	}

	virtual ExpressionForm getForm() const
	{
		return NUMBER_LITERAL;
	}

	virtual std::wstring getType(const Substitution& subs) const
	{
		return L"number";
	}
};

#endif // NUMBERLITERAL_H