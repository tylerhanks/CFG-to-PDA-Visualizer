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
	// int nt_count = 0;

	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		//read the lhs non terminal
		iss >> non_terminal;

		// TODO: Delete this?
		// non_terminals[non_terminal] = ++nt_count;

		//nom the arrow
		iss >> consumer;

		//add the rest of the production to a vector
		while (!iss.eof())
		{
			iss >> consumer;
			if (consumer != "|")
			{
				productions[non_terminal].emplace(consumer);
			}
		}
	}
}

void Grammar::convertToCNF()
{
	// TODO: convert to CNF
}

void Grammar::convertToGNF()
{
	if (!isCNF())
		convertToCNF();

	// TODO: convert to GNF
}

void Grammar::printGrammar()
{
	// Loop through all production symbols
	for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); ++iter_symbol)
	{
		std::cout << iter_symbol->first << " -> ";

		// Loop through all sets of rules for each production symbol
		for (auto iter_rule = iter_symbol->second.begin(); iter_rule != iter_symbol->second.end(); ++iter_rule)
		{
			if (iter_rule != iter_symbol->second.begin())
				std::cout << " | ";

			std::cout << *iter_rule;
		}
		std::cout << "\n";
	}
}

bool Grammar::isNonTerminal(char ch)
{
	return (ch >= 'A' && ch <= 'Z');
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

std::vector<std::string> Grammar::getCombos(char symbolToRemove, std::string rule)
{
	std::vector<int> positions;
	std::vector<std::string> output;

	// Parse string for all positions of symbolToRemove
	for (int i = 0; i < rule.size(); ++i)
	{
		if (rule[i] == symbolToRemove)
		{
			positions.push_back(i);
		}
	}

	getCombosRecur(positions.size(), rule, output, positions);

	return output;
}

void Grammar::elimLambda()
{
	struct pair_hash
	{
		std::size_t operator () (std::pair<std::string, std::string> const &pair) const
		{
			std::size_t h1 = std::hash<std::string>()(pair.first);
			std::size_t h2 = std::hash<std::string>()(pair.second);

			return h1 ^ h2;
		}
	};
	std::unordered_set<std::string> hasLambda;
	// Map<Symbol referenced, Vector< Pair<Symbol referencing, rule>>>
	std::unordered_map < std::string, std::unordered_set<std::pair<std::string, std::string>, pair_hash>> references;

	// Loop through all production symbols
	for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); ++iter_symbol)
	{
		// Loop through all rules of current symbol
		for (auto iter_rule = iter_symbol->second.begin(); iter_rule != iter_symbol->second.end();)
		{
			bool erased = false;

			// Loop through all char of current rule
			for (int i = 0; i < (*iter_rule).size(); ++i)
			{
				std::string currChar = std::string(1, (*iter_rule)[i]); // current character

				// Keep track of referenced productions in case removal is necessary
				if (isNonTerminal((*iter_rule)[i]))
				{
					references[currChar].emplace(std::make_pair(iter_symbol->first, *iter_rule));
				}

				// If lambda is found and production is not set to for lambda removal
				if (currChar == "#" && (*iter_rule).size() == 1)
				{
					// Mark for work and remove lambda
					hasLambda.emplace(iter_symbol->first);
					iter_rule = iter_symbol->second.erase(iter_rule);
					erased = true;
					break;
				}
			}

			if (!erased)
			{
				++iter_rule;
			}

		}
	}

	// Go through and remove lambdas
	for (auto iter_symbol = hasLambda.begin(); iter_symbol != hasLambda.end(); ++iter_symbol)
	{
		// If reference was found for current lambda
		auto curr = references.find(*iter_symbol);
		if (curr != references.end())
		{
			// Loop through all references
			for (auto iter_rule = curr->second.begin(); iter_rule != curr->second.end(); ++iter_rule)
			{
				std::cout << "After removing # from (" << *iter_symbol << ") => Added to (" << iter_rule->first << ") : ";
				auto outputVector = getCombos((*iter_symbol)[0], iter_rule->second);

				// Add output to productions
				for (int i = 0; i < outputVector.size(); ++i)
				{
					productions[iter_rule->first].emplace(outputVector[i]);
					std::cout << outputVector[i] << " ";
				}
				std::cout << std::endl;
			}
		}
	}
}

void Grammar::elimUnit()
{
	//TODO: eliminate unit productions from the grammar
}

//void Grammar::elimUnreachable()
//{
//	//store reached variables; anything not in this vector at end of bfs is unreachable
//	std::unordered_set<std::string> visited;
//	visited.emplace("S");
//
//	//breadth-first search
//	std::queue<std::string> bfs;
//	bfs.push("S");
//
//	while (!bfs.empty())
//	{
//		std::string examine = bfs.front();
//		auto iter = productions.find(examine);
//
//		// Check if current symbol exists in map
//		if (iter != productions.end())
//		{
//			auto currProduction = iter->second;
//
//			// Look through that symbol's productions for nonterminals
//			for (int i = 0; i < currProduction.size(); ++i)
//			{
//				for (int j = 0; j < currProduction[i].size(); ++j)
//				{
//					std::unordered_map<std::string, int>::const_iterator got = non_terminals.find(iter->second[i][j]);
//					// If nonterminal found, check that it hasn't been encountered yet
//					if (got != non_terminals.end())
//					{
//						// Check if encountered nonterminal is new
//						auto reached = visited.find(got->first);
//
//						// If nonterminal is new, add it to visited and add it to queue
//						if (reached == visited.end())
//						{
//							bfs.push(got->first);
//							visited.emplace(got->first);
//						}
//					}
//				}
//			}
//		}
//
//		bfs.pop();
//
//	}
//
//	// After bfs, remove unreachable symbols from both maps
//	// Loop through all symbols
//	for (auto iter = productions.begin(); iter != productions.end();)
//	{
//		// Check if they were reached in bfs
//		auto reached = visited.find(iter->first);
//		if (reached == visited.end())
//		{
//			// If symbol was not reached, remove it
//			iter = productions.erase(iter);
//		}
//		else
//			iter++;
//	}
//	for (auto iter = non_terminals.begin(); iter != non_terminals.end();)
//	{
//		// Check if they were reached in bfs
//		auto reached = visited.find(iter->first);
//		if (reached == visited.end())
//		{
//			// If symbol was not reached, remove it
//			iter = non_terminals.erase(iter);
//		}
//		else
//			iter++;
//	}
//}

void Grammar::elimNonterminating()
{
}

//void Grammar::cleanUp()
//{
//	elimUnreachable();
//	elimNonterminating();
//	elimLambda();
//	elimUnit();
//}

// TODO: Unfinished?
bool Grammar::isCNF()
{
	return false;
}

// TODO: Unfinished?
bool Grammar::isGNF()
{
	return true;
}

//void Grammar::printTransitionFunctions()
//{
//	//# is lambda
//	std::cout << "d(q0, #, z) = (q1, Sz)";
//	std::getchar();
//
//	// Loop through all production symbols
//	for (auto iter = productions.begin(); iter != productions.end(); iter++)
//	{
//		// Loop through all rule sets of current production symbol
//		for (int i = 0; i < iter->second.size(); i++)
//		{
//			// Loop through each symbol of current production rule
//			for (int j = 0; j < iter->second[i].size(); j++)
//			{
//				if (iter->second[i].size() == 1)
//					std::cout << "d(q1, " << iter->second[i][j] << ", " << iter->first << ") = (q1, #)";
//				else
//				{
//					if (j == 0)
//						std::cout << "d(q1, " << iter->second[i][j] << ", " << iter->first << ") = (q1, ";
//					else
//						std::cout << iter->second[i][j];
//
//					if (j == (iter->second[i].size() - 1))
//						std::cout << ")";
//				}
//			}
//
//			// Output production that yields transition rule
//			std::cout << "\t" << iter->first << " -> ";
//			for (int k = 0; k < iter->second[i].size(); k++)
//				std::cout << iter->second[i][k] << " ";
//
//			std::getchar();
//		}
//	}
//	std::cout << "d(q1, #, z) = (q2, #)\n";
//}