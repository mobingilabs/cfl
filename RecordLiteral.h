#ifndef RECORDLITERAL_H
#define RECORDLITERAL_H

#include <vector>
#include <memory>

#include "Expression.h"
#include "RecordField.h"

class RecordLiteral : public Expression
{
	std::vector< std::shared_ptr<RecordField> > fields;

public:
	RecordLiteral()
	{
		
	}

	void Add(std::shared_ptr<RecordField> field)
	{
		fields.push_back(field);
	}
	
	virtual picojson::value asJson(const Substitution& subs) const 
	{
		std::map<std::wstring, picojson::value> obj;

		for (const std::shared_ptr<RecordField> &field : fields)
		{
			obj[field->getKey()] = field->getValue()->asJson(subs);
		}

		return picojson::value(obj);
	}

	virtual ExpressionForm getForm() const
	{
		return RECORD_LITERAL;
	}

	virtual std::wstring getType(const Substitution& subs) const
	{
		return L"record";
	}
};

#endif // RECORDLITERAL_H
