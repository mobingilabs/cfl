#ifndef STRINGLITERAL_H
#define STRINGLITERAL_H

#include <string>
#include "Expression.h"


class StringLiteral : public Expression
{
	std::wstring str;

	static int isDoubleQuote(int chr)
	{
		if (chr == '\"')
		{
			return 1;
		}
		return 0;
	}

	// trim from start
	static inline std::wstring &ltrim(std::wstring &s) {
	        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(isDoubleQuote))));
	        return s;
	}

	// trim from end
	static inline std::wstring &rtrim(std::wstring &s) {
	        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(isDoubleQuote))).base(), s.end());
	        return s;
	}

	// trim from both ends
	static inline std::wstring &trim(std::wstring &s) {
	        return ltrim(rtrim(s));
	}

public:

	StringLiteral(std::wstring str) : str(trim(str))
	{

	}

	virtual picojson::value asJson(const Substitution& subs) const 
	{
		return picojson::value(str);
	}
};

#endif // STRINGLITERAL_H