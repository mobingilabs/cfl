#ifndef STRINGLITERAL_H
#define STRINGLITERAL_H

#include <string>
#include "Expression.h"


class StringLiteral : public Expression
{
	std::wstring str;

	// trim quotes from both ends
	static inline std::wstring trim(const std::wstring &s) {
		return s.substr(1, s.size() - 2);
	}

public:

	StringLiteral(std::wstring str) : str(trim(str))
	{

	}

	virtual picojson::value asJson(const Substitution& subs) const 
	{
		return picojson::value(str);
	}

	std::wstring getContent() const
	{
		return str;
	}
};

#endif // STRINGLITERAL_H