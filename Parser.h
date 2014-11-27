

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__

#include <string>
#include <map>
#include <vector>
#include <memory>

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

#include "Variable.h"
#include "Stack.h"
#include "Resource.h"
#include "Parameter.h"

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

typedef std::shared_ptr<StringLiteral> StringLiteralPtr;


#include "Scanner.h"



class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_ident=1,
		_number=2,
		_string=3,
		_absimp=4,
		_char=5,
		_nextLine=6,
		_comments=7,
		_ddtSym=28,
		_optionSym=29
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token

std::wstring description = L"Created using git@github.com:davidsiaw/cfl. Since this is generated code, your changes will likely be replaced.";

	std::vector<VariablePtr> variables;
	std::vector<StackPtr> stacks;
	std::vector<StringLiteralPtr> imports;
	std::vector<StringLiteralPtr> absoluteImports;
	std::vector<MappingPtr> mappings;



	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void Boolean(ExpressionPtr &expr);
	void String(ExpressionPtr &expr);
	void Number(ExpressionPtr &expr);
	void Array(ExpressionPtr &expr);
	void Expression(ExpressionPtr &expr);
	void RecordField(RecordFieldPtr &record);
	void Record(ExpressionPtr &expr);
	void Literal(ExpressionPtr &expr);
	void FunctionCall(ParameterList &list);
	void MemberCall(std::wstring &member);
	void MappingCall(ExpressionPtr &expr1, ExpressionPtr &expr2);
	void Reference(ExpressionPtr &expr);
	void VariableDeclaration(VariablePtr &variable);
	void Property(ResourcePtr resource);
	void ExtraMetadataInfo(MetadataPtr& metadata);
	void MetadataDeclaration(MetadataPtr& metadata);
	void MetadataListing(MetadataList& list);
	void ResourceDeclaration(StackPtr stack);
	void StackParameter(StackPtr stack);
	void OutputParameter(StackPtr stack);
	void OutputDeclaration(StackPtr stack);
	void StackDeclaration(StackPtr &stack);
	void MappingRow(MappingPtr& mapping);
	void MappingDeclaration(MappingPtr& mapping);
	void Statement();
	void Description();
	void ImportStatement();
	void CFL();

	void Parse();

}; // end Parser



#endif

