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

	virtual picojson::value asJson(const Substitution& subs) const 
	{
		return picojson::value(val);
	}

	bool getContent() const
	{
		return val;
	}
};

#endif // BOOLEANLITERAL_H