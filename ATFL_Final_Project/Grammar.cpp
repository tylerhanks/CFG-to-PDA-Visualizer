#include "Grammar.h"

Grammar::Grammar(std::string filename)
{
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

void Grammar::elimLambda()
{
}

void Grammar::elimUnit()
{
}

bool Grammar::isCNF()
{
	return false;
}

bool Grammar::isGNF()
{
	return false;
}
