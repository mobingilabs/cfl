#ifndef TERNARYEXPRESSION_H
#define TERNARYEXPRESSION_H

#include <memory>
#include "Expression.h"

class TernaryExpression : public Expression
{
	std::shared_ptr<Expression> condition;
	std::shared_ptr<Expression> trueExpr;
	std::shared_ptr<Expression> falseExpr;

public:

	TernaryExpression(std::shared_ptr<Expression> condition, std::shared_ptr<Expression> trueExpr, std::shared_ptr<Expression> falseExpr)
	: condition(condition), trueExpr(trueExpr), falseExpr(falseExpr)
	{
	}

	virtual picojson::value asJson(std::shared_ptr<Substitution> subs, bool forConditionSection) const
	{
		if ( condition->getType(subs).compare(L"boolean") != 0 )
		{
			std::wcerr << "Ternary (?:) expression condition must be boolean" << std::endl;
			exit(EXIT_FAILURE);
		}

		if ( trueExpr->getType(subs).compare(falseExpr->getType(subs)) != 0 )
		{
			//std::wcerr << "True and false results of the ternary (?:) expression must be of the same type." << std::endl;
			//exit(EXIT_FAILURE);
		}

		std::map<std::wstring, picojson::value> funcall;


		if (condition->asJson(subs, true).is<picojson::object>())
		{
			std::vector<picojson::value> params;

			if (condition->asJson(subs, true).get<picojson::object>()[L"Condition"].is<picojson::null>())
			{
                if (condition->asJson(subs, true).get<picojson::object>()[L"Fn::If"].is<picojson::array>())
                {
                    params.push_back(condition->asJson(subs, true).get<picojson::object>()[L"Fn::If"].get<picojson::array>()[0]);
                }
                else
                {
                    std::wcout << "Unable to determine condition type" << std::endl;
                    exit(1);
                }
			}
            else
            {
                params.push_back(condition->asJson(subs, true).get<picojson::object>()[L"Condition"]);
            }


			params.push_back(trueExpr->asJson(subs, forConditionSection));
			params.push_back(falseExpr->asJson(subs, forConditionSection));
			funcall[L"Fn::If"] = picojson::value(params);
		}
		else if (condition->asJson(subs, true).is<bool>())
		{
			if (condition->asJson(subs, true).get<bool>())
			{
				return trueExpr->asJson(subs, forConditionSection);
			}
			else
			{
				return falseExpr->asJson(subs, forConditionSection);
			}
		}
		else
		{
			std::wcerr << "Unknown kind of condition: " << condition->asJson(subs, true).serialize();
			exit(EXIT_FAILURE);
		}

		return picojson::value(funcall);
	}

	virtual ExpressionForm getForm() const
	{
		return TERNARY_EXPRESSION;
	}

	virtual std::wstring getType(std::shared_ptr<Substitution> subs) const 
	{
		return trueExpr->getType(subs);
	}
};

#endif // TERNARYEXPRESSION_H