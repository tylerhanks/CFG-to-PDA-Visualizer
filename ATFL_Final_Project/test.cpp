#include "Grammar.h"

// Arg1: path to input file
int main(int argc, char *argv[])
{
	// Check for all args present
	if (argc < 2)
	{
		std::cout << "Usage: 'program' <pathToInputFile>" << std::endl;
		return 0;
	}
	// Begin
	try
	{
		Grammar test_grammar(argv[1]);
		std::cout << "Input:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.printGrammar();

		std::getchar();

		std::cout << "Eliminate unreachable symbols:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.elimUnreachable();
		test_grammar.printGrammar();

		std::getchar();

		std::cout << "Eliminate nonterminating productions and symbols:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.elimNonterminating();
		test_grammar.printGrammar();

		std::getchar();

		std::cout << "Eliminate lambda productions:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.elimLambda();
		test_grammar.printGrammar();

		std::getchar();

		std::cout << "Eliminate unit productions:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.elimUnit();
		test_grammar.printGrammar();

		std::getchar();

		std::cout << "Eliminate redundant symbols:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.elimRedundant();
		test_grammar.printGrammar();

		std::getchar();

		std::cout << "Eliminate left recursion:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.elimLeftRecursion();
		test_grammar.printGrammar();

		std::getchar();

		std::cout << "Greibach Normal Form:" << std::endl << "---------------------------------" << std::endl;
		test_grammar.convertToGNF();
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