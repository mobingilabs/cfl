#ifndef ARRAYLITERAL_H
#define ARRAYLITERAL_H

#include <memory>
#include <vector>

#include "Expression.h"


class ArrayLiteral : public Expression
{
	std::vector<std::shared_ptr<Expression>> elements;
public:
	ArrayLiteral()
	{

	}

	void Add (std::shared_ptr<Expression> element)
	{
		elements.push_back(element);
	}

	virtual picojson::value asJson(const Substitution& subs) const 
	{
		std::vector<picojson::value> arr;

		for (const std::shared_ptr<Expression>& e : elements)
		{
			arr.push_back(e->asJson(subs));
		}

		return picojson::value(arr);
	}
};

#endif // ARRAYLITERAL_H