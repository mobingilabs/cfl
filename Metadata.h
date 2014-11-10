#ifndef METADATA_H
#define METADATA_H

#include <string>
#include <memory>
#include "Expression.h"

class Metadata
{
	std::wstring key;
	std::shared_ptr<Expression> expr;
	std::map<std::wstring, std::shared_ptr<Expression>> info;

public:

	Metadata(std::wstring key, std::shared_ptr<Expression> expr) : key(key), expr(expr)
	{

	}

	std::wstring getKey() const
	{
		return key;
	}

	std::shared_ptr<Expression> getExpression() const
	{
		return expr;
	}

	std::map<std::wstring, std::shared_ptr<Expression>> getInfo() const
	{
		return info;
	}

	void addInfo(std::wstring key, std::shared_ptr<Expression> expr)
	{
		info[key] = expr;
	}
};

#endif // METADATA_H
