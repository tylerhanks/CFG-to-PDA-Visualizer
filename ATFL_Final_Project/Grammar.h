#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <fstream>

class Grammar
{
public:
	/*Constructor
	/*Makes a grammar based on the input file*/
	Grammar(std::string filename);

	/*Functions to operate on grammars
	/*Mutates original grammar*/
	void convertToCNF(); //converts grammar to Chomsky normal form
	void convertToGNF(); //converts grammar to Griebach normal form


	/*Accessors*/
	void printGrammar(); //prints grammar to the terminal

private:
	inline void cleanUp() { elimLambda(); elimUnit(); } //eliminates lambda and unit productions
	void elimLambda(); //eliminates lambda productions
	void elimUnit(); //eliminates unit productions
	bool isCNF();
	bool isGNF();
	std::unordered_map<std::string, int> non_terminals;
	std::unordered_map<std::string, std::vector<std::vector<std::string>>> productions;
	std::string filename;
	std::ifstream infile;
};
