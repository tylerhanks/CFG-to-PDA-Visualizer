#include "Grammar.h"
#include <unordered_set> // Used for BFS in ::elimUnreachable
#include <unordered_map>
#include <string>

Grammar::Grammar(std::string filename) : filename(filename)
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


bool Grammar::isTerminal(std::string ch)
{
	if (ch.length() > 1)
	{
		return false;
	}

	return (ch[0] >= 'A' && ch[0] <= 'Z');
}

void Grammar::getCombosRecur(int len, std::string str, std::vector<std::string> &out, std::vector<int> &pos)
{
	if (len < 1)
	{
		return;
	}

	// Loop through pos vector backwards
	for (int i = pos.size() - len; i >= 0; --i)
	{
		std::string input = str;

		// Loop through current length starting from outside loop end point
		for (int j = i + len - 1; j >= i; --j)
		{
			input.erase(input.begin() + pos[j]);
		}
		out.push_back(input);
	}

	getCombosRecur(len - 1, str, out, pos);
}

std::vector<std::string> Grammar::getCombos(std::string symbolToRemove, std::vector<std::string> &rule)
{
	std::string input;
	std::vector<int> positions;
	std::vector<std::string> output;

	// Put rule into string form
	for (int i = 0; i < rule.size(); ++i)
	{
		if (rule[i] == symbolToRemove)
		{
			positions.push_back(i);
		}

		input += rule[i];
	}

	getCombosRecur(positions.size(), input, output, positions);

	return output;
}

// TODO: Remove duplicate rules (possibly in dedicated function)
void Grammar::elimLambda()
{
	struct coord
	{
		int i;
		int j;
	};

	std::unordered_set<std::string> hasLambda;
	// Map -> Vector -> Pair(Symbol string, coord in production map vector)
	std::unordered_map < std::string, std::vector<std::pair<std::string, coord>>> references;

	// Loop through all production symbols
	for (auto iter = non_terminals.begin(); iter != non_terminals.end(); ++iter)
	{
		auto currVector = productions.at(iter->first);
		// Loop through all productions of current symbol
		for (int i = 0; i < currVector.size(); ++i)
		{
			// Loop through all char of current production
			for (int j = 0; j < currVector[i].size(); ++j)
			{
				// Keep track of referenced productions in case removal is necessary
				if (isTerminal(currVector[i][j]))
				{
					references[currVector[i][j]].push_back(std::make_pair(iter->first, coord{ i,j }));
				}

				// If lambda is found and production is not set to for lambda removal
				if (currVector[i][j] == "#" && currVector[i].size() == 1)
				{
					// Mark for work and remove lambda
					hasLambda.emplace(iter->first);
					productions.at(iter->first).erase(productions.at(iter->first).begin() + i);
				}
			}
		}
	}



	// Go through and remove lambdas
	for (auto iter = hasLambda.begin(); iter != hasLambda.end(); ++iter)
	{
		std::unordered_map<std::string, std::unordered_set<int>> prodRulesFinished;

		// If reference was found for current lambda
		auto curr = references.find(*iter); // This should always find it since it was put in in the previous step, so not sure if necessary
		if (curr != references.end())
		{
			// Loop through all references
			for (int i = 0; i < curr->second.size(); ++i)
			{
				auto currSet = prodRulesFinished.find(curr->second[i].first);
				if (currSet == prodRulesFinished.end() || currSet->second.find(curr->second[i].second.i) == currSet->second.end())
				{
					// Add this to map to ensure we don't repeat for the same nonterminal in the same rule later
					prodRulesFinished[curr->second[i].first].emplace(curr->second[i].second.i);

					std::cout << "After removing # from (" << *iter << ") => Added to (" << curr->second[i].first << ") : ";

					auto currVector = productions.find(curr->second[i].first)->second[curr->second[i].second.i];
					auto outputVector = getCombos(*iter, currVector);

					// Push string vector from above to the appropriate production vector
					for (int j = 0; j < outputVector.size(); ++j)
					{
						productions[curr->second[i].first].push_back(std::vector<std::string>());
						for (int k = 0; k < outputVector[j].size(); ++k)
						{
							// Push characters in the string to production vector
							productions[curr->second[i].first][productions[curr->second[i].first].size() - 1].push_back(std::string(1, outputVector[j][k]));
						}

						if (j > 0)
						{
							std::cout << " | ";
						}
						std::cout << outputVector[j];

					}
					std::cout << std::endl;
				}
			}
		}
	}
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

	// After bfs, remove unreachable symbols from both maps
	// Loop through all symbols
	for (auto iter = productions.begin(); iter != productions.end();)
	{
		// Check if they were reached in bfs
		auto reached = visited.find(iter->first);
		if (reached == visited.end())
		{
			// If symbol was not reached, remove it
			iter = productions.erase(iter);
		}
		else
			iter++;
	}
	for (auto iter = non_terminals.begin(); iter != non_terminals.end();)
	{
		// Check if they were reached in bfs
		auto reached = visited.find(iter->first);
		if (reached == visited.end())
		{
			// If symbol was not reached, remove it
			iter = non_terminals.erase(iter);
		}
		else
			iter++;
	}
}

void Grammar::elimNonterminating()
{
}

void Grammar::cleanUp()
{
	elimUnreachable();
	elimNonterminating();
	elimLambda();
	elimUnit();
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

			// Output production that yields transition rule
			std::cout << "\t" << iter->first << " -> ";
			for (int k = 0; k < iter->second[i].size(); k++)
				std::cout << iter->second[i][k] << " ";

			std::getchar();
		}
	}
	std::cout << "d(q1, #, z) = (q2, #)\n";
}