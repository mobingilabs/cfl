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
	virtual picojson::value asJson(const Substitution& subs, bool forConditionSection = false) const = 0;
	virtual ExpressionForm getForm() const = 0;
	virtual std::wstring getType(const Substitution& subs) const = 0;
};

#endif // EXPRESSION_H
