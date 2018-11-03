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
		std::cout << "Input:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.printGrammar();
		std::cout << std::endl;

		std::cout << "Lambda Removal:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.elimLambda();
		test_grammar.printGrammar();
		std::cout << std::endl;


		std::getchar();

		std::cout << "Eliminate unreachable productions:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.elimUnreachable();
		test_grammar.printGrammar();

		std::getchar();

		std::cout << "Print PDA:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.printTransitionFunctions();
		std::getchar();
	}
	catch (std::exception& error)
	{
		std::cout << "Error occurred: " << error.what();
	}


	return 0;
}