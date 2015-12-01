#include "stdafx.h"
#include "analysis.h"

int main(int argc, const char* argv[])
{
	/*if (argc != 2)
	{
		std::cout << "file name expected";
		std::exit(EXIT_FAILURE);
	}*/
	setlocale(LC_ALL, "Russian");
	Parser Interpretator("code.txt"/*argv[1]*/);	//имя файла
	try {
		Interpretator.gc();
		Interpretator.getNextLexeme();
		Interpretator.PARSE_PROGRAM();
	}
	catch (const char* err) {
		std::cout << "Error parsing: " << err 
			<< " in line #" << Interpretator.line_num << std::endl;
		system("Pause");
		return 1;
	}
	catch (const std::ifstream::failure&) {
		//if there is no end-of-code indicatore before eof then smth gone wrong
		if (Interpretator.currentChar != '.')
		{
			std::cout << "Unexpected end of file\n";
			system("Pause");
			return 1;
		}
		else
			std::cout << "Expression parsed, calculating...\n";
	}
	catch (...) {
		std::cout << "Undefined exception while parsing thrown.\n"
			<< "Waiting for input to exit the program";
		std::cin.get();
		std::exit(EXIT_FAILURE);
	}

	//Вычисление
	try {
		Interpretator.calculate();
		std::cout << "Calculated\n";
	}
	catch (const char* err) {
		std::cout << "Error calculating: " << err << std::endl;
		system("Pause");
		return 1;
	}
	catch (...) {
		std::cout << "Undefined exception while calculating thrown.\n"
			<< "Waiting for input to exit the program";
		std::cin.get();
		std::exit(EXIT_FAILURE);
	}

	system("Pause");
	return 0;
}
