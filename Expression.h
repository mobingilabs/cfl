#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Substitution.h"
#include "picojson/picojson.h"

class Expression
{
public:
	virtual picojson::value asJson(const Substitution& subs) const = 0;
};

#endif // EXPRESSION_H
