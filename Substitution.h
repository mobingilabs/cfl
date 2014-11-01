#ifndef SUBSTITUTION_H
#define SUBSTITUTION_H

#include <iostream>
#include "picojson/picojson.h"

class Substitution
{
	std::map<std::wstring, picojson::value> subs;

	class NoSubstitute {};

public:
	Substitution()
	{

	}

	void Add(std::wstring str, picojson::value json)
	{
		subs[str] = json;
	}

	picojson::value Substitute(std::wstring str) const
	{
		if (HasSubstitute(str))
		{
			return subs.find(str)->second;
		}

		throw NoSubstitute();
	}

	bool HasSubstitute(std::wstring str) const
	{
		return subs.find(str) != subs.end();
	}
};

#endif // SUBSTITUTION_H
