#ifndef OPERATIONLIST_H
#define OPERATIONLIST_H

#include <memory>
#include <vector>
#include <fstream>
#include <assert.h>

#include "Expression.h"
#include "ConditionsTable.h"

class OperationList : public Expression
{
	class NotYet {};

	typedef std::pair< std::wstring, std::shared_ptr<Expression> > OpExprPair;
	std::shared_ptr<Expression> firstExpr;
	std::vector< OpExprPair > expressions;
	std::shared_ptr<ConditionsTable> ctable;
	
public:
	OperationList(std::shared_ptr<ConditionsTable> ctable, std::shared_ptr<Expression> firstExpr) : 
		firstExpr(firstExpr), ctable(ctable)
	{

	}

	void AddExpression(std::wstring op, std::shared_ptr<Expression> expr)
	{
		expressions.push_back(OpExprPair(op, expr));
	}

	void reduce(std::vector< std::shared_ptr<Expression> >& exprList, std::vector< std::wstring >& operators) const
	{
		std::vector< std::wstring > operatorsByPrecedence {
			L"==", 
			L"!=", 
			L"and", 
			L"or"
		};

		for (auto curOp : operatorsByPrecedence)
		{
			std::vector< std::shared_ptr<Expression> > newExprList;
			std::vector< std::wstring > newOperators;

			for (int i=0; i<operators.size(); i++)
			{
				if (operators[i].compare(curOp) == 0)
				{
					OperationList* opl = new OperationList(ctable, exprList[i]);
					opl->AddExpression(operators[i], exprList[i + 1]);
					std::shared_ptr<Expression> expr(opl);

					exprList[i + 1] = expr;
				}
				else
				{
					newOperators.push_back(operators[i]);
					newExprList.push_back(exprList[i]);
				}
			}

			newExprList.push_back(exprList[exprList.size() - 1]);

			exprList = newExprList;
			operators = newOperators;
		}
	}

	picojson::value generateFuncCall(std::wstring funname, picojson::value lhs) const
	{
		std::map<std::wstring, picojson::value> funcall;
		std::vector<picojson::value> params;

		params.push_back(lhs);

		funcall[funname] = picojson::value(params);
		return picojson::value(funcall);
	}

	picojson::value generateFuncCall(std::wstring funname, picojson::value lhs, picojson::value rhs) const
	{
		std::map<std::wstring, picojson::value> funcall;
		std::vector<picojson::value> params;

		params.push_back(lhs);
		params.push_back(rhs);

		funcall[funname] = picojson::value(params);
		return picojson::value(funcall);
	}

	int CommitToConditionTable(const Substitution& subs) const
	{
		std::vector< std::shared_ptr<Expression> > exprList;
		std::vector< std::wstring > operators;

		exprList.push_back(firstExpr);
		for (auto pair : expressions)
		{
			operators.push_back(pair.first);
			exprList.push_back(pair.second);
		}

		if (expressions.size() > 1)
		{
			reduce(exprList, operators);
		}

		assert(exprList.size() == 2 && operators.size() == 1);


		picojson::value resultingExpression;

		if (operators[0].compare(L"==") == 0)
		{
			resultingExpression = generateFuncCall(L"Fn::Equals", exprList[0]->asJson(subs), exprList[1]->asJson(subs));
		}
		else if (operators[0].compare(L"!=") == 0)
		{
			resultingExpression = generateFuncCall(L"Fn::Equals", exprList[0]->asJson(subs), exprList[1]->asJson(subs));
		}
		else if (operators[0].compare(L"and") == 0)
		{
			resultingExpression = generateFuncCall(L"Fn::And", exprList[0]->asJson(subs), exprList[1]->asJson(subs));
		}
		else if (operators[0].compare(L"or") == 0)
		{
			resultingExpression = generateFuncCall(L"Fn::Or", exprList[0]->asJson(subs), exprList[1]->asJson(subs));
		}

		return ctable->AddCondition(resultingExpression);
	}

	virtual picojson::value asJson(const Substitution& subs) const 
	{
		int cond = CommitToConditionTable(subs);

		std::map<std::wstring, picojson::value> funcall;

		std::vector<picojson::value> params;

		params.push_back(picojson::value(ctable->GetConditionName(cond)));
		params.push_back(picojson::value(true));
		params.push_back(picojson::value(false));

		funcall[L"Fn::If"] = picojson::value(params);

		return picojson::value(funcall);
	}

	virtual ExpressionForm getForm() const
	{
		return OPERATION_LIST;
	}

	virtual std::wstring getType(const Substitution& subs) const
	{
		return L"boolean";
	}
};

#endif // OPERATIONLIST_H
