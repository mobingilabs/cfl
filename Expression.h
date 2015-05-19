#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Substitution.h"
#include "picojson/picojson.h"

enum ExpressionForm
{
	NUMBER_LITERAL,
	STRING_LITERAL,
	BOOLEAN_LITERAL,
	ARRAY_LITERAL,
	RECORD_LITERAL,
	MEMBER_CALL,
	FUNCTION_CALL,
	SYMBOL_REFERENCE,
	OPERATION_LIST,
	TERNARY_EXPRESSION
};

class Expression
{
public:
	virtual picojson::value asJson(std::shared_ptr<Substitution> subs, bool forConditionSection = false) const = 0;
	virtual ExpressionForm getForm() const = 0;
    virtual std::wstring getType(std::shared_ptr<Substitution> subs) const = 0;
    virtual bool IsLiteral() const
    {
    	if (getForm() == NUMBER_LITERAL ||
			getForm() == STRING_LITERAL ||
			getForm() == BOOLEAN_LITERAL ||
			getForm() == ARRAY_LITERAL ||
			getForm() == RECORD_LITERAL)
    	{
    		return true;
    	}
    	return false;
    }
};

#endif // EXPRESSION_H
