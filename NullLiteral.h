#ifndef NULLLITERAL_H
#define NULLLITERAL_H

#include <string>
#include "Expression.h"


class NullLiteral : public Expression
{
public:

	NullLiteral()
	{

	}

	virtual picojson::value asJson(std::shared_ptr<Substitution> subs, bool forConditionSection) const 
	{
		return picojson::value();
	}

	virtual ExpressionForm getForm() const
	{
		return NULL_LITERAL;
	}

	virtual std::wstring getType(std::shared_ptr<Substitution> subs) const
	{
		return L"null";
	}
};

#endif // NULLLITERAL_H
