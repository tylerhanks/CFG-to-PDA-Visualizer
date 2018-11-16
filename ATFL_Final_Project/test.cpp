#include "Grammar.h"

// Arg1: path to input file
int main(int argc, char *argv[])
{
	// Check for all args present
	if (argc < 2)
	{
		std::cout << "Usage: 'program' <pathToInputFile> [optional]<nameOfOutputFile>(defaults to PDA.txt)" << std::endl;
		return 0;
	}
	// Begin
	if (argc == 2)
	{
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
			int r = test_grammar.elimLeftRecursion();
			if (r == -1)
			{
				std::cout << "Failed to eliminate left recursion" << std::endl;
			}
			test_grammar.printGrammar();
			
			std::getchar();

			std::cout << "Greibach Normal Form:" << std::endl << "---------------------------------" << std::endl;
			test_grammar.convertToGNF();
			test_grammar.printGrammar();

			std::getchar();
			
			std::cout << "Print PDA:" << std::endl << "---------------------------------" << std::endl;
			test_grammar.printTransitionFunctions();

			std::getchar();

			std::cout << "Print PDA to file:" << std::endl << "---------------------------------" << std::endl;
			test_grammar.printTransitionFunctions("PDA.txt");
		}
		catch (std::exception& error)
		{
			std::cout << "Error occurred: " << error.what();
		}
		return 0;
	}
	else
	{
		try
		{
			Grammar test_grammar(argv[1]);
			test_grammar.elimUnreachable();
			test_grammar.elimNonterminating();
			test_grammar.elimLambda();
			test_grammar.elimUnit();
			test_grammar.elimRedundant();
			test_grammar.elimLeftRecursion();
			test_grammar.convertToGNF();
			test_grammar.printTransitionFunctions(argv[2]);
		}
		catch (std::exception& error)
		{
			std::cout << "Error occurred: " << error.what();
			return -1;
		}
		return 0;
	}
}