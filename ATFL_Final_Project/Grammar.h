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
/* Productions in input file must be of the form S -> aBc | DEf ...     */
/************************************************************************/
/// <summary>
/// Used to store and manipulate context-free grammars.
/// Productions in input file must be of the form S -> aBc | DEf ...
/// </summary>
class Grammar
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="Grammar"/> class.
	/// </summary>
	/// <param name="filename">The name of the file to read as input.</param>
	Grammar(std::string filename);

	// Functions to operate on grammars
	// Mutates original grammar

	/// <summary>
	/// Eliminates all unreachable production.
	/// </summary>
	void elimUnreachable();

	/// <summary>
	/// Eliminates all nonterminating productions.
	/// </summary>
	void elimNonterminating();

	/// <summary>
	/// Eliminates all lambda productions.
	/// </summary>
	void elimLambda();

	/// <summary>
	/// Eliminates all unit productions.
	/// </summary>
	void elimUnit();

	/// <summary>
	/// Eliminates all redundant productions.
	/// </summary>
	void elimRedundant();

	/// <summary>
	/// Eliminates left recursion from the grammar.
	/// </summary>
	void elimLeftRecursion();

	/// <summary>
	/// Converts the current production rules to correct GNF formatting.
	/// </summary>
	/// <returns></returns>
	int convertToGNF();

	/// <summary>
	/// Prints the transition functions in PDA format.
	/// </summary>
	/// <returns>
	///   <c>1</c> if the transitions could be printed; otherwise <c>0</c>
	/// </returns>
	int printTransitionFunctions();

	/// <summary>
	/// Prints the grammar.
	/// </summary>
	void printGrammar();

private:

	std::unordered_map<std::string, int> non_terminals;
	std::unordered_map<std::string, std::unordered_set<std::string>> productions;
	std::string filename;
	std::ifstream infile;


	/// <summary>
	/// Determines whether the specified char is non terminal.
	/// </summary>
	/// <param name="ch">The char to test.</param>
	/// <returns>
	///   <c>true</c> if the specified char is non terminal ; otherwise, <c>false</c>.
	/// </returns>
	bool isNonTerminal(char ch);


	/// <summary>
	/// Gets all possible combinations of deletions of the specified symbol from the specified rule.
	/// </summary>
	/// <param name="symbolToRemove">The symbol to remove.</param>
	/// <param name="rule">The rule to from symbols from.</param>
	/// <returns>
	/// A vector of all possible rules resulting from deletions of the symbol.
	/// </returns>
	std::vector<std::string> getCombos(char symbolToRemove, std::string rule);


	/// <summary>
	/// Helper function of getCombos. Outputs all possible combinations involved in deleting specified locations of the input string.
	/// </summary>
	/// <param name="len">The size of the window to use when deleting.</param>
	/// <param name="str">The string to delete from.</param>
	/// <param name="out">The container to output to.</param>
	/// <param name="pos">The positions of character to delete.</param>
	void getCombosRecur(int len, std::string str, std::vector<std::string> &out, std::vector<int> &pos);

	/// <summary>
	/// Converts the an <c>int</c> to a <c>string</c>.
	/// </summary>
	/// <param name="x">The integer to convert.</param>
	/// <returns>A string version of the integer.</returns>
	std::string convert_int_to_string(int x);
};
