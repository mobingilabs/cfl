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

	static inline std::wstring unescape(const std::wstring& s)
	{
	  std::wstring res;
	  std::wstring::const_iterator it = s.begin();
	  while (it != s.end())
	  {
	    wchar_t c = *it++;
	    if (c == '\\' && it != s.end())
	    {
	      switch (*it++) {
	      case L'\\': c = L'\\'; break;
	      case L'n': c = L'\n'; break;
	      case L't': c = L'\t'; break;
	      case L'r': c = L'\r'; break;
	      case L'\"': c = L'\"'; break;
	      default: 
	        // invalid escape sequence - skip it. alternatively you can copy it as is, throw an exception...
	        continue;
	      }
	    }
	    res += c;
	  }

	  return res;
	}

public:

	StringLiteral(std::wstring str) : str(unescape(trim(str)))
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

	virtual ExpressionForm getForm() const
	{
		return STRING_LITERAL;
	}
};

#endif // STRINGLITERAL_H