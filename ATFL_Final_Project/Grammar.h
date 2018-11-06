#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

/************************************************************************/
/* Grammar Class: used to store and manipulate context-free grammars    */
/* Productions in input file must be of the form S -> a b c | ...       */
/************************************************************************/
class Grammar
{
public:
	/*Constructor
	/*Makes a grammar based on the input file*/
	Grammar(std::string filename);

	/*Functions to operate on grammars
	/*Mutates original grammar*/
	void cleanUp(); //simplify productions
	void elimUnreachable(); //eliminates unreachable productions
	void elimNonterminating(); //eliminates nonterminating productions
	void elimLambda(); //eliminates lambda productions
	void elimUnit(); //eliminates unit productions
	void convertToCNF(); //converts grammar to Chomsky normal form
	void convertToGNF(); //converts grammar to Griebach normal form
	void printTransitionFunctions();

	/*Accessors*/
	void printGrammar(); //prints grammar to the terminal

private:

	bool isCNF();
	bool isGNF();
	std::unordered_map<std::string, int> non_terminals;
	std::unordered_map<std::string, std::unordered_set<std::string>> productions;
	std::string filename;
	std::ifstream infile;

	bool isNonTerminal(char ch);
	std::vector<std::string> getCombos(char symbolToRemove, std::string rule);
	void getCombosRecur(int len, std::string str, std::vector<std::string> &out, std::vector<int> &pos);
};
