#include "stdafx.h"
#include "analysis.h"
#include "enums.h"
#include <sstream>
#include <stack>
#include <vector>
#include <cmath>
#include <string.h>

//Конструкторы
Operation::Operation(int type_, double data_, int t = 0, std::string s = "")
{
	type = type_;
	data = data_;
	ValType = t;
	str = s;
}
Operation::Operation() {}
Lexeme::Lexeme(int type_ = LEX_NULL, int index_ = LEX_NULL, const std::string& buf_ = "", double value_ = 0, int LexType_ = 0)
{
	Lexeme::type = type_;
	Lexeme::index = index_;
	Lexeme::buf = buf_;
	Lexeme::value = value_;
	Lexeme::LexType = LexType_;
}
Parser::Parser(const char fname[])
{
	try {
		ifs = new std::ifstream;
	}
	catch (std::bad_alloc) {
		std::cerr << "Error while allocating memory for input file stream\n";
		std::exit(EXIT_FAILURE);
	}
	ifs->exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
	try {
		ifs->open(fname, std::ifstream::in);
		std::cout << "File " << fname << " has been correctly opened\n";
	}
	catch (const std::ifstream::failure& ferr) {
		std::cerr << "Exception opening/reading " << fname
			<< "\nError: " << ferr.what();
		std::exit(EXIT_FAILURE);
	}

	line_num = 1;
	readFile = true;
	varStart = countOfVars = 0;
}
Parser::~Parser()
{
	try {
		ifs->close();
		std::cout << "file has been correctly closed\n";
	}
	catch(const std::ifstream::failure& ferr) {
		std::cerr << "Exception closing the opened file"
			<< "\nError: " << ferr.what();
		std::cin.get();
		std::exit(EXIT_FAILURE);
	}
	delete this->ifs;
}
void Parser::setType(const int start, const int count, const int type)
{
	for (int i = start; i < start + count; ++i)
		this->variables[i].setVarType(type);
}
Variable::Variable(const std::string& name, const int countOfVars = 1): varName(name)
{
	this->varType = LEX_VT_NULL;
}

//Вспомогательные функции
void Parser::gc() throw (const std::ifstream::failure&)
{
	if (readFile)
		this->ifs->get(currentChar);
	else
		std::cin.get(currentChar);
}
int Parser::find(const char* buf, const char* list[])
{
	int i = 0;
	while (list[i] != 0)
	{
		if (strcmp(list[i], buf) == 0)
			return i;
		i++;
	}
	return 0;
}
int Parser::findVariable(const std::string& var)
{
    for (int i = 0; i < (int)variables.size(); ++i)
        if (variables[i].varName == var)
            return i;
        
    return -1;
}
void Variable::setVarType(const int type_)
{
	varType = type_;
}
bool Parser::isBetta(char symb)
{
	if (65 <= symb && symb <= 90 || 97 <= symb && symb <= 122)
		return true;
	return false;
}
bool Parser::isDigit(char dig)
{
	if (48 <= dig && dig <= 57)
		return true;
	return false;
}
void Parser::checkWithReserved(const std::string& var_name)
{	
	for (unsigned int i = 0; i < variables.size(); ++i)
	{
		if	(
			variables[i].varName == var_name || 
			LEX_VAR_TYPES[i] == var_name ||
			LEX_CONSTS[i] == var_name ||
			LEX_KEYWORDS[i] == var_name ||
			LEX_DELIMS[i] == var_name
			)
		{
			throw "Multiple definition of variable";
		}
	}
}

//Чтение и разбиение на лексемы
void Parser::getNextLexeme()
{
	currentLex = readNextLexeme();
}
Lexeme Parser::readNextLexeme()
{
	enum State {Start, Less, More, Word, Any};
	State currentState = Start;
	std::string buf;

	while (true)
	{
		switch (currentState)
		{
			case Start:
			{
				if	(
					currentChar == ' ' ||
					currentChar == '\t'||
					currentChar == '\n'
					)
				{
					if (currentChar == '\n')
						this->line_num++;
					gc();
					currentState = Start;
				}
				else if (currentChar == '"') //строки
				{
					gc();
					currentState = Any;
				}
				else if (isDigit(currentChar))
				{
					bool floatMode = false;
					int intPart = 0;
					double floatPart = 0.0;
					const double floatMul = 0.1;
					int	p = 1, dotCount = 0, digCount= 0;

					while (true)
					{
						if (isDigit(currentChar))
						{
							digCount++;
							if (!floatMode)
							{
								intPart = 10*intPart + (int)currentChar - (int)'0';
								gc();
							}
							else
							{
								floatPart = floatPart + ((int)currentChar - (int)'0')*std::pow(floatMul, p++);
								gc();
							}
						}
						else if (currentChar == '.')
						{
							floatMode = true;
							if (++dotCount > 1)
								throw "Invalid input in entered number";
							gc();
						}
						else break;
					}
					std::stringstream ss;
					ss << intPart + floatPart;
					
					if (digCount == 0)
						throw "no digit in number found";

					if (floatMode)
						return Lexeme(LEX_NUMBER, LEX_NULL, ss.str(), intPart + floatPart, LEX_VT_DOUBLE);
					else
						return Lexeme(LEX_NUMBER, LEX_NULL, ss.str(), intPart, LEX_VT_INT);
				}
				else if (isBetta(currentChar) || currentChar == '_')
				{
					buf += currentChar;
					gc();
					currentState = Word;
				}
				else if (currentChar == '<')
				{
					buf += currentChar;
					gc();
					currentState = Less;
				}
				else if (currentChar == '>')
				{
					buf += currentChar;
					gc();
					currentState = More;
				}
				else if (currentChar == '=')
				{
					buf += currentChar;
					gc();
					return Lexeme(LEX_DELIM, LEX_DEL_EQUAL, buf);
				}
				/*else if (currentChar == '|')
				{
					buf += currentChar;
					gc();
					return Lexeme(LEX_DELIM, LEX_DEL_OR, buf);
				}
				else if (currentChar == '&')
				{
					buf += currentChar;
					gc();
					return Lexeme(LEX_DELIM, LEX_DEL_AND, buf);
				}
				else if (currentChar == '!')
				{
					buf += currentChar;
					gc();
					return Lexeme(LEX_DELIM, LEX_DEL_NOT, buf);
				}*/
				else if (currentChar == '.')	//end of code
				{
					buf += currentChar;
					gc();
					return Lexeme(LEX_EOC, LEX_NULL, buf);
				}
				else
				{
					buf += currentChar;
					gc();
					if (currentChar == '=')
					{
						buf += currentChar;
						gc();
					}
					int index = find(buf.c_str(), LEX_DELIMS);
					return Lexeme(LEX_DELIM, index, buf);
				}
				break;
			}
			case Less:
			{
				if (currentChar == '=')
				{
					buf += currentChar;
					gc();
					return Lexeme(LEX_DELIM, LEX_DEL_ELESS, buf);
				}
				else if (currentChar == '>')
				{
					buf += currentChar;
					gc();
					return Lexeme(LEX_DELIM, LEX_DEL_NEQUAL, buf);
				}
				else
				{
					return Lexeme(LEX_DELIM, LEX_DEL_LESS, buf);
				}
				break;
			}
			case More:
			{
				if (currentChar == '=')
				{
					buf += currentChar;
					gc();
					return Lexeme(LEX_DELIM, LEX_DEL_EMORE, buf);
				}
				else
				{
					return Lexeme(LEX_DELIM, LEX_DEL_MORE, buf);
				}
				break;
			}
			case Any:
			{
				if (currentChar != '"')
				{
					buf += currentChar;
					gc();
					currentState = Any;
				}
				else 
				{
					gc();
					return Lexeme(LEX_CONST_STR, LEX_NULL, buf);
				}
				break;
			}
			case Word:
			{
				if (isBetta(currentChar) || isDigit(currentChar))
 				{
					buf += currentChar;
					gc();
					currentState = Word;
				}
				else
				{
					int index = find(buf.c_str(), LEX_CONSTS);
					if (index == 0)
					{
						index = find(buf.c_str(), LEX_KEYWORDS);
						if (index == 0)
						{
							index = find(buf.c_str(), LEX_VAR_TYPES);
							if (index == 0)
							{
								return Lexeme(LEX_VAR, LEX_NULL, buf);
							}
							else 
								return Lexeme(LEX_VAR_TYPE, index, buf);
						}
						else 
							return Lexeme(LEX_KEYWORD, index, buf);
					}
					else 
						return Lexeme(LEX_CONST, index, buf, (double)(index == LEX_CONST_TRUE), LEX_VT_BOOL);
					break;
				} //else in case Word
			} //casa W
		} //switch
	} //while
} //func

//Логические и арифметические выражения
Parser::ExpType Parser::Number()
{
    if (currentLex.type == LEX_NUMBER)
	{
		program.push_back(Operation(Operation::CONST, currentLex.value, currentLex.LexType));
        
        getNextLexeme();
        
        return EXP_NUM;
    }
	else if (currentLex.type == LEX_CONST_STR)	//"строка"
	{
		// минус единицей обозначим строку
		this->program.push_back(Operation(Operation::CONST, -1, 0, currentLex.buf));
		getNextLexeme();
		return EXP_STR;
	}
    else if (currentLex.type == LEX_VAR)
	{
        int varIndex = findVariable(currentLex.buf);
        if (varIndex < 0)
            throw "Unknown variable or using not initialised variable";
        
        program.push_back(Operation(Operation::VAR, varIndex));
        
        getNextLexeme();
		if	(variables[varIndex].varType == LEX_VT_BOOL)
			return EXP_LOG;

        return EXP_NUM;;
        
    }
    else if (currentLex.type == LEX_CONST)
	{
        program.push_back(Operation(Operation::CONST, currentLex.value, LEX_VT_BOOL));
        getNextLexeme();
        return EXP_LOG;
    }
    else if (
			currentLex.type == LEX_DELIM &&
			currentLex.index == LEX_DEL_MIN
			)
	{
        Lexeme operation = currentLex;
        
        getNextLexeme();
        
        ExpType argType = Number();
        
        if (argType != EXP_NUM)
            throw "Int expression required";
        
        program.push_back(Operation(Operation::UNARY, operation.index));
        return EXP_NUM;
    }
    else if (
			currentLex.type == LEX_DELIM &&
			currentLex.index == LEX_DEL_BROPEN
			)
	{
        getNextLexeme();

        ExpType r = Logic();

        if	(
			currentLex.type != LEX_DELIM || 
			currentLex.index != LEX_DEL_BRCLOSE
			)
            throw ") needed";

        getNextLexeme();
        return r;
    }
}
Parser::ExpType Parser::Mul()
{
    ExpType left = Number();
    ExpType ans = left;
    while	(
				(
					currentLex.type == LEX_KEYWORD &&
					(
						currentLex.index == LEX_KW_DIV ||	//div
						currentLex.index == LEX_KW_MOD ||	//mod
						currentLex.index == LEX_KW_AND		//and
					)
					||
					(
					currentLex.type == LEX_DELIM &&
						(
						currentLex.index == LEX_DEL_DIV ||	// /
						currentLex.index == LEX_DEL_MUL		// *
						)
					)
				)
			)
	{
        Lexeme op = currentLex;
        getNextLexeme();
        ExpType right = Number();

        if (left != right) throw "Type mismatch";

        if (op.type == LEX_DELIM)
		{
            if (left != EXP_NUM) throw "Integer expected";
        }
		else 
		{
            if (op.index == LEX_KW_AND) 
			{
                if (left != EXP_LOG) throw "Bool expected";
            }
			else
			{
                if (left != EXP_NUM) throw "Integer expected";
            }
        }
        ans = right;
        if (op.type == LEX_DELIM)
            program.push_back(Operation(Operation::BINARY,op.index));
        else
            program.push_back(Operation(Operation::BINARY,op.index));
    }
    return ans;
}
Parser::ExpType Parser::Sum()
{
    ExpType left = Mul();
    ExpType ans = left;
    while	(
				(
				currentLex.type == LEX_DELIM &&
					(
					currentLex.index == LEX_DEL_ADD ||
					currentLex.index == LEX_DEL_MIN
					)
				)
				||
				(
				currentLex.type == LEX_KEYWORD &&	//or
				currentLex.index == LEX_KW_OR
				)
			)
	{
        Lexeme operation = currentLex;
        getNextLexeme();
        ExpType right = Mul();

        if (left != right)
            throw "Type mismatch";

        if	(
			operation.type == LEX_KEYWORD &&
			operation.index == LEX_KW_OR
			)
		{
            if (left != EXP_LOG)
                throw "Bool expected";
        }
		else
		{
            if (left != EXP_NUM)
                throw "Integer expected";
        }
        ans = right;
        if (operation.type == LEX_DELIM)
            program.push_back(Operation(Operation::BINARY, operation.index));
        else 
			program.push_back(Operation(Operation::BINARY, operation.index));
        
    }
    return ans;
}
Parser::ExpType Parser::Compare()
{
    ExpType left = Sum();
    if	(
			currentLex.type == LEX_DELIM &&
			(
				currentLex.index == LEX_DEL_MORE ||
				currentLex.index == LEX_DEL_LESS ||
				currentLex.index == LEX_DEL_NEQUAL ||
				currentLex.index == LEX_DEL_ELESS ||
				currentLex.index == LEX_DEL_EMORE ||
				currentLex.index == LEX_DEL_EQUAL
			)
		)
	{
        Lexeme op = currentLex;
        getNextLexeme();
        ExpType right = Compare();

        if (left != right)
            throw "Type mismatch";

        if	(
			op.index == LEX_DEL_EMORE ||
			op.index == LEX_DEL_ELESS ||
			op.index == LEX_DEL_LESS ||
			op.index == LEX_DEL_MORE
			)
            if (left != EXP_NUM)
                throw "Integer expected";

        program.push_back(Operation(Operation::BINARY, op.index));
        return EXP_LOG;
    }
    return left;
}
Parser::ExpType Parser::Logic()
{
    if	(
		currentLex.type == LEX_KEYWORD &&	//not !
		currentLex.index == LEX_KW_NOT
		)
	{
		Lexeme operation = currentLex;
        getNextLexeme();
        ExpType r = Logic();

        if (r != EXP_LOG) 
            throw "Bool expression expected";

        program.push_back(Operation(Operation::UNARY, operation.index));
        return EXP_LOG;
    }
	else
	{
        ExpType left = Compare();
        return left;
    }
}

//Программа (наш язык)
void Parser::PARSE_PROGRAM()
{
	if	(
		currentLex.type == LEX_KEYWORD &&
		currentLex.index == LEX_KW_PROG
		)
	{
		getNextLexeme();
		VAR();
		//объявление переменных

		PARSE_BLOCK();
		//BEGIN...END

		if	(currentLex.type != LEX_EOC)
			throw "final . (dot) missed";
	}
	else throw "invalid program start"; 
}
void Parser::VAR()
{
	if	(
		currentLex.type == LEX_KEYWORD &&
		currentLex.index == LEX_KW_GETVAR
		)
	{
		getNextLexeme();
		PARSE_INIT();

		while	(
				currentLex.type == LEX_DELIM &&
				currentLex.index == LEX_DEL_COM
				)
		{
			getNextLexeme();
			
			PARSE_INIT();
		}

		if	(
			currentLex.type != LEX_DELIM ||
			currentLex.index != LEX_DEL_SEMICOLON
			)
			throw "; needed";

		getNextLexeme();
	}
}
void Parser::PARSE_INIT()
{
	if (currentLex.type == LEX_VAR)
	{
		checkWithReserved(currentLex.buf); //Проверка на совпадение имён с зарезервированными
		variables.push_back(Variable(currentLex.buf));
		countOfVars++;
		getNextLexeme();

		while	(
				currentLex.type == LEX_DELIM &&
				currentLex.index == LEX_DEL_COM
				)
		{
			getNextLexeme();

			if (currentLex.type != LEX_VAR)
				throw "variable not found";

			checkWithReserved(currentLex.buf);
			variables.push_back(Variable(currentLex.buf));
			countOfVars++;
			getNextLexeme();
		}

		if	(
			currentLex.type != LEX_DELIM ||
			currentLex.index != LEX_DEL_COLON
			)
			throw ": missed";

		getNextLexeme();

		if	(currentLex.type == LEX_VAR_TYPE)
		{
			getVarType();
		}
		else if	(
				currentLex.type == LEX_KEYWORD && //array
				currentLex.index == LEX_KW_ARR
				)
		{
			//Тут куч всего добавить ещё надо
			getNextLexeme();

			if	(
				currentLex.type != LEX_KEYWORD ||
				currentLex.index != LEX_KW_OF
				)
				throw "invalid array declaration";

			getNextLexeme();
			Sum();		//N - size of array

			getVarType();

		}

		getNextLexeme();
	}
}
void Parser::PARSE_BLOCK()
{
	if	(
		currentLex.type == LEX_KEYWORD &&
		currentLex.index == LEX_KW_BEGIN
		)
	{
		getNextLexeme();

		PARSE_EXPRESSION();

		while	(
				currentLex.type == LEX_DELIM &&
				currentLex.index == LEX_DEL_SEMICOLON
				)
		{
			getNextLexeme();
			PARSE_EXPRESSION();
		}

		if	(
			currentLex.type != LEX_KEYWORD ||
			currentLex.index != LEX_KW_END
			)
			throw "block has no finish marker";

		getNextLexeme();
	}
}
void Parser::PARSE_EXPRESSION()
{
	if (currentLex.type == LEX_VAR) //переменная
	{
		int varIndex = findVariable(currentLex.buf);
		if (varIndex < 0)
			throw "unknown variable";

		getNextLexeme();

		if	(
			currentLex.type != LEX_DELIM ||
			currentLex.index != LEX_DEL_INIT
			)
			throw "assignment (:=) expected";

		getNextLexeme();
		Sum();
		
		this->program.push_back(Operation(Operation::ASSIGN, varIndex));
	}
	else if	(	//if
			currentLex.type == LEX_KEYWORD &&
			currentLex.index == LEX_KW_IF
			)
	{
		getNextLexeme();

		ExpType ifType = Sum();
		if (ifType != EXP_LOG)
			throw "Invalid 'if' argument type";

		this->program.push_back(Operation(Operation::IF_JMP_FALSE, 0));
		int jumpPosIfFalse = this->program.size() - 1; //прыгаем отсюда

		if	(
			currentLex.type != LEX_KEYWORD ||
			currentLex.index != LEX_KW_THEN
			)
			throw "\"then\" needed after condition operator";

		getNextLexeme();
		PARSE_EXPRESSION();

		this->program.push_back(Operation(Operation::IF_JMP_TRUE, 0));
		int jumpPosIfTrue = this->program.size() - 1;

		this->program[jumpPosIfFalse].data = this->program.size(); //сюда (пропуск then, если false) и отсюда (если true)

		if	(	//else
			currentLex.type == LEX_KEYWORD &&
			currentLex.index == LEX_KW_ELSE
			)
		{
			getNextLexeme();
			PARSE_EXPRESSION();
		}
		this->program[jumpPosIfTrue].data = this->program.size();
	}
	else if	(	//while
			currentLex.type == LEX_KEYWORD &&
			currentLex.index == LEX_KW_WHILE
			)
	{
		getNextLexeme();
		int loopPos = this->program.size();
		ExpType whileType = Sum();

		if (whileType != EXP_LOG)
			throw "Invalid 'while' argument type";
		this->program.push_back(Operation(Operation::WHILE_JMP_FALSE, 0));
		int loopToEnd = program.size() - 1;

		if	(
			currentLex.type != LEX_KEYWORD ||
			currentLex.index != LEX_KW_DO
			)
			throw "\"do\" needed after loop operator";

		getNextLexeme();
		PARSE_EXPRESSION();

		this->program.push_back(Operation(Operation::WHILE_CONTINUE, 0));
		this->program[program.size() - 1].data = loopPos;

		this->program[loopToEnd].data = program.size();

	}	//write
	else if (
			currentLex.type == LEX_KEYWORD &&
			currentLex.index == LEX_KW_WRITE
			)
	{
		getNextLexeme();
		int toPrint = 0;

		if	(
			currentLex.type != LEX_DELIM ||
			currentLex.index != LEX_DEL_BROPEN
			)
			throw "opennig brace '(' needed";
		getNextLexeme();
		if	(
			currentLex.type == LEX_NUMBER ||
			currentLex.type == LEX_CONST_STR ||
			currentLex.type == LEX_VAR ||
				(
				currentLex.type == LEX_DELIM &&
					(
					currentLex.index == LEX_DEL_MIN ||
					currentLex.index == LEX_DEL_ADD
					)
				)
			)
		{
			if	(
				currentLex.type == LEX_DELIM &&
				currentLex.index == LEX_DEL_ADD
				)
				getNextLexeme();

			Sum();
			toPrint++;

			if	(
				currentLex.type != LEX_DELIM ||
				currentLex.index != LEX_DEL_BRCLOSE
				)
				throw "closing brace ')' missed";

			getNextLexeme();

			this->program.push_back(Operation(Operation::WRITE, 0, toPrint));
		}
		else throw "Invalid print operator's first argument type";
	}
	else if	(
			currentLex.type == LEX_KEYWORD &&
			currentLex.index == LEX_KW_READ
			)
	{
		getNextLexeme();

		if	(
			currentLex.type != LEX_DELIM ||
			currentLex.index != LEX_DEL_BROPEN
			)
			throw "openning brace '(' missed";

		getNextLexeme();

		if	(currentLex.type != LEX_VAR)
			throw "Variable name in 'read' argument expected";

		int varIndex = findVariable(currentLex.buf);
		if (varIndex < 0)
			throw "Initialization of non-declared variable";

		this->program.push_back(Operation(Operation::READ, varIndex));

		getNextLexeme();

		if	(
			currentLex.type != LEX_DELIM ||
			currentLex.index != LEX_DEL_BRCLOSE
			)
			throw "closing brace ')' missed";

		getNextLexeme();
	}
	else PARSE_BLOCK();
}

//Для повторного обращения
void Parser::getVarType()
{
	if	(currentLex.index == LEX_VT_INT)
	{
		this->setType(varStart, countOfVars, LEX_VT_INT);
		varStart += countOfVars;
	}
	else if	(currentLex.index == LEX_VT_DOUBLE)
	{
		this->setType(varStart, countOfVars, LEX_VT_DOUBLE);
		varStart += countOfVars;
	}
	else if	(currentLex.index == LEX_VT_BOOL)
	{
		this->setType(varStart, countOfVars, LEX_VT_BOOL);
		varStart += countOfVars;
	}
	else throw "invalid variable type entered";
	countOfVars = 0;
}

//работа с ПОЛИЗом
void Parser::calculate()
{
    std::stack <Operation> stack;
    std::vector <Operation> values;
    
    values.resize(variables.size());

	unsigned int currOp = 0;
	while (currOp < program.size())
	{
        Operation op = program[currOp];
		int nextOp = currOp + 1;

        switch (op.type)
		{
            case Operation::CONST:
			{
                stack.push(op);
                break;
			}
			case Operation::BINARY:
            {
                Operation right = stack.top();
                stack.pop();
                
                Operation left = stack.top(); 
                stack.pop();
                
                switch ((int)op.data)	//тип операции
				{
                    case LEX_DEL_ADD:
                        stack.push(Operation(Operation::CONST, left.data + right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
                        break;
                    case LEX_DEL_MIN:
                        stack.push(Operation(Operation::CONST, left.data - right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
                        break;
                    case LEX_DEL_MUL:
                       stack.push(Operation(Operation::CONST, left.data * right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
                        break;
					case LEX_KW_DIV:
						if (!right.data)
							throw "division by zero detacted";
						if	(
							right.ValType != LEX_VT_INT ||
							left.ValType != LEX_VT_INT
							)
							throw "'div' requires integer operands";

						stack.push(Operation(Operation::CONST, (int)left.data / (int)right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
						break;
					case LEX_DEL_DIV:
						if (!right.data)
							throw "division by zero detacted";

						stack.push(Operation(Operation::CONST, (double)left.data / (double)right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
						break;
					case LEX_KW_MOD:
						if (right.data == 0)
							throw "'mod' division by zero detected";
						else
						{	
							if	(
								left.ValType != LEX_VT_INT ||
								right.ValType != LEX_VT_INT
								)
								throw "Invalid operand type while calculating operation 'mod'";

							stack.push(Operation(Operation::CONST, (int)left.data % (int)right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
						}
						break;
                    case LEX_DEL_LESS:
                        stack.push(Operation(Operation::CONST, left.data < right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
                        break;
                    case LEX_DEL_MORE:
                        stack.push(Operation(Operation::CONST, left.data > right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
                        break;
					case LEX_DEL_ELESS:
                        stack.push(Operation(Operation::CONST, left.data <= right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
                        break;
                    case LEX_DEL_EMORE:
                        stack.push(Operation(Operation::CONST, left.data >= right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
                        break;
					case LEX_DEL_EQUAL:
						stack.push(Operation(Operation::CONST, left.data == right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
						break;
					case LEX_DEL_NEQUAL:
						stack.push(Operation(Operation::CONST, left.data != right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
						break;
					case LEX_KW_AND:
						stack.push(Operation(Operation::CONST, left.data && right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
						break;
					case LEX_KW_OR:
						stack.push(Operation(Operation::CONST, left.data || right.data, (left.ValType < right.ValType) ? right.ValType : left.ValType));
						break;
                    default: throw "Unknown binary operation";
                }
                break;
            }
            case Operation::UNARY:
            {
				Operation arg = stack.top();
                stack.pop();

                switch ((int)op.data)
				{
                    case LEX_DEL_MIN:
                        stack.push(Operation(Operation::CONST, -arg.data, arg.ValType));
                        break;
					case LEX_KW_NOT:
						stack.push(Operation(Operation::CONST, !arg.data, arg.ValType));
						break;
                    default: throw "Unknown unary operation";
                }
                break;
            }
			case Operation::VAR:
			{
				if (values[(int)op.data].ValType == LEX_VT_NULL)
					throw "using not initialised variable";
				stack.push(values[(int)op.data]);
				break;
			}
			case Operation::ASSIGN:
			{
				Operation val = stack.top();	//значение инициализации
				stack.pop();

				int index = (int)op.data;		//индекс переменной, куда присвоить

				if	(
					values[index].ValType == LEX_VT_INT &&
					val.ValType == LEX_VT_DOUBLE
					)
					throw "<int> := <double>, types mismatch";
				else if (values[index].ValType == LEX_VT_BOOL)
				{
					if (val.data != 0)
					{
						val.data = 1;	//true
						val.ValType = LEX_VT_BOOL;
						values[index] = val;
					}
					else
					{
						val.data = 0;	//false
						val.ValType = LEX_VT_BOOL;
						values[index] = val;
					}
				}
				else values[index] = val;
				break;
			}
			case Operation::IF_JMP_FALSE:
			{
				Operation val = stack.top();
				stack.pop();

				if (!val.data)
					nextOp = (int)op.data;
				break;
			}
			case Operation::IF_JMP_TRUE:
			{
				nextOp = (int)op.data;
				break;
			}
			case Operation::WHILE_JMP_FALSE:
			{
				Operation val = stack.top();
				stack.pop();

				if (!val.data)
					nextOp = (int)op.data;
				break;
			}
			case Operation::WHILE_CONTINUE:
			{
				nextOp = (int)op.data;
				break;
			}
			case Operation::WRITE:
			{
				std::string* str = new std::string[op.ValType];
				for (int i = op.ValType - 1; i >= 0; --i)
				{
					if (stack.top().data == -1)
						str[i] = stack.top().str;
					else
					{
						std::string buf;
						std::stringstream ss;
						ss << stack.top().data;
						ss >> buf;
						str[i] = buf;
					}
					stack.pop();
				}
				for (int i = 0; i < op.ValType; ++i)
					std::cout << str[i];
				std::cout << std::endl;
				break;
			}
			case Operation::READ:
			{
				std::string arg;
				//read from standart input stream
				readFile = false;
				gc();
				getNextLexeme();
				if	(
					currentLex.type != LEX_NUMBER &&
					currentLex.type != LEX_CONST
					)
					throw "Invalid type entered in 'read' argument";
				values[(int)op.data] = Operation(Operation::CONST, currentLex.value);
				values[(int)op.data].ValType = currentLex.LexType;
				break;
			}
            default: throw "Unknown operation type";
        }
		currOp = nextOp;
    }
    return;
}
