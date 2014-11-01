#ifndef VARIABLE_H
#define VARIABLE_H

#include <map>
#include <memory>
#include <string>
#include "Expression.h"

class Variable
{
	std::wstring name;
	std::shared_ptr<Expression> expr;

public:
	Variable(std::wstring name, std::shared_ptr<Expression> expr) : name(name), expr(expr)
	{

	}

	std::wstring getName() const
	{
		return name;
	}

	std::shared_ptr<Expression> getExpr() const
	{
		return expr;
	}
};

#endif // VARIABLE_H
