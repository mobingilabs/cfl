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
				if (operators[i].compare(curOp) == 0 &&
					newOperators.size() + operators.size() - i > 1)
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

			if (exprList.size() == 2)
			{
				break;
			}

			//std::wcerr << "Result of reduction " << curOp << ": " << exprList.size() << " -> " << newExprList.size() << std::endl;

			exprList = newExprList;
			operators = newOperators;
		}
	}

	picojson::value generateFuncCall(std::wstring funname, picojson::value lhs) const
	{
		std::map<std::wstring, picojson::value> funcall;
		std::vector<picojson::value> params;

		if (funname.compare(L"Fn::Not") == 0 && lhs.is<bool>())
		{
			return picojson::value(!lhs.get<bool>());
		}

		params.push_back(lhs);

		funcall[funname] = picojson::value(params);
		return picojson::value(funcall);
	}

	picojson::value generateFuncCall(std::wstring funname, picojson::value lhs, picojson::value rhs) const
	{
		std::map<std::wstring, picojson::value> funcall;
		std::vector<picojson::value> params;

		if (funname.compare(L"Fn::And") == 0)
		{
			if (lhs.is<bool>() && rhs.is<bool>())
			{
				return picojson::value(lhs.get<bool>() && rhs.get<bool>());
			}

			std::shared_ptr<bool> val;
			picojson::value other;

			if (lhs.is<bool>())
			{
				val = std::make_shared<bool>(lhs.get<bool>());
				other = rhs;
			}

			if (rhs.is<bool>())
			{
				val = std::make_shared<bool>(rhs.get<bool>());
				other = lhs;
			}

			if (val)
			{
				if (*val == true)
				{
					return other;
				}
				else
				{
					return picojson::value(false);
				}
			}
		}

		if (funname.compare(L"Fn::Or") == 0)
		{
			if (lhs.is<bool>() && rhs.is<bool>())
			{
				return picojson::value(lhs.get<bool>() || rhs.get<bool>());
			}

			std::shared_ptr<bool> val;
			picojson::value other;

			if (lhs.is<bool>())
			{
				val = std::make_shared<bool>(lhs.get<bool>());
				other = rhs;
			}

			if (rhs.is<bool>())
			{
				val = std::make_shared<bool>(rhs.get<bool>());
				other = lhs;
			}

			if (val)
			{
				if (*val == true)
				{
					return picojson::value(true);
				}
				else
				{
					return other;
				}
			}
		}


		params.push_back(lhs);
		params.push_back(rhs);

		funcall[funname] = picojson::value(params);
		return picojson::value(funcall);
	}

	int CommitToConditionTable(const Substitution& subs) const
	{
		auto resultingExpression = getConditionResult(subs);
		return ctable->AddCondition(resultingExpression);
	}

	picojson::value getConditionResult(const Substitution& subs) const
	{
		if (expressions.size() == 0)
		{
			return firstExpr->asJson(subs, true);
		}

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

		//std::wcerr << "exprList.size() == " << exprList.size() << ", operators.size() ==" << operators.size() << std::endl;
		assert(exprList.size() == 2 && operators.size() == 1);


		picojson::value resultingExpression;

		if (operators[0].compare(L"==") == 0)
		{
			if (exprList[0]->getType(subs).compare(exprList[1]->getType(subs)) != 0)
			{
				std::wcerr << "Cannot compare " << exprList[0]->getType(subs) << " and " << exprList[1]->getType(subs) << std::endl;
				exit(EXIT_FAILURE);
			}

			if (exprList[0]->getType(subs).compare(L"boolean") == 0)
			{
				// Need to perform XNOR for equality of two booleans
				// because CF templates really really suck
				resultingExpression = 
					generateFuncCall(L"Fn::Or", 
						generateFuncCall(L"Fn::And", 
							exprList[0]->asJson(subs, true), 
							exprList[1]->asJson(subs, true)),
						generateFuncCall(L"Fn::And", 
							generateFuncCall(L"Fn::Not", exprList[0]->asJson(subs, true)), 
							generateFuncCall(L"Fn::Not", exprList[1]->asJson(subs, true))
						)

					);
			}
			else
			{
				resultingExpression = generateFuncCall(L"Fn::Equals", exprList[0]->asJson(subs, true), exprList[1]->asJson(subs, true));
			}

		}
		else if (operators[0].compare(L"!=") == 0)
		{
			if (exprList[0]->getType(subs).compare(L"boolean") == 0)
			{
				// Need to perform XOR for equality of two booleans here too.
				resultingExpression = 
					generateFuncCall(L"Fn::Or", 
						generateFuncCall(L"Fn::And", 
							generateFuncCall(L"Fn::Not", exprList[0]->asJson(subs, true)), 
							exprList[1]->asJson(subs, true)
						),
						generateFuncCall(L"Fn::And", 
							exprList[0]->asJson(subs, true), 
							generateFuncCall(L"Fn::Not", exprList[1]->asJson(subs, true))
						)

					);
			}
			else
			{
				resultingExpression = generateFuncCall(L"Fn::Not",
					generateFuncCall(L"Fn::Equals", exprList[0]->asJson(subs, true), exprList[1]->asJson(subs, true)));
			}
		}
		else if (operators[0].compare(L"and") == 0)
		{
			resultingExpression = generateFuncCall(L"Fn::And", exprList[0]->asJson(subs, true), exprList[1]->asJson(subs, true));
		}
		else if (operators[0].compare(L"or") == 0)
		{
			resultingExpression = generateFuncCall(L"Fn::Or", exprList[0]->asJson(subs, true), exprList[1]->asJson(subs, true));
		}

		return resultingExpression;
	}

	virtual picojson::value asJson(const Substitution& subs, bool forConditionSection) const 
	{
		if (expressions.size() == 0)
		{
			return firstExpr->asJson(subs, forConditionSection);
		}

		int cond = CommitToConditionTable(subs);

		if (forConditionSection)
		{
			if (ctable->GetConditionByNum(cond).is<bool>())
			{
				return ctable->GetConditionByNum(cond);
			};

			std::map<std::wstring, picojson::value> condRef;
			condRef[L"Condition"] = picojson::value(ctable->GetConditionName(cond));
			return picojson::value(condRef);
		}

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
		if (expressions.size() > 0)
		{
			return L"boolean";
		}

		return firstExpr->getType(subs);
	}
};

#endif // OPERATIONLIST_H
