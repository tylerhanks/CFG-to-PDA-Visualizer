#include "Grammar.h"

// Arg1: path to input file
int main(int argc, char *argv[])
{
	// Check for all args present
	if (argc < 2)
	{
		std::cout << "Usage: 'program' <pathToInputFile>" << std::endl;
		std::getchar();
		return 0;
	}

	// Begin
	try
	{
		Grammar test_grammar(argv[1]);
		test_grammar.printGrammar();
	}
	catch (std::exception& error)
	{
		std::cout << "Error occurred: " << error.what();
	}
	std::getchar();

	return 0;
}