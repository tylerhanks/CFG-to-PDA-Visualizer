#include "Grammar.h"

Grammar::Grammar(std::string filename)
{
	this->filename = filename;
	infile.open(filename, std::ios::in);
	std::string line, non_terminal, consumer;
	std::vector<std::vector<std::string>> rules;
	std::vector<std::string> rule;

	while (!infile.eof())
	{
		rule.clear();
		rules.clear();
		std::getline(infile, line);
		std::istringstream iss(line);

		//read the lhs non terminal
		iss >> non_terminal;
		non_terminals.insert({ non_terminal, 0 });

		//nom the arrow
		iss >> consumer;

		//add the rest of the production to a vector
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

		//add the productions to the productions map
		productions.insert({ non_terminal, rules });
	}

	//this code will only be useful if we have to convert CFGs into GNF
	int nt_count = 0;
	int nt_size = non_terminals.size();
	for (auto iter = productions.begin(); iter != productions.end(); iter++)
	{
		if (non_terminals[iter->first] == 0)
		{
			non_terminals[iter->first] = nt_count+1;
			nt_count++;
		}
		if (nt_count == nt_size)
			return;
		for (int i = 0; i < iter->second.size(); i++)
			for (int j = 0; j < iter->second[i].size(); j++)
			{
				if (non_terminals.count(iter->second[i][j]) && non_terminals[iter->second[i][j]] == 0)
				{
					non_terminals[iter->second[i][j]] = nt_count+1;
					nt_count++;
				}
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
