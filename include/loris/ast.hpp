/*

Copyright (C) 2014-2018 Nicolas Brown

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once
#include <string>
#include <vector>

using namespace std;
#include "lexer.hpp"

namespace loris
{

class ASTNode
{
public:
	virtual ~ASTNode()
	{
	}

	enum Type
	{
		//expressions
		Expr,
		ExprStmt,
		BinaryExpr,
		//literals
		NumberLiteral,
		StringLiteral,
		BoolLiteral,
		NullLiteral,
		//itentifiers and expressions
		Iden,
		PropAccess,
		FunctionCall,
		New,
		Var,
		//statements
		IfStmt,
		ReturnStmt,
		WhileStmt,
		///definitions
		Enum,
		FunctionDef,
		ClassDef,

		//other
		BlockStmt,
		Neg
	};

	Type type;
	int line;
};

class Statement:public ASTNode{
};

class Block:public Statement
{
public:
	vector<Statement*> statements;

	Block()
	{
		type = Type::BlockStmt;
	}

	void AddStatement(Statement *stmt)
	{
		statements.push_back(stmt);
	}
};

class ClassDefinition;
class FunctionDefinition;

class Program:public ASTNode
{
public:
	vector<ClassDefinition*> classes;
	vector<FunctionDefinition*> functions;

	void AddClass(ClassDefinition* def)
	{
		classes.push_back(def);
	}

	void AddFunction(FunctionDefinition* func)
	{
		functions.push_back(func);
	}

};

class ImportStatement:public Statement
{
public:
	vector<string> path;

	void AddToPath(string part)
	{
		path.push_back(part);
	}
};

class ClassAttribDefinition:public ASTNode
{
public:
	string name;
	string type;
	bool isStatic;
	FunctionDefinition* init;//initialization expression enclosed in a function

	ClassAttribDefinition()
	{
		init = nullptr;
	}

	void SetStatic(bool stat)
	{
		isStatic = stat;
	}

	void SetName(string name)
	{
		this->name = name;
	}

	void SetType(string type)
	{
		this->type = type;
	}
};

class FunctionDefinition;

class ClassDefinition:public ASTNode
{
public:
	string name;
	string superClass;
	vector<ClassAttribDefinition*> attribs;
	vector<FunctionDefinition*> functions;

	void SetName(string name)
	{
		this->name = name;
	}

	void SetSuperClass(string name)
	{
		this->superClass = name;
	}
 
	void AddAttrib(ClassAttribDefinition* attrib)
	{
		attribs.push_back(attrib);
	}

	void AddFunction(FunctionDefinition* func)
	{
		functions.push_back(func);
	}
};

class FunctionParameter:public ASTNode
{
public:
	string name;
	string type;

	void SetName(string name)
	{
		this->name = name;
	}

	void SetType(string type)
	{
		this->type = type;
	}
};

class FunctionDefinition:public Statement
{
public:
	vector<FunctionParameter*> params;
	vector<Statement*> statements;
	//Block *block;
	string name;
	string returnType;
	bool isStatic;
	bool isConstructor;

	FunctionDefinition()
	{
		type = ASTNode::FunctionDef;
	}

	void SetName(string name)
	{
		this->name = name;
	}

	void SetStatic(bool stat)
	{
		isStatic = stat;
	}

	void SetReturnType(string type)
	{
		this->returnType = type;
	}

	void AddParam(FunctionParameter* param)
	{
		params.push_back(param);
	}

	void AddStatement(Statement *stmt)
	{
		//block->AddStatement(stmt);
		statements.push_back(stmt);
	}
};

class Expression;

class ExpressionStatement:public Statement
{
public:
	Expression *expr;

	ExpressionStatement(Expression *e)
	{
		expr = e;

		type = ASTNode::ExprStmt;
	}
};

class Expression:public ASTNode
{
public:

};

class Identifier:public Expression
{
public:
	string name;

	Identifier(string name)
	{
		this->name = name;
		type = ASTNode::Iden;
	}
};

class BinaryExpression:public Expression
{
public:
	//left and right children nodes
	Expression *left;
	Expression *right;
	Token::Type op;

	BinaryExpression(Token::Type op,Expression *left,Expression *right)
	{
		this->left = left;
		this->right = right;
		this->op = op;

		type = ASTNode::BinaryExpr;
	}
};

class NumberLiteral:public Expression
{
public:
	float value;

	NumberLiteral(float val)
	{
		value = val;
		type = ASTNode::NumberLiteral;
	}

	NumberLiteral(string val)
	{
		value = (float)atof(val.c_str());
		type = ASTNode::NumberLiteral;
	}
};

class StringLiteral:public Expression
{
public:
	std::string value;

	StringLiteral(std::string val)
	{
		value = val;
		type = ASTNode::StringLiteral;
	}
};

class BoolLiteral:public Expression
{
public:
	bool value;

	BoolLiteral(bool val)
	{
		value = val;
		type = ASTNode::BoolLiteral;
	}
};

class NullLiteral:public Expression
{
public:

	NullLiteral()
	{
		type = ASTNode::NullLiteral;
	}
};

class NegExpr:public Expression
{
public:
	Expression* child;
	NegExpr()
	{
		type = ASTNode::Neg;
	}
};

class PropertyAccess:public Expression
{
public:
	Expression *obj;
	string name;

	PropertyAccess(Expression *lhs,string rhs)
	{
		obj = lhs;
		name = rhs;

		type = ASTNode::PropAccess;
	}
};

class Arguments;

class CallExpr:public Expression
{
public:
	Expression *obj;
	Arguments* args;

	CallExpr(Expression* lhs,Arguments* rhs)
	{
		obj = lhs;
		args = rhs;

		type = ASTNode::FunctionCall;
	}
};

class NewExpr:public Expression
{
public:
	string name;
	Arguments* args;

	NewExpr(string n,Arguments* rhs)
	{
		name = n;
		args = rhs;

		type = ASTNode::New;
	}
};

class VarExpr:public Expression
{
public:
	string name;
	string typeName;

	VarExpr(string n,string t)
	{
		name = n;
		typeName = t;

		type = ASTNode::Var;
	}
};

class Arguments:public Expression
{
public:
	vector<Expression*> args;

	void AddArg(Expression* expr)
	{
		args.push_back(expr);
	}
};

class IfStatement:public Statement
{
public:
	Expression *expr;
	Block *block;
	Statement* elseStmt;

	IfStatement(Expression *e,Block *b)
	{
		expr = e;
		block = b;

		elseStmt = NULL;
		type = ASTNode::IfStmt;
	}
};

class WhileStatement:public Statement
{
public:
	Expression* expr;
	Block* block;

	WhileStatement(Expression* e,Block* b)
	{
		type = ASTNode::WhileStmt;
		expr = e;
		block = b;
	}
};

class ReturnStatement:public Statement
{
public:
	Expression* expr;

	ReturnStatement()
	{
		type = ASTNode::ReturnStmt;
	}
};

class EnumStatement:public Statement
{
public:
	string name;
	vector<string> values;

	EnumStatement(string n)
	{
		name = n;
		type = ASTNode::Enum;
	}

	void AddValue(string val)
	{
		values.push_back(val);
	}
};


class EmptyStatement:public Statement
{

};

}