#include "Grammar.h"

Grammar::Grammar(std::string filename): filename(filename)
{
	infile.open(filename, std::ios::in);

	// Check if file opened
	if (!infile)
	{
		// If not, throw exception
		throw std::runtime_error("Could not open file.");
	}

	std::string line, non_terminal, consumer;
	std::vector<std::vector<std::string>> rules;
	std::vector<std::string> rule;

	while (std::getline(infile, line))
	{
		rule.clear();
		rules.clear();
		std::istringstream iss(line);

		// Read the lhs non terminal
		iss >> non_terminal;
		non_terminals.insert({ non_terminal, 0 });

		// Nom the arrow
		iss >> consumer;

		// Add the rest of the production to a vector
		while (!iss.eof())
		{
			iss >> consumer;
			if (consumer == "|")
			{
				rules.push_back(rule);
				rule.clear();
			}
			else
			{
				rule.push_back(consumer);
			}
		}
		rules.push_back(rule);

		// Add the productions to the productions map
		productions.insert({ non_terminal, rules });
	}

	// This code will only be useful if we have to convert CFGs into GNF
	int nt_count = 0;
	int nt_size = non_terminals.size();
	for (auto iter = productions.begin(); iter != productions.end(); iter++)
	{
		if (non_terminals[iter->first] == 0)
			non_terminals[iter->first] = ++nt_count;

		if (nt_count == nt_size)
			return;

		for (int i = 0; i < iter->second.size(); i++)
			for (int j = 0; j < iter->second[i].size(); j++)
			{
				if (non_terminals.count(iter->second[i][j]) && non_terminals[iter->second[i][j]] == 0)
					non_terminals[iter->second[i][j]] = ++nt_count;

				if (nt_count == nt_size)
					return;
			}
	}
}

void Grammar::convertToCNF()
{
	cleanUp();

	//TODO: convert to CNF
}

void Grammar::convertToGNF()
{
	if (!isCNF())
		convertToCNF();

	//TODO: convert to GNF
}

void Grammar::printGrammar()
{
	for (auto iter = productions.begin(); iter != productions.end(); iter++)
	{
		std::cout << iter->first << " -> ";
		for (int i = 0; i < iter->second.size(); i++)
		{
			if (i > 0)
				std::cout << " | ";
			for (int j = 0; j < iter->second[i].size(); j++)
			{
				std::cout << iter->second[i][j] << " ";
			}
		}
		std::cout << "\n";
	}
}

void Grammar::elimLambda()
{
	//TODO: eliminate lambda productions from the grammar
}

void Grammar::elimUnit()
{
	//TODO: eliminate unit productions from the grammar
}

bool Grammar::isCNF()
{
	return false;
}

bool Grammar::isGNF()
{
	return true;
}
