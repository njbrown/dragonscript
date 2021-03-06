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

#include "../include/loris/lexer.hpp"
#include <iostream>

using namespace loris;

/*********************
	TokenStream
*********************/

TokenStream::TokenStream()
{
	index=0;
}

void TokenStream::AddToken(Token t)
{
	tokens.push_back(t);
}

void TokenStream::AddToken(Token::Type type,string tok,int line)
{
	Token token;
	token.type = type;
	token.token = tok;
	token.line = line;

	AddToken(token);
}

Token TokenStream::NextToken()
{
	size_t size = tokens.size();
	if(index<size)
		return tokens[index++];

	index+=1;
	return Token::EOSToken();
}

Token::Type TokenStream::PeekTokenType(unsigned int look_ahead)
{
	if(tokens.size()<=index+look_ahead)
		return Token::EOS;

	return tokens[index+look_ahead].type;
}

Token TokenStream::PeekToken(unsigned int look_ahead)
{
	if(tokens.size()<=index+look_ahead)
	{
		//return line of previous token if there's any available
		if(index>0)
			return Token::EOSToken(tokens[index-1].line);
		else
			return Token::EOSToken(1);
	}

	return tokens[index+look_ahead];
}

bool TokenStream::HasMore()
{
	return index<tokens.size();
}

void TokenStream::Advance()
{
	index+=1;
}

/*********************
	CharStream
*********************/

CharStream::CharStream(string code)
{
	chars = code;
	index=0;
}

/*
returns EOF if out of bounds
*/
int CharStream::NextChar()
{
	if(index<chars.size())
		return chars[index++];

	index++;
	return EOF;	
}


void CharStream::Advance()
{
	index+=1;
}

/*
returns EOF if out of bounds
*/
int CharStream::PeekChar(unsigned int look_ahead)
{
	int size = (int)chars.size();
	int look =(index+look_ahead);

	if(look<size)
		return chars[index+look_ahead];

	return EOF;
}

bool CharStream::HasMore()
{
	return index<chars.size();
}


/**************************
		Lexer
***************************/
Lexer::Lexer()
{
	tokens=NULL;
	stream=NULL;
	line=1;
}


bool Lexer::Parse(string code)
{
	if(tokens)delete tokens;
	if(stream)delete stream;

	tokens = new TokenStream();
	stream = new CharStream(StripComments(code));

	line=1;

	while(stream->HasMore())
	{
		int chr=stream->PeekChar();
		
		if(IsWhiteSpace())
			ReadWhiteSpace();
		else if(IsIdentifier())
			ReadIdentifier();
		else if(IsNumber())
			ReadNumber();
		else{
			//single character tokens and symbols
			switch(chr)
			{
			case '+':
				//stream->Advance();
				//if(stream->PeekChar()=='=')
				//	AddToken(Token::AddAssign);
				//else
					AddToken(Token::Add);//,false);
				break;
			case '-':
				//stream->Advance();
				//if(stream->PeekChar()=='=')
				//	AddToken(Token::SubAssign);
				//else
					AddToken(Token::Sub);//,false);
				break;
			case '*':
				//stream->Advance();
				//if(stream->PeekChar()=='=')
				//	AddToken(Token::MulAssign);
				//else
					AddToken(Token::Mul);//,false);
				break;
			case '/':
				//stream->Advance();
				//if(stream->PeekChar()=='=')
				//	AddToken(Token::DivAssign);
				//else
					AddToken(Token::Div);//,false);
				break;
			case '(':
				AddToken(Token::OpenParen);
				break;
			case ')':
				AddToken(Token::CloseParen);
				break;
			case '{':
				AddToken(Token::OpenCurlyBrace);
				break;
			case '}':
				AddToken(Token::CloseCurlyBrace);
				break;
			case '[':
				AddToken(Token::OpenBracket);
				break;
			case ']':
				AddToken(Token::CloseBracket);
				break;
			case ',':
				AddToken(Token::Comma);
				break;
			case '.':
				AddToken(Token::Dot);
				break;
			case '=':
				stream->Advance();
				if(stream->PeekChar()=='=')
					AddToken(Token::EQ);
				else
					AddToken(Token::Assign,false);
				break;
			case '!':
				stream->Advance();
				if(stream->PeekChar()=='=')
					AddToken(Token::NEQ);
				else
					AddToken(Token::Not,false);
				break;
			case '<':
				stream->Advance();
				if(stream->PeekChar()=='=')
					AddToken(Token::LTE);
				else
					AddToken(Token::LT,false);
				break;
			case '>':
				stream->Advance();
				if(stream->PeekChar()=='=')
					AddToken(Token::GTE);
				else
					AddToken(Token::GT,false);
				break;
			case ':':
				AddToken(Token::Colon);
				break;
			case ';':
				AddToken(Token::SemiColon);
				break;
			case '\'':
				ReadSingleQuotedString();
				break;
			case '"':
				ReadDoubleQuotedString();
				break;
			default:
				error.code = Error::UNKOWN_CHAR;
				error.message = "unknown character ";
				error.message+=chr;
				return false;
			}
		}
	}

	return true;
}

void Lexer::AddToken(Token::Type type,bool advance)
{
	Token token;
	token.type = type;
	token.line=line;

	tokens->AddToken(token);

	if(advance)
		stream->Advance();
}

bool Lexer::IsWhiteSpace()
{
	int c = stream->PeekChar();
	return (c==' '||c=='\n'||c=='\t');
}

void Lexer::ReadWhiteSpace()
{
	int c;
	while(stream->HasMore())
	{
		c = stream->PeekChar();
		if(c=='\n')
			line++;
		if (c==' '||c=='\n'||c=='\t')
			stream->NextChar();
		else
			return;
	}
}

bool Lexer::IsIdentifier()
{
	int c = stream->PeekChar();
	return (c=='_'||isalpha(c));
}

void Lexer::ReadIdentifier()
{
	int c;
	string token;
	while(stream->HasMore())
	{
		c = stream->PeekChar();
		if (c=='_'||isalpha(c) || (token.size()>0 && isdigit(c)))
		{
			char chr = stream->NextChar();
			token+=chr;
		}
		else
			break;
	}
	Token::Type type = Token::Iden;

	//check for keywords
	if(token=="import")
		type = Token::Import;
	else if(token=="module")
		type = Token::Module;
	else if(token=="new")
		type = Token::New;
	else if(token=="class")
		type = Token::Class;
	else if(token=="function")
		type = Token::Function;
	else if(token=="def")
		type = Token::Def;
	else if(token=="extends")
		type = Token::Extends;
	else if(token=="static")
		type = Token::Static;
	else if(token=="return")
		type = Token::Return;
	else if(token=="var")
		type = Token::Var;
	else if(token=="enum")
		type = Token::Enum;
	else if(token=="if")
		type = Token::If;
	else if(token=="else")
		type = Token::Else;
	else if(token=="elif")
		type = Token::Elif;
	else if(token=="for")
		type = Token::For;
	else if(token=="while")
		type = Token::While;
	else if(token=="break")
		type = Token::Break;
	else if(token=="or")
		type = Token::Or;
	else if(token=="and")
		type = Token::And;
	/* literals */
	else if(token=="true")
		type = Token::True;
	else if(token=="false")
		type = Token::False;
	else if(token=="null")
		type = Token::Null;
	

	//todo: check for specific token types
	tokens->AddToken(type,token,line);
}

bool Lexer::IsNumber()
{
	return isdigit(stream->PeekChar())!=0;
}

void Lexer::ReadNumber()
{
	bool dot = false;//check if dot is reached
	string token;
	while(stream->HasMore())
	{
		char c=stream->PeekChar();

		if(isdigit(c))
		{
			stream->Advance();
		}
		else if(c=='.' && dot==false)
		{
			stream->Advance();
			dot=true;
		}
		else
		{
			break;
		}

		token+=c;
	}

	Token::Type type;
	if(dot)
		type = Token::Float;
	else
		type = Token::Integer;

	tokens->AddToken(type,token,line);
}

void Lexer::ReadSingleQuotedString()
{
	stream->Advance();// '

	Token token;
	token.line = line;
	token.type = Token::String;

	while(stream->HasMore())
	{
		int chr = stream->NextChar();
		if(chr=='\'')
		{
			//stream->Advance();
			break;
		}
		if(chr==EOF)
		{
			//error

			break;
		}
		token.token+=chr;
	}
	tokens->AddToken(token);
}

void Lexer::ReadDoubleQuotedString()
{
	stream->Advance();// "

	Token token;
	token.line = line;
	token.type = Token::String;

	while(stream->HasMore())
	{
		int chr = stream->NextChar();
		if(chr=='\"')
		{
			//stream->Advance();
			break;
		}
		if(chr==EOF)
		{
			//error

			break;
		}
		token.token+=chr;
	}
	tokens->AddToken(token);
}

string Lexer::StripComments(string code)
{
	char* res = new char[code.size()+1];
	int resSize = 0;//char limit of max(long)

	bool commentMode = false;

	for(int i=0;i<code.size();i++)
	{
		if(commentMode)
		{
			
			if(code[i]=='\n')
			{
				//end of comment
				commentMode = false;
				res[i] = '\n';
			}
			else
			{
				//replace with space
				res[i] = ' ';
			}

		}
		else
		{
			if(code[i]=='/' && ( i+1<code.size() && code[i+1]=='/')  )
			{
				//start of comment
				commentMode = true;
				res[i] = ' ';
			}
			else
			{
				//normal line
				res[i] = code[i];
			}
		}

	}

	res[code.size()]='\0';
	string result(res);
	delete res;

	//std::cout<<code.c_str()<<endl;
	//std::cout<<result.c_str()<<endl;

	return result;
}

Lexer::~Lexer()
{
	if(tokens)delete tokens;
	if(stream)delete stream;
}