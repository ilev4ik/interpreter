#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <exception>

#pragma warning(disable: 4290)

#ifndef CLASS_H
#define CLASS_H

class Lexeme
{
public:
	int type;
	int index;
	std::string buf;
	double value;
	
	int LexType;

	Lexeme(int, int, const std::string&, double, int);
};

class Operation
{
public:
	int type;
	double data;
	std::string str;
	int ValType;

	enum OpType {
		CONST,
		BINARY,
		UNARY,
		ASSIGN,
		VAR,
		IF_JMP_FALSE,
		IF_JMP_TRUE,
		WHILE_JMP_FALSE,
		WHILE_CONTINUE,
		WRITE,
		READ
	};

	Operation();
	Operation(int, double, int, std::string);
};

class Variable
{
public:

	std::string varName;
	int varType;
	double value;

	void setVarType(const int);

	Variable(const std::string&, const int);
};

class Parser
{
public:
	Parser(const char*); 	//конструктор
	~Parser();				//деструктор

	//Данные
	std::ifstream* ifs;
	char currentChar;
	unsigned int line_num;
	int varStart, countOfVars;
	Lexeme currentLex;
	std::vector <Operation> program;
	std::vector <Variable>	variables;
	bool readFile;

	//Синтаксический анализ
	Lexeme readNextLexeme();

	int find(const char*, const char**);
	int findVariable(const std::string&);

	bool isBetta(char);
	bool isDigit(char);
	void gc() throw (const std::ifstream::failure&);
	void getNextLexeme();
	void setType(const int, const int, const int);
	void getVarType();
	void checkWithReserved(const std::string&);

	enum ExpType {
		EXP_LOG,
		EXP_NUM,
		EXP_STR
	};

	//Семантический разбор языка
	void PARSE_PROGRAM();
	void VAR();
	void PARSE_BLOCK();
	void PARSE_INIT();
	void PARSE_EXPRESSION();
	
	//Методы лексического анализатора
	//Разбор логических и арифметических выражений
	ExpType Number();
	ExpType Mul();
	ExpType Sum();
	ExpType Compare();
	ExpType Logic();

	//Вычисление ПОЛИЗа
	void calculate();
};

#endif