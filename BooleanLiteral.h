#ifndef BOOLEANLITERAL_H
#define BOOLEANLITERAL_H

#include "Expression.h"

class BooleanLiteral : public Expression
{
	bool val;

public:

	BooleanLiteral(bool val) : val(val)
	{

	}

	virtual picojson::value asJson(const Substitution& subs, bool forConditionSection) const 
	{
		return picojson::value(val);
	}

	bool getContent() const
	{
		return val;
	}

	virtual ExpressionForm getForm() const
	{
		return BOOLEAN_LITERAL;
	}

	virtual std::wstring getType(const Substitution& subs) const 
	{
		return L"boolean";
	}
};

#endif // BOOLEANLITERAL_H