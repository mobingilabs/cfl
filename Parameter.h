#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include <memory>

class Parameter
{
	std::wstring name;
	std::wstring type;
	std::shared_ptr<Expression> expr;
	bool reveal;

public:
	Parameter(std::wstring name, std::wstring type, bool reveal) : name(name), type(type), reveal(reveal)
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

	bool getReveal() const
	{
		return reveal;
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
