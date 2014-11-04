#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include <memory>

class Parameter
{
	std::wstring name;
	std::wstring type;
	std::shared_ptr<Expression> expr;

public:
	Parameter(std::wstring name, std::wstring type) : name(name), type(type)
	{

	}

	std::wstring getName() const
	{
		return name;
	}

	std::wstring getType() const
	{
		return type;
	}

	std::shared_ptr<Expression> getDefault() const
	{
		return expr;
	}

	void setDefault(std::shared_ptr<Expression> expr)
	{
		this->expr = expr;
	}

	bool hasDefault() const
	{
		return !!expr;
	}
};

#endif // PARAMETER_H
