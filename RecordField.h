#ifndef RECORDFIELD_H
#define RECORDFIELD_H

#include <memory>

#include "Expression.h"

class RecordField
{
	std::wstring key;
	std::shared_ptr<Expression> value;

public:
	RecordField(std::wstring key, std::shared_ptr<Expression> value) : key(key), value(value)
	{

	}

	std::wstring getKey() const
	{
		return key;
	}

	std::shared_ptr<Expression> getValue() const
	{
		return value;
	}
};

#endif // RECORDFIELD_H
