#include "Grammar.h"
#include <unordered_set> // Used for BFS in ::elimUnreachable
#include <unordered_map>
#include <string>

/* CONSTRUCTOR */
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

		//add to  non_terminals map
		non_terminals.emplace(non_terminal, 0);

		//nom the arrow
		iss >> consumer;

		//add the rest of the production to a set
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


std::string Grammar::convert_int_to_string(int x)
{
	// Because to_string() is bugged using MinGW
	if (x < 0)
		return std::string("-") + convert_int_to_string(-x);
	if (x < 10)
		return std::string(1, x + '0');
	return convert_int_to_string(x / 10) + convert_int_to_string(x % 10);
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

	// Get combinations of deletions
	getCombosRecur(positions.size(), rule, output, positions);

	return output;
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

int Grammar::printTransitionFunctions()
{
	// Check first that grammar is in GNF
	// Loop through all symbols
	for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
	{
		// Loop through all curent rules of symbol
		for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
		{
			// Loop through each character in production
			for (int i = 0; i < (*iter_rules).length(); i++)
			{
				// Not in GNF if first character isn't terminal or if non-first characters are terminals
				if (i == 0)
				{
					if (((*iter_rules)[i] < 'a') || ('z' < (*iter_rules)[i]))
					{
						std::cout << "Invalid Grammar: not in Greibach normal form\n\n";
						return 1;
					}
				}
				else
				{
					if (((*iter_rules)[i] >= 'a') && ('z' >= (*iter_rules)[i]))
					{
						std::cout << "Invalid Grammar: not in Greibach normal form\n\n";
						return 1;
					}
				}
			}
		}
	}

	//# is lambda
	std::cout << "d(q0, #, z) = (q1, Sz)";
	std::getchar();

	// Loop through all production symbols
	for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
	{
		// Loop through all rule sets of current production symbol
		for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
		{
			// Loop through characters in production
			for (int i = 0; i < (*iter_rules).size(); ++i)
			{
				if ((*iter_rules).size() == 1)
					std::cout << "d(q1, " << (*iter_rules)[i] << ", " << iter_symbol->first << ") = (q1, #)";
				else
				{
					if (i == 0)
						std::cout << "d(q1, " << (*iter_rules)[i] << ", " << iter_symbol->first << ") = (q1, ";
					else
						std::cout << (*iter_rules)[i];

					if (i == ((*iter_rules).size() - 1))
						std::cout << ")";
				}
			}

			// Output production that yields transition rule
			std::cout << "\t\t" << iter_symbol->first << " -> ";
			for (int k = 0; k < (*iter_rules).size(); k++)
				std::cout << (*iter_rules)[k];

			std::getchar();
		}
	}
	std::cout << "d(q1, #, z) = (q2, #)\n";
	return 0;
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
		auto iter_symbol = productions.find(examine);
		// Look through examine's productions for nonterminals
		for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
		{
			// Loop through characters in production
			for (int i = 0; i < (*iter_rules).size(); ++i)
			{
				std::string in_question = (*iter_rules).substr(i, 1);
				auto got = non_terminals.find(in_question);

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
	for (auto iter = non_terminals.begin(); iter != non_terminals.end(); iter++)
		iter->second = 0;	// 1->that symbol terminates, 0-> that symbol does not

	int init_term, end_term;
	bool done = false;

	// Look for symbols with productions that include only terminals, #,  and terminating nonterminals
	while (!done)
	{
		init_term = 0;
		end_term = 0;
		// Count number of terminating symbols
		for (auto iter = non_terminals.begin(); iter != non_terminals.end(); iter++)
			init_term += iter->second;

		// Check symbols
		for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
		{
			// Proceed if symbol doesn't terminate
			if (non_terminals[iter_symbol->first] == 0)
			{
				// Loop through all rules of current symbol
				for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
				{
					bool prod_terminates = true;
					// Scan each character in rule
					for (int i = 0; i < (*iter_rules).length(); i++)
					{
						// If character is a nonterminating nonterminal, production doesn't terminate
						std::string in_question = (*iter_rules).substr(i, 1);
						auto got = non_terminals.find(in_question);
						if (got != non_terminals.end())
						{
							if (got->second == 0)
							{
								prod_terminates = false;
								break;
							}
						}
					}
					if (prod_terminates)
					{
						non_terminals[iter_symbol->first] = 1;
						break;
					}
				}
			}
		}

		// Recount number of terminating symbols
		for (auto iter = non_terminals.begin(); iter != non_terminals.end(); iter++)
			end_term += iter->second;

		// If init_term == end_term, no new nonterminating symbols were found and we're done
		if (init_term == end_term)
			break;
	}

	// Delete productions containing nonterminating symbols
	for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
	{
		// Loop through all rules of current symbol
		for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end();)
		{
			bool kill = false;
			// Check each character
			for (int i = 0; i < (*iter_rules).length(); i++)
			{
				std::string in_question = (*iter_rules).substr(i, 1);
				auto got = non_terminals.find(in_question);
				if (got != non_terminals.end())
				{
					if (got->second == 0)
					{
						kill = true;
						break;
					}
				}
			}
			if (kill)
				iter_rules = productions[iter_symbol->first].erase(iter_rules);
			else
				iter_rules++;
		}
	}

	// Delete symbols that have no more productions from both maps
	for (auto iter = productions.begin(); iter != productions.end();)
	{
		if (non_terminals[iter->first] == 0)
			iter = productions.erase(iter);
		else
			iter++;
	}
	for (auto iter = non_terminals.begin(); iter != non_terminals.end();)
	{
		if (iter->second == 0)
			iter = non_terminals.erase(iter);
		else
			iter++;
	}

	// Reset integers in nonterminal map to 0 for future functions
	for (auto iter = non_terminals.begin(); iter != non_terminals.end(); iter++)
		iter->second = 0;
}

void Grammar::elimLambda()
{
	struct pair_hash // Used to allow hashing of pairs of type string
	{
		std::size_t operator () (std::pair<std::string, std::string> const &pair) const
		{
			std::size_t h1 = std::hash<std::string>()(pair.first);
			std::size_t h2 = std::hash<std::string>()(pair.second);

			return h1 ^ h2;
		}
	};

	std::unordered_set<std::string> hasLambda;
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
				// std::cout << "After removing # from (" << *iter_symbol << ") => Added to (" << iter_rule->first << ") : ";
				auto outputVector = getCombos((*iter_symbol)[0], iter_rule->second);

				// Add output to productions
				for (int i = 0; i < outputVector.size(); ++i)
				{
					// Only add rule if it is nonempty
					if (outputVector[i] != "")
					{
						productions[iter_rule->first].emplace(outputVector[i]);
						// std::cout << outputVector[i] << " ";
					}
				}
				// std::cout << std::endl;
			}
		}
	}
}

void Grammar::elimUnit()
{
	// Get intial count of unit productions
	for (auto iter = non_terminals.begin(); iter != non_terminals.end(); iter++)
		iter->second = 0;	// Count of unit productions for that symbol	
	// Loop through all production symbols
	for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
	{
		// Loop through all rules of current symbol
		for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
		{
			// Production is unit production if it contains only a single nonterminal.
			if ((*iter_rules).length() == 1
				&& ((*iter_rules)[0] >= 'A')
				&& ((*iter_rules)[0] <= 'Z'))
			{
				non_terminals[iter_symbol->first]++;
			}
		}
	}
	int total_units = 0;
	for (auto iter = non_terminals.begin(); iter != non_terminals.end(); iter++)
		total_units += iter->second;

	// If unit productions exist, replace them with the productions for the symbol it's unit with
	while (total_units != 0)
	{
		// Loop through all production symbols
		for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
		{
			// Process symbol only if it has unit productions
			if (non_terminals[iter_symbol->first] != 0)
			{
				std::unordered_set<std::string> new_set;
				std::string copy_from;
				bool unit_hit = false;
				bool self_loop = false;
				// Loop through all rules of current symbol
				for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
				{
					if (((*iter_rules).length() == 1) && (((*iter_rules)[0] >= 'A') && ((*iter_rules)[0] <= 'Z')))
					{
						// Only the first unit production found for this symbol will be handled in this while loop iteration
						// First found unit production will not be copied, but the rest will
						if (!unit_hit)
						{
							// Identify the symbol who's productions will be copied to current symbol's list
							copy_from = (*iter_rules);

							if (copy_from.compare(iter_symbol->first) == 0)
								self_loop = true;

							unit_hit = true;
						}
						else
							new_set.emplace((*iter_rules));
					}
					else
						new_set.emplace((*iter_rules));
				}
				// If unit production isn't self loop, copy productions of symbol it's unit with to new_set
				// Swap sets.
				if (!self_loop)
				{
					for (auto copy_iter = productions[copy_from].begin(); copy_iter != productions[copy_from].end(); ++copy_iter)
					{
						// Don't copy production if (*copy_iter) == iter_symbol->first (self loop)
						if ((*copy_iter).compare(iter_symbol->first) != 0)
							new_set.emplace((*copy_iter));
					}
				}
				swap(iter_symbol->second, new_set);
			}
		}

		// Get new count of unit productions
		for (auto iter = non_terminals.begin(); iter != non_terminals.end(); iter++)
			iter->second = 0;
		for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
		{
			for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
			{
				if ((*iter_rules).length() == 1
					&& ((*iter_rules)[0] >= 'A')
					&& ((*iter_rules)[0] <= 'Z'))
				{
					non_terminals[iter_symbol->first]++;
				}
			}
		}
		total_units = 0;
		for (auto iter = non_terminals.begin(); iter != non_terminals.end(); iter++)
			total_units += iter->second;
	}
}

void Grammar::elimRedundant()
{
	//get all redundant pairs into a vector
	std::vector<std::pair<std::string, std::string>> redundancies;
	for(auto iter1 = productions.begin(); iter1 != productions.end(); iter1++)
		for (auto iter2 = iter1; iter2 != productions.end(); iter2++)
		{
			if (iter2 == iter1)
				continue;
			if (iter1->second == iter2->second)
			{
				redundancies.push_back({ iter1->first,iter2->first });
			}
		}
	//remove every redundant pair
	for (auto redundant_pair : redundancies)
	{
		std::string toReplace;
		std::string replaceWith;
		if (redundant_pair.first == "S") //S should never be replaced
		{
			replaceWith = redundant_pair.first;
			toReplace = redundant_pair.second;
		}
		else
		{
			replaceWith = redundant_pair.second;
			toReplace = redundant_pair.first;
		}
		
		productions.erase(toReplace);
		//iterate through all productions and replace toReplace with replaceWith
		for (auto production = productions.begin(); production != productions.end(); production++)
		{
			//iterate through all rules of a production
			for (auto rule = production->second.begin(); rule != production->second.end(); )
			{
				auto index = rule->find(toReplace);
				bool replaced = false;
				std::string new_rule = *rule;
				//replace all occurences of toReplace with replaceWith in rule
				while (index != std::string::npos)
				{
					replaced = true;
					new_rule = new_rule.replace(index, toReplace.length(), replaceWith);
					index = new_rule.find(toReplace);
				}
				if (replaced)
				{
					rule = production->second.erase(rule);
					production->second.insert(new_rule);
				}
				else
					rule++;
			}
		}
	}
}

int Grammar::elimLeftRecursion()
{
	//for each production in productions
	for (auto production = productions.begin(); production != productions.end(); )
	{
		std::unordered_set<std::string> bad_rules;
		std::unordered_set<std::string> good_rules;

		//partition rules into left recursive and non left recursive sets
		for (auto rule = production->second.begin(); rule != production->second.end(); rule++)
		{
			if (rule->at(0) == production->first[0]) //if a rule begins with the same non-terminal i.e. is left recursive
			{
				std::string alpha = rule->substr(1, std::string::npos);
				bad_rules.insert(alpha);
			}
			else
			{
				good_rules.insert(*rule);
			}
		}
		
		if (bad_rules.size() == 0)
		{
			production++;
			continue; //there was no left recursion
		}
		else
		{
			//find a viable replacement symbol
			char t;
			bool broken = false;
			for (t = 'Z'; t >= 'A'; t--)
			{
				std::string tail;
				tail += t;
				if (!non_terminals.count(tail))
				{
					broken = true;
					break;
				}
			}
			if (!broken)
				return -1; //failed to find available replacement symbol

			std::string tail;
			tail += t;
			non_terminals.insert({ tail, 0 }); //add new non-terminal to non_terminals
			std::unordered_set<std::string> alpha_rules;
			std::unordered_set<std::string> beta_rules;
		
			for (auto str : bad_rules)
			{
				str.append(tail);
				alpha_rules.insert(str);
			}
			for (auto str : good_rules)
			{
				str.append(tail);
				beta_rules.insert(str);
			}
			alpha_rules.insert("#");
			std::string newStr = production->first;

			production = productions.erase(production);

			productions.insert({ newStr, beta_rules });
			productions.insert({ tail, alpha_rules });
		}
	}
	return 1;
}

int Grammar::convertToGNF()
{
	// Check first that each production has a terminal as its initial character
	// Loop through all symbols
	for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
	{
		// Loop through all curent rules of symbol
		for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
		{
			// Check first character of each production
			if (((*iter_rules)[0] < 'a') || ('z' < (*iter_rules)[0]))
			{
				std::cout << "Invalid Grammar: contains production with non-terminal as first character\n\n";
				return 1;
			}
		}
	}

	// Index 0->a, 1->b, ..., 25->z.  Replace[i] = 1 means that terminal was replaced.
	int replace[26] = {};

	// Loop through all symbols
	for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
	{
		std::unordered_set<std::string> modified_productions;
		// Loop through all curent rules of symbol and store modified rules in modified_productions
		for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
		{
			std::string modified_rule = "";
			// Loop through each chracter in current rule and write modifications to modified_rule
			for (int i = 0; i < (*iter_rules).length(); i++)
			{
				// Except first character...
				if (i != 0)
				{
					// ...if terminal, replace with (number)
					// number = index in replace[] + 1; i.e. a->(1), b->(2), .., z->(26)
					// This way (number) is that letter's position in the alphabet
					if (((*iter_rules)[i] >= 'a') && ((*iter_rules)[i] <= 'z'))
					{
						//Generate (number) and add to modified_rule
						int index = (*iter_rules)[i] - 'a';
						replace[index] = 1;
						std::string replace_with = "(";
						replace_with += convert_int_to_string(index + 1);
						replace_with += ")";
						// Add to modified_rule
						modified_rule += replace_with;
					}
					else
						// Copy character to modified_rule if not terminal
						modified_rule += (*iter_rules).substr(i, 1);
				}
				else
					// Always get first character
					modified_rule += (*iter_rules).substr(i, 1);
			}
			// Put finished rule in new set
			modified_productions.emplace(modified_rule);
		}
		// Swap new set with symbol's old set
		swap(iter_symbol->second, modified_productions);
	}

	// Add corresponding productions for the terminals that were replaced.
	for (int i = 0; i < 26; i++)
	{
		if (replace[i] == 1)
		{
			std::string new_symbol = "(";
			new_symbol += convert_int_to_string(i + 1);
			new_symbol += ")";
			std::string new_prod(1, i + 'a');
			productions[new_symbol].emplace(new_prod);
		}
	}
	return 0;
}

int Grammar::printTransitionFunctions(int)
{
	// Check first that grammar is in GNF
	// Loop through all symbols
	for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
	{
		// Loop through all curent rules of symbol
		for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
		{
			// Loop through each character in production
			for (int i = 0; i < (*iter_rules).length(); i++)
			{
				// Not in GNF if first character isn't terminal or if non-first characters are terminals
				if (i == 0)
				{
					if (((*iter_rules)[i] < 'a') || ('z' < (*iter_rules)[i]))
					{
						std::cout << "Invalid Grammar: not in Greibach normal form\n\n";
						return 1;
					}
				}
				else
				{
					if (((*iter_rules)[i] >= 'a') && ('z' >= (*iter_rules)[i]))
					{
						std::cout << "Invalid Grammar: not in Greibach normal form\n\n";
						return 1;
					}
				}
			}
		}
	}

	//# is lambda
	std::cout << "d(q0, #, z) = (q1, Sz)" << std::endl;

	// Loop through all production symbols
	for (auto iter_symbol = productions.begin(); iter_symbol != productions.end(); iter_symbol++)
	{
		// Loop through all rule sets of current production symbol
		for (auto iter_rules = iter_symbol->second.begin(); iter_rules != iter_symbol->second.end(); ++iter_rules)
		{
			// Loop through characters in production
			for (int i = 0; i < (*iter_rules).size(); ++i)
			{
				if ((*iter_rules).size() == 1)
					std::cout << "d(q1, " << (*iter_rules)[i] << ", " << iter_symbol->first << ") = (q1, #)";
				else
				{
					if (i == 0)
						std::cout << "d(q1, " << (*iter_rules)[i] << ", " << iter_symbol->first << ") = (q1, ";
					else
						std::cout << (*iter_rules)[i];

					if (i == ((*iter_rules).size() - 1))
						std::cout << ")";
				}
			}

			// Output production that yields transition rule
			std::cout << "\t\t" << iter_symbol->first << " -> ";
			for (int k = 0; k < (*iter_rules).size(); k++)
				std::cout << (*iter_rules)[k];
			std::cout << std::endl;
		}
	}
	std::cout << "d(q1, #, z) = (q2, #)\n";
	return 0;
}
