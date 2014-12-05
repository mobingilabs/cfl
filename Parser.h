

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <stack>

#include "BooleanLiteral.h"
#include "NumberLiteral.h"
#include "StringLiteral.h"
#include "ArrayLiteral.h"
#include "RecordLiteral.h"
#include "RecordField.h"
#include "SymbolReference.h"
#include "FunctionCall.h"
#include "MemberCall.h"
#include "Metadata.h"
#include "Mapping.h"
#include "MappingCall.h"
#include "OperationList.h"
#include "TernaryExpression.h"

#include "Variable.h"
#include "Stack.h"
#include "Resource.h"
#include "Parameter.h"
#include "ConditionsTable.h"

typedef std::shared_ptr<Expression> ExpressionPtr;
typedef std::shared_ptr<RecordField> RecordFieldPtr;
typedef std::shared_ptr<Metadata> MetadataPtr;
typedef std::vector< std::shared_ptr<Expression> > ParameterList;
typedef std::vector< MetadataPtr > MetadataList;

typedef std::shared_ptr<Variable> VariablePtr;
typedef std::shared_ptr<Stack> StackPtr;
typedef std::shared_ptr<Resource> ResourcePtr;
typedef std::shared_ptr<Parameter> ParameterPtr;
typedef std::shared_ptr<Mapping> MappingPtr;
typedef std::shared_ptr<ConditionsTable> ConditionsTablePtr;

typedef std::shared_ptr<StringLiteral> StringLiteralPtr;


#include <string>
#include "Scanner.h"

	enum {
		_EOF=0,
		_ident=1,
		_number=2,
		_string=3,
		_absimp=4,
		_char=5,
		_nextLine=6,
		_comments=7,
		_ddtSym=34,
		_optionSym=35
	};



class Errors {
public:
	int count;			// number of errors detected

	Errors() {
		count = 0;
	}

	void SynErr(int line, int col, int n) {
		std::wstring s;
		switch (n) {
			case 0: s = CocoUtil::coco_string_create(L"EOF expected"); break;
			case 1: s = CocoUtil::coco_string_create(L"ident expected"); break;
			case 2: s = CocoUtil::coco_string_create(L"number expected"); break;
			case 3: s = CocoUtil::coco_string_create(L"string expected"); break;
			case 4: s = CocoUtil::coco_string_create(L"absimp expected"); break;
			case 5: s = CocoUtil::coco_string_create(L"char expected"); break;
			case 6: s = CocoUtil::coco_string_create(L"nextLine expected"); break;
			case 7: s = CocoUtil::coco_string_create(L"comments expected"); break;
			case 8: s = CocoUtil::coco_string_create(L"\"true\" expected"); break;
			case 9: s = CocoUtil::coco_string_create(L"\"false\" expected"); break;
			case 10: s = CocoUtil::coco_string_create(L"\"[\" expected"); break;
			case 11: s = CocoUtil::coco_string_create(L"\",\" expected"); break;
			case 12: s = CocoUtil::coco_string_create(L"\"]\" expected"); break;
			case 13: s = CocoUtil::coco_string_create(L"\":\" expected"); break;
			case 14: s = CocoUtil::coco_string_create(L"\"{\" expected"); break;
			case 15: s = CocoUtil::coco_string_create(L"\"}\" expected"); break;
			case 16: s = CocoUtil::coco_string_create(L"\"(\" expected"); break;
			case 17: s = CocoUtil::coco_string_create(L"\")\" expected"); break;
			case 18: s = CocoUtil::coco_string_create(L"\".\" expected"); break;
			case 19: s = CocoUtil::coco_string_create(L"\"==\" expected"); break;
			case 20: s = CocoUtil::coco_string_create(L"\"!=\" expected"); break;
			case 21: s = CocoUtil::coco_string_create(L"\"and\" expected"); break;
			case 22: s = CocoUtil::coco_string_create(L"\"or\" expected"); break;
			case 23: s = CocoUtil::coco_string_create(L"\"?\" expected"); break;
			case 24: s = CocoUtil::coco_string_create(L"\"SET\" expected"); break;
			case 25: s = CocoUtil::coco_string_create(L"\"=\" expected"); break;
			case 26: s = CocoUtil::coco_string_create(L"\"MAKE\" expected"); break;
			case 27: s = CocoUtil::coco_string_create(L"\"RETURN\" expected"); break;
			case 28: s = CocoUtil::coco_string_create(L"\"if\" expected"); break;
			case 29: s = CocoUtil::coco_string_create(L"\"STACK\" expected"); break;
			case 30: s = CocoUtil::coco_string_create(L"\";\" expected"); break;
			case 31: s = CocoUtil::coco_string_create(L"\"MAPPING\" expected"); break;
			case 32: s = CocoUtil::coco_string_create(L"\"IMPORT\" expected"); break;
			case 33: s = CocoUtil::coco_string_create(L"??? expected"); break;

			default:
			{
				std::wstringstream ss;
				ss << "error " << n;
				s = ss.str();
			}
			break;
		}
		fwprintf(stderr, L"-- line %d col %d: %ls\n", line, col, s.c_str());
		CocoUtil::coco_string_delete(s);
		count++;
	}

	void Error(int line, int col, const std::wstring s) {
		fwprintf(stderr, L"-- line %d col %d: %ls\n", line, col, s.c_str());
		count++;
	}

	void Warning(int line, int col, const std::wstring s) {
		fwprintf(stderr, L"-- line %d col %d: %ls\n", line, col, s.c_str());
	}

	void Warning(const std::wstring s) {
		fwprintf(stderr, L"%ls\n", s.c_str());
	}

	void Exception(const std::wstring s) {
		fwprintf(stderr, L"%ls", s.c_str()); 
		exit(1);
	}
						

}; // Errors


class Parser {
private:

	std::shared_ptr<Token> dummyToken;
	int errDist;
	int minErrDist;
	int maxT;


	void SynErr(int n) {
		if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
		errDist = 0;
	}


	void Get() {
		for (;;) {
			t = la;
			la = scanner->Scan();
			if (la->kind <= maxT) { ++errDist; break; }
			if (la->kind == _ddtSym) {
		}
		if (la->kind == _optionSym) {
		}

			if (dummyToken != t) {
				dummyToken->kind = t->kind;
				dummyToken->pos = t->pos;
				dummyToken->col = t->col;
				dummyToken->line = t->line;
				dummyToken->next = NULL;
				dummyToken->val = t->val;
				t = dummyToken;
			}
			la = t;
		}
	}

	void Expect(int n) {
		if (la->kind==n) Get(); else { SynErr(n); }
	}

	void ExpectWeak(int n, int follow) {
		if (la->kind == n) Get();
		else {
			SynErr(n);
			while (!StartOf(follow)) Get();
		}
	}

	bool WeakSeparator(int n, int syFol, int repFol) {
		if (la->kind == n) {Get(); return true;}
		else if (StartOf(repFol)) {return false;}
		else {
			SynErr(n);
			while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
				Get();
			}
			return StartOf(syFol);
		}
	}


	void Boolean(ExpressionPtr &expr) {
		if (la->kind == 8 /* "true" */) {
			Get();
			expr = ExpressionPtr(new BooleanLiteral(true)); 
		} else if (la->kind == 9 /* "false" */) {
			Get();
			expr = ExpressionPtr(new BooleanLiteral(false)); 
		} else SynErr(34);
}

void String(ExpressionPtr &expr) {
		Expect(_string);
		expr = ExpressionPtr(new StringLiteral(t->val)); 
}

void Number(ExpressionPtr &expr) {
		Expect(_number);
		expr = ExpressionPtr(new NumberLiteral(std::stold(t->val))); 
}

void Array(ExpressionPtr &expr) {
		ExpressionPtr member; 
		ArrayLiteral* arr = new ArrayLiteral(); 
		expr = ExpressionPtr(arr); 
		Expect(10 /* "[" */);
		if (StartOf(1)) {
			Expression(member);
			arr->Add(member); 
			while (la->kind == 11 /* "," */) {
				Get();
				Expression(member);
				arr->Add(member); 
			}
		}
		Expect(12 /* "]" */);
}

void Expression(ExpressionPtr &expr) {
		Term(expr);
		if (StartOf(2)) {
			if (StartOf(3)) {
				InfixExpression(expr);
			} else {
				TernaryExpression(expr);
			}
		}
}

void RecordField(RecordFieldPtr &record) {
		std::wstring key; ExpressionPtr value; 
		Expect(_ident);
		key = t->val; 
		Expect(13 /* ":" */);
		Expression(value);
		record = RecordFieldPtr(new class RecordField(key, value)); 
}

void Record(ExpressionPtr &expr) {
		RecordFieldPtr record; 
		RecordLiteral* rec = new RecordLiteral(); 
		expr = ExpressionPtr(rec);
		Expect(14 /* "{" */);
		if (la->kind == _ident) {
			RecordField(record);
			rec->Add(record); 
			while (la->kind == 11 /* "," */) {
				Get();
				RecordField(record);
				rec->Add(record); 
			}
		}
		Expect(15 /* "}" */);
}

void Literal(ExpressionPtr &expr) {
		if (la->kind == _string) {
			String(expr);
		} else if (la->kind == _number) {
			Number(expr);
		} else if (la->kind == 10 /* "[" */) {
			Array(expr);
		} else if (la->kind == 14 /* "{" */) {
			Record(expr);
		} else if (la->kind == 8 /* "true" */ || la->kind == 9 /* "false" */) {
			Boolean(expr);
		} else SynErr(35);
}

void FunctionCall(ParameterList &list) {
		ExpressionPtr expr; 
		Expect(16 /* "(" */);
		if (StartOf(1)) {
			Expression(expr);
			list.push_back(expr); 
			while (la->kind == 11 /* "," */) {
				Get();
				Expression(expr);
				list.push_back(expr); 
			}
		}
		Expect(17 /* ")" */);
}

void MemberCall(std::wstring &member) {
		Expect(18 /* "." */);
		Expect(_ident);
		member = t->val; 
}

void MappingCall(ExpressionPtr &expr1, ExpressionPtr &expr2) {
		Expect(10 /* "[" */);
		Expression(expr1);
		Expect(11 /* "," */);
		Expression(expr2);
		Expect(12 /* "]" */);
}

void Reference(ExpressionPtr &expr) {
		std::wstring member; 
		Expect(_ident);
		std::wstring ident = t->val; 
		expr = ExpressionPtr(new SymbolReference(ident)); 
		ParameterList paramList; 
		ExpressionPtr expr1; 
		ExpressionPtr expr2; 
		if (la->kind == 10 /* "[" */ || la->kind == 16 /* "(" */ || la->kind == 18 /* "." */) {
			if (la->kind == 16 /* "(" */) {
				FunctionCall(paramList);
				expr = ExpressionPtr(new class FunctionCall(ident, paramList)); 
			} else if (la->kind == 18 /* "." */) {
				MemberCall(member);
				expr = ExpressionPtr(new class MemberCall(ident, member)); 
			} else {
				MappingCall(expr1, expr2);
				expr = ExpressionPtr(new class MappingCall(ident, expr1, expr2)); 
			}
		}
}

void Term(ExpressionPtr &expr) {
		if (StartOf(4)) {
			if (StartOf(5)) {
				Literal(expr);
			} else {
				Reference(expr);
			}
		} else if (la->kind == 16 /* "(" */) {
			Get();
			Expression(expr);
			Expect(17 /* ")" */);
		} else SynErr(36);
}

void BooleanBinaryOperator(std::wstring &op) {
		if (la->kind == 19 /* "==" */) {
			Get();
			op = t->val; 
		} else if (la->kind == 20 /* "!=" */) {
			Get();
			op = t->val; 
		} else if (la->kind == 21 /* "and" */) {
			Get();
			op = t->val; 
		} else if (la->kind == 22 /* "or" */) {
			Get();
			op = t->val; 
		} else SynErr(37);
}

void BinaryOperator(std::wstring &op) {
		BooleanBinaryOperator(op);
}

void InfixExpression(ExpressionPtr &expr) {
		OperationList* list = new OperationList(ctable, expr); 
		expr = ExpressionPtr(list); 
		ExpressionPtr rhs; 
		std::wstring op;   
		BinaryOperator(op);
		Term(rhs);
		list->AddExpression(op, rhs); 
		while (StartOf(3)) {
			BinaryOperator(op);
			Term(rhs);
			list->AddExpression(op, rhs); 
		}
}

void TernaryExpression(ExpressionPtr &expr) {
		ExpressionPtr trueExpr, falseExpr; 
		Expect(23 /* "?" */);
		Expression(trueExpr);
		Expect(13 /* ":" */);
		Expression(falseExpr);
		expr = ExpressionPtr(new class TernaryExpression(expr, trueExpr, falseExpr)); 
}

void VariableDeclaration(VariablePtr &variable) {
		ExpressionPtr expr; 
		Expect(24 /* "SET" */);
		Expect(_ident);
		std::wstring identifier = t->val; 
		Expect(25 /* "=" */);
		Expression(expr);
		variable = VariablePtr(new Variable(identifier, expr)); 
}

void Property(ResourcePtr resource) {
		ExpressionPtr expr; 
		std::wstring name; 
		Expect(_ident);
		name = t->val; 
		Expect(25 /* "=" */);
		Expression(expr);
		resource->AddProperty(name, expr); 
}

void ExtraMetadataInfo(MetadataPtr& metadata) {
		ExpressionPtr expr; 
		Expect(_ident);
		std::wstring key = t->val; 
		Expect(13 /* ":" */);
		Expression(expr);
		metadata->addInfo(key, expr); 
}

void MetadataDeclaration(MetadataPtr& metadata) {
		ExpressionPtr expr; 
		Expect(10 /* "[" */);
		Expect(_ident);
		std::wstring key = t->val; 
		Expect(13 /* ":" */);
		Expression(expr);
		metadata = MetadataPtr(new Metadata(key, expr)); 
		while (la->kind == 11 /* "," */) {
			Get();
			ExtraMetadataInfo(metadata);
		}
		Expect(12 /* "]" */);
}

void MetadataListing(MetadataList& list) {
		MetadataPtr metadata; 
		while (la->kind == 10 /* "[" */) {
			MetadataDeclaration(metadata);
			list.push_back(metadata); 
		}
}

void ResourceDeclaration(StackPtr stack) {
		std::wstring type, name; 
		MetadataList list; 
		MetadataListing(list);
		Expect(26 /* "MAKE" */);
		Expect(_ident);
		type = t->val; 
		Expect(_ident);
		name = t->val; 
		ResourcePtr resource = ResourcePtr(new Resource(type, name, list)); 
		if (la->kind == 16 /* "(" */) {
			Get();
			if (la->kind == _ident) {
				Property(resource);
				while (la->kind == 11 /* "," */) {
					Get();
					Property(resource);
				}
			}
			Expect(17 /* ")" */);
		}
		resource->SetCondition(conditions); 
		stack->AddResource(resource); 
}

void StackParameter(StackPtr stack) {
		std::wstring type, name; 
		ExpressionPtr expr; 
		Expect(_ident);
		type = t->val; 
		Expect(_ident);
		name = t->val; 
		ParameterPtr ptr(new Parameter(name, type)); 
		stack->AddParameter(ptr); 
		if (la->kind == 25 /* "=" */) {
			Get();
			Expression(expr);
			ptr->setDefault(expr); 
		}
}

void OutputParameter(StackPtr stack) {
		ExpressionPtr expr; 
		std::wstring name; 
		Expect(_ident);
		name = t->val; 
		Expect(25 /* "=" */);
		Expression(expr);
		stack->AddOutput(name, expr); 
}

void OutputDeclaration(StackPtr stack) {
		Expect(27 /* "RETURN" */);
		Expect(16 /* "(" */);
		OutputParameter(stack);
		while (la->kind == 11 /* "," */) {
			Get();
			OutputParameter(stack);
		}
		Expect(17 /* ")" */);
}

void IfStatement(StackPtr stack) {
		ExpressionPtr condExpr; 
		Expect(28 /* "if" */);
		Expect(16 /* "(" */);
		Expression(condExpr);
		Expect(17 /* ")" */);
		Expect(14 /* "{" */);
		conditions.push(condExpr); 
		while (la->kind == 10 /* "[" */ || la->kind == 26 /* "MAKE" */ || la->kind == 28 /* "if" */) {
			StackStatement(stack);
		}
		Expect(15 /* "}" */);
		conditions.pop(); 
}

void StackStatement(StackPtr stack) {
		if (la->kind == 10 /* "[" */ || la->kind == 26 /* "MAKE" */) {
			ResourceDeclaration(stack);
		} else if (la->kind == 28 /* "if" */) {
			IfStatement(stack);
		} else SynErr(38);
}

void StackDeclaration(StackPtr &stack) {
		Expect(29 /* "STACK" */);
		Expect(_ident);
		stack = StackPtr(new Stack(t->val)); 
		Expect(16 /* "(" */);
		if (la->kind == _ident) {
			StackParameter(stack);
			while (la->kind == 11 /* "," */) {
				Get();
				StackParameter(stack);
			}
		}
		Expect(17 /* ")" */);
		Expect(14 /* "{" */);
		while (la->kind == 10 /* "[" */ || la->kind == 26 /* "MAKE" */ || la->kind == 28 /* "if" */) {
			StackStatement(stack);
		}
		if (la->kind == 27 /* "RETURN" */) {
			OutputDeclaration(stack);
		}
		Expect(15 /* "}" */);
}

void MappingRow(MappingPtr& mapping) {
		std::vector< std::wstring > data; 
		Expect(_string);
		std::wstring rowname = StringLiteral(t->val).getContent(); 
		Expect(25 /* "=" */);
		Expect(_string);
		data.push_back(StringLiteral(t->val).getContent()); 
		while (la->kind == 11 /* "," */) {
			Get();
			Expect(_string);
			data.push_back(StringLiteral(t->val).getContent()); 
		}
		Expect(30 /* ";" */);
		mapping->AddRow(rowname, data); 
}

void MappingDeclaration(MappingPtr& mapping) {
		Expect(31 /* "MAPPING" */);
		Expect(_ident);
		mapping = MappingPtr(new Mapping(t->val)); 
		Expect(16 /* "(" */);
		Expect(_string);
		mapping->AddColumn(StringLiteral(t->val).getContent()); 
		while (la->kind == 11 /* "," */) {
			Get();
			Expect(_string);
			mapping->AddColumn(StringLiteral(t->val).getContent()); 
		}
		Expect(17 /* ")" */);
		Expect(14 /* "{" */);
		while (la->kind == _string) {
			MappingRow(mapping);
		}
		Expect(15 /* "}" */);
}

void Statement() {
		VariablePtr variable; 
		StackPtr stack; 
		MappingPtr mapping; 
		if (la->kind == 24 /* "SET" */) {
			VariableDeclaration(variable);
			variables.push_back(variable); 
		} else if (la->kind == 29 /* "STACK" */) {
			StackDeclaration(stack);
			stacks.push_back(stack); 
		} else if (la->kind == 31 /* "MAPPING" */) {
			MappingDeclaration(mapping);
			mappings.push_back(mapping); 
		} else SynErr(39);
}

void Description() {
		Expect(_comments);
		description = t->val; 
}

void ImportStatement() {
		Expect(32 /* "IMPORT" */);
		if (la->kind == _string) {
			Get();
			StringLiteralPtr str = StringLiteralPtr(new StringLiteral(t->val)); 
			imports.push_back(str); 
		} else if (la->kind == _absimp) {
			Get();
			StringLiteralPtr str = StringLiteralPtr(new StringLiteral(t->val)); 
			absoluteImports.push_back(str); 
		} else SynErr(40);
}

void CFL() {
		if (la->kind == _comments) {
			Description();
		}
		while (la->kind == 32 /* "IMPORT" */) {
			ImportStatement();
		}
		while (la->kind == 24 /* "SET" */ || la->kind == 29 /* "STACK" */ || la->kind == 31 /* "MAPPING" */) {
			Statement();
		}
		Expect(_EOF);
}



public:

	std::shared_ptr<Scanner> scanner;
	std::shared_ptr<Errors>  errors;

	std::shared_ptr<Token> t;			// last recognized token
	std::shared_ptr<Token> la;			// lookahead token

std::wstring description = L"Created using git@github.com:davidsiaw/cfl. Since this is generated code, your changes will likely be replaced.";

	std::vector<VariablePtr> variables;
	std::vector<StackPtr> stacks;
	std::vector<StringLiteralPtr> imports;
	std::vector<StringLiteralPtr> absoluteImports;
	std::vector<MappingPtr> mappings;

	std::stack<ExpressionPtr> conditions; 

	ConditionsTablePtr ctable;



	// If the user declared a method Init and a mehtod Destroy they should
	// be called in the contructur and the destructor respctively.
	//
	// The following templates are used to recognize if the user declared
	// the methods Init and Destroy.

	template<typename T>
	struct ParserInitExistsRecognizer {
		template<typename U, void (U::*)() = &U::Init>
		struct ExistsIfInitIsDefinedMarker{};

		struct InitIsMissingType {
			char dummy1;
		};
		
		struct InitExistsType {
			char dummy1; char dummy2;
		};

		// exists always
		template<typename U>
		static InitIsMissingType is_here(...);

		// exist only if ExistsIfInitIsDefinedMarker is defined
		template<typename U>
		static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

		enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
	};

	template<typename T>
	struct ParserDestroyExistsRecognizer {
		template<typename U, void (U::*)() = &U::Destroy>
		struct ExistsIfDestroyIsDefinedMarker{};

		struct DestroyIsMissingType {
			char dummy1;
		};
		
		struct DestroyExistsType {
			char dummy1; char dummy2;
		};

		// exists always
		template<typename U>
		static DestroyIsMissingType is_here(...);

		// exist only if ExistsIfDestroyIsDefinedMarker is defined
		template<typename U>
		static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

		enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
	};

	// The folloing templates are used to call the Init and Destroy methods if they exist.

	// Generic case of the ParserInitCaller, gets used if the Init method is missing
	template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
	struct ParserInitCaller {
		static void CallInit(T *t) {
			// nothing to do
		}
	};

	// True case of the ParserInitCaller, gets used if the Init method exists
	template<typename T>
	struct ParserInitCaller<T, true> {
		static void CallInit(T *t) {
			t->Init();
		}
	};

	// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
	template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
	struct ParserDestroyCaller {
		static void CallDestroy(T *t) {
			// nothing to do
		}
	};

	// True case of the ParserDestroyCaller, gets used if the Destroy method exists
	template<typename T>
	struct ParserDestroyCaller<T, true> {
		static void CallDestroy(T *t) {
			t->Destroy();
		}
	};

	void Parse() {
		t = NULL;
		la = dummyToken = std::shared_ptr<Token>(new Token());
		la->val = CocoUtil::coco_string_create(L"Dummy Token");
		Get();
		CFL();
	Expect(0);
	}

	Parser(std::shared_ptr<Scanner> scanner) {
		maxT = 33;

		ParserInitCaller<Parser>::CallInit(this);
		dummyToken = NULL;
		t = la = NULL;
		minErrDist = 2;
		errDist = minErrDist;
		this->scanner = scanner;
		errors = std::shared_ptr<Errors>(new Errors());
	}


	~Parser() {
		ParserDestroyCaller<Parser>::CallDestroy(this);
	}

	void SemErr(const std::wstring msg) {
		if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
		errDist = 0;
	}

private:

	bool StartOf(int s) {
		const bool T = true;
		const bool x = false;

		static bool set[6][35] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,T, x,x,x,x, T,T,T,x, x,x,T,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,T, x,x,x,x, T,T,T,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,T,T, x,x,x,x, T,T,T,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x}
	};



		return set[s][la->kind];
	}


}; // end Parser






#endif

