#include "Grammar.h"
#include <unordered_set> // Used for BFS in ::elimUnreachable

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
	Grammar::cleanUp();

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
	// Loop through all production symbols
	for (auto iter = productions.begin(); iter != productions.end(); iter++)
	{
		std::cout << iter->first << " -> ";

		// Loop through all sets of rules for each production symbol
		for (int i = 0; i < iter->second.size(); ++i)
		{
			if (i > 0)
				std::cout << "| ";

			// Loop through each symbol in current production rule
			for (int j = 0; j < iter->second[i].size(); ++j)
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

void Grammar::elimUnreachable()
{
	//store reached variables; anything not in this vector at end of bfs is unreachable
	std::unordered_set<std::string> visited;
	visited.emplace("S");

	//breadth-first search
	std::queue<std::string> bfs;
	bfs.push("S");

	while (!bfs.empty())
	{
		std::string examine = bfs.front();
		auto iter = productions.find(examine);

		// Check if current symbol exists in map
		if (iter != productions.end())
		{
			auto currProduction = iter->second;

			// Look through that symbol's productions for nonterminals
			for (int i = 0; i < currProduction.size(); ++i)
			{
				for (int j = 0; j < currProduction[i].size(); ++j)
				{
					std::unordered_map<std::string, int>::const_iterator got = non_terminals.find(iter->second[i][j]);
					// If nonterminal found, check that it hasn't been encountered yet
					if (got != non_terminals.end())
					{
						// Check if encountered nonterminal is new
						auto reached = visited.find(got->first);

						// If nonterminal is new, add it to visited and add it to queue
						if (reached == visited.end())
						{
							bfs.push(got->first);
							visited.emplace(got->first);
						}
					}
				}
			}
		}

		bfs.pop();

	}

	// After bfs, remove unreachable productions from productions map
	// Loop through all production symbols
	for (auto iter = productions.begin(); iter != productions.end(); ++iter)
	{
		// Check if they were reached in previous step
		auto reached = visited.find(iter->first);
		if (reached == visited.end())
		{
			// If production symbol was not reached, remove it
			iter = productions.erase(iter);
		}
	}
}

void Grammar::elimNonterminating()
{
}

bool Grammar::isCNF()
{
	return false;
}

bool Grammar::isGNF()
{
	return true;
}

void Grammar::printTransitionFunctions()
{
	//# is lambda
	std::cout << "d(q0, #, z) = (q1, Sz)";
	std::getchar();

	// Loop through all production symbols
	for (auto iter = productions.begin(); iter != productions.end(); iter++)
	{
		// Loop through all rule sets of current production symbol
		for (int i = 0; i < iter->second.size(); i++)
		{
			// Loop through each symbol of current production rule
			for (int j = 0; j < iter->second[i].size(); j++)
			{
				if (iter->second[i].size() == 1)
					std::cout << "d(q1, " << iter->second[i][j] << ", " << iter->first << ") = (q1, #)";
				else
				{
					if (j == 0)
						std::cout << "d(q1, " << iter->second[i][j] << ", " << iter->first << ") = (q1, ";
					else
						std::cout << iter->second[i][j];

					if (j == (iter->second[i].size() - 1))
						std::cout << ")";
				}
			}

			// Output current production symbol and its rules for comparison
			std::cout << "\t" << iter->first << " -> ";
			for (int k = 0; k < iter->second[i].size(); k++)
				std::cout << iter->second[i][k] << " ";

			std::getchar();
		}
	}
	std::cout << "d(q1, #, z) = (q2, #)\n";
}