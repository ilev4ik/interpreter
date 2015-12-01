#include "stdafx.h"

enum LexType {
	LEX_NULL,			//для ошибок и нераспознанных лексем +
	LEX_VAR_TYPE,
	LEX_NUMBER,			//числа +
	LEX_CONST_STR,		//константные строки типа
	LEX_CONST,	
	LEX_DELIM,			//логическое выражение +
	LEX_VAR,			//переменные
	LEX_KEYWORD,		//if, then, while +
	LEX_EOC				//конец кода
};

enum LexVarType {
	LEX_VT_NULL,
	LEX_VT_BOOL,
	LEX_VT_INT,
	LEX_VT_DOUBLE
};

const char* LEX_VAR_TYPES [] = {
	"",
	"bool",
	"int",
	"double",
	0
};

enum LexConst {
	LEX_CONST_NULL,
	LEX_CONST_TRUE,
	LEX_CONST_FALSE
};

const char* LEX_CONSTS [] = {
	"",
	"true",
	"false",
	0
};

enum LexKeyword {
	LEX_KW_NULL,
	LEX_KW_PROG,
	LEX_KW_GETVAR,
	LEX_KW_BEGIN,
	LEX_KW_END,
	LEX_KW_IF,
	LEX_KW_THEN,
	LEX_KW_ELSE,
	LEX_KW_WHILE,
	LEX_KW_DO,
	LEX_KW_WRITE,
	LEX_KW_READ,
	LEX_KW_ARR,
	LEX_KW_OF,
	LEX_KW_OR,
	LEX_KW_AND,
	LEX_KW_NOT,
	LEX_KW_MOD,
	LEX_KW_DIV
};

const char* LEX_KEYWORDS [] = {
	"",
	"program",
	"var",
	"begin",
	"end",
	"if",
	"then",
	"else",
	"while",
	"do",
	"write",
	"read",
	"array",
	"of",
	//////
	"or",
	"and",
	"not",
	"mod",
	"div",
	0
};

enum LexDelims {
	LEX_DEL_NULL,
	LEX_DEL_ADD,
	LEX_DEL_MIN,
	LEX_DEL_MUL,
	LEX_DEL_DIV,
	LEX_DEL_BROPEN,		//круглые скобки
	LEX_DEL_BRCLOSE,
	LEX_DEL_LESS,
	LEX_DEL_ELESS,
	LEX_DEL_MORE,
	LEX_DEL_EMORE,
	LEX_DEL_EQUAL,
	LEX_DEL_NEQUAL,
	LEX_DEL_COM,		//запятая
	LEX_DEL_SEMICOLON,	//точка с запятой
	LEX_DEL_COLON,
	LEX_DEL_INIT
};

const char* LEX_DELIMS [] = {
	"",
	"+",
	"-",
	"*",
	"/",
	"(",
	")",
	"<",
	"<=",
	">",
	">=",
	"=",
	"<>",
	",",
	";",
	":",
	":=",
	0
};