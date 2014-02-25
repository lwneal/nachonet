/*******************************************************************************
File:
Author: 		Josh Siva
Date:
Class:			CS460
Assignment:
Purpose:
*******************************************************************************/

#include "../../include/util/jsonParser.h"

#define IS_DIGIT(c) ((c >= '0') && (c<= '9'))
#define IS_LETTER(c) (((c >= 'a') && (c <= 'z')) ||\
                         ((c >= 'A') && (c <= 'Z')))
#define IS_OTHER_VALID_CHAR(c) ((c == '_') || (c == '\"') || (c == '\\') || \
															 (c == '\/') || (c == '\b') || (c == '\f') || \
															 (c == 'n') || (c == '\r') || (c == '\t'))
#define IS_VALID_START_STR(c) (IS_LETTER(c) || IS_OTHER_VALID_CHAR(c))

static int gFirsts[jsonParser::MAX_NONTERMINALS][jsonParser::MAX_FIRSTS] =
{
		{1,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0,0,0,0},
		{0,0,1,0,0,0,0,0,0,0,0,0},
		{1,0,1,0,1,0,0,0,0,1,1,1},
		{1,0,1,0,1,0,0,0,0,1,1,1},
		{0,0,0,0,1,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,1,1,0}
};


jsonParser::jsonParser (std::string rawJSON)
{
	init (rawJSON);
}
jsonParser::~jsonParser ()
{

}

void jsonParser::init (std::string rawJSON)
{
	object ();
}

JSON jsonParser::getObject ()
{
	return json;
}

void jsonParser::getNextToken ()
{
	Token returnTok;
	bool tokenNotFound = true;
	char ch;

	while (tokenNotFound)
	{
		ch = rawJSON[strPos++];

		switch (ch)
		{
			case '{':
				returnTok.tokenClass = L_BRACE;
				returnTok.lexeme.push_back(ch);
				returnTok.firstCharPos = strPos - 1;
				tokenNotFound = false;
				break;

			case '}':
				returnTok.tokenClass = R_BRACE;
				returnTok.lexeme.push_back(ch);
				tokenNotFound = false;
				break;

			case '[':
				returnTok.tokenClass = L_BRKT;
				returnTok.lexeme.push_back(ch);
				tokenNotFound = false;
				break;

			case ']':
				returnTok.tokenClass = R_BRKT;
				returnTok.lexeme.push_back(ch);
				tokenNotFound = false;
				break;

			case '"':
				returnTok.tokenClass = R_BRKT;
				returnTok.lexeme.push_back(ch);
				tokenNotFound = false;
				break;

			case ':':
				returnTok.tokenClass = COLON;
				returnTok.lexeme.push_back(ch);
				tokenNotFound = false;
				break;

			case ',':
				returnTok.tokenClass = COMMA;
				returnTok.lexeme.push_back(ch);
				tokenNotFound = false;
				break;

			case '-':
				returnTok.tokenClass = NEG;
				returnTok.lexeme.push_back(ch);
				tokenNotFound = false;
				break;

			case '.':
				returnTok.tokenClass = DECIMAL;
				returnTok.lexeme.push_back(ch);
				tokenNotFound = false;
				break;

			case ' ':
			case '\n':
			case '\t':
				break;

			default:
				if (IS_DIGIT (ch))
				{
					returnTok.tokenClass = INT;

					do
					{
						returnTok.lexeme.push_back(ch);
						ch = rawJSON[strPos++];
					} while (IS_DIGIT (ch));

					tokenNotFound = false;
				}
				else if (IS_VALID_START_STR (ch))
				{
					returnTok.tokenClass = STR;

					do
					{
						returnTok.lexeme.push_back(ch);
						ch = rawJSON[strPos++];
					} while ('"' != ch && '\\' != ch);

					if (returnTok.lexeme.compare(TRUE) ||
							returnTok.lexeme.compare(FALSE))
					{
						returnTok.tokenClass = BOOL;
					}

					tokenNotFound = false;
				}
				else
				{
					validJSON = false; //something went wrong so the whole thing is bad
				}

				break;
		}
	}

	currentTok = returnTok;
}

std::string jsonParser::getObjectString (int index)
{
	int objectCount = 0;
	std::string objectString;

	do
	{ //at the beginning of an object
		if ('{' == rawJSON[index])
		{
			objectCount++;
		}
		else if ('}' == rawJSON[index]) //end of an object
		{
			objectCount--;
		}

		objectString += rawJSON[index++]; //build up rawJSON string for object

	} while (0 != objectCount);

	return objectString;
}

bool jsonParser::peek (int nonTerminal)
{
	return gFirsts[nonTerminal][currentTok.tokenClass];
}

bool jsonParser::match (int tokenClass)
{
	bool isMatch = false;

	if (NULL == currentTok.tokenClass)
	{
		getNextToken();
	}

	if(tokenClass == currentTok)
	{
		isMatch = true;
		currentTok.tokenClass = NULL;
	}

	return isMatch;
}


bool jsonParser::object ()
{
	if (validJSON)
	{
		if (match (L_BRACE))
		{
			getNextToken ();

			if(peek(MEMBERS))
			{
				members ();
			}
			else if (match (R_BRACE))
			{

			}
			else
			{
				validJSON = false;
			}
		}
		else
		{
			validJSON = false;
		}
	}

	return validJSON;
}

bool jsonParser::members ()
{
	if (validJSON)
	{
		pair ();

		if (match (COMMA))
		{
			if(peek (MEMBERS))
			{
				members ();
			}
			else
			{
				validJSON = false;
			}
		}
	}

	return validJSON;
}

bool jsonParser::pair ()
{
	jsonVal newVal;
	std::string key;

	if (validJSON)
	{
		string (&newVal);

		key = newVal.strVal;

		if (match (COLON))
		{
			getNextToken ();

			if (peek (VALUE))
			{
				value (&newVal);

				json.setValue(key, newVal);
			}
			else
			{
				validJSON = false;
			}
		}
		else
		{
			validJSON = false;
		}
	}

	return validJSON;
}
bool jsonParser::array (jsonVal *pVal)
{
	if (validJSON)
	{
		if (match (L_BRKT))
		{
			getNextToken ();

			if (peek (ELEMENTS))
			{
				elements(pVal);

				if (!match (R_BRKT))
				{
					validJSON = false;
				}
			}
			else
			{
				validJSON = false;
			}
		}
		else
		{
			validJSON = false;
		}
	}

	return validJSON;
}
bool jsonParser::elements (jsonVal *pVal)
{
	jsonVal element;

	if (validJSON)
	{
		value (&element);

		pVal->array.push_back(element);

		if (match (COMMA))
		{
			getNextToken ();

			if (peek (ELEMENTS))
			{
				elements (pVal);
			}
			else
			{
				validJSON = false;
			}
		}
	}

	return validJSON;
}

bool jsonParser::value (jsonVal *pVal)
{
	jsonParser * pNewParser;

	if (validJSON)
	{
		if (peek (STRING))
		{
			string (pVal);
		}
		else if (peek (NUM))
		{
			num (pVal);
		}
		else if (peek (OBJECT))
		{
			pNewParser = new jsonParser (getObjectString (currentTok.firstCharPos));
			pVal->object = pNewParser->getObject();
			delete pNewParser;
		}
		else if (peek (ARRAY))
		{
			array (pVal);
		}
		else if (BOOL == currentTok.tokenClass)
		{
			if (currentTok.lexeme.compare (TRUE))
			{
				pVal->boolVal = true;
			}
			else
			{
				pVal->boolVal = false;
			}

			match (BOOL);
		}
		else
		{
			// I don't know how the hell you got here, but
			// you probably did something wrong...
			validJSON = false;
		}
	}

	return validJSON;
}
bool jsonParser::string (jsonVal *pVal)
{
	if (validJSON)
	{
		if (match (QUOTE))
		{
			if (STR == currentTok.tokenClass)
			{
				pVal->strVal = currentTok.lexeme;
				match (STR);
			}

			if (!match (QUOTE))
			{
				validJSON = false;
			}
		}
		else
		{
			validJSON = false;
		}
	}

	return validJSON;
}

bool jsonParser::num (jsonVal *pVal)
{
	std::string numStr;

	if (validJSON)
	{
		if (match (NEG))
		{
			numStr.push_back ('-');
		}

		if (INT == currentTok.tokenClass)
		{
			numStr += currentTok.lexeme;
			match (INT);

			if (match (DECIMAL))
			{
				numStr.push_back('.');

				if (INT == currentTok.tokenClass)
				{
					numStr += currentTok.lexeme;
					match (INT);
				}

				numStr.push_back('0');

				pVal->floatVal = atof (numStr.c_str());
			}
			else
			{
				pVal->intVal = atoi (numStr.c_str());
			}

		}
		else
		{
			validJSON = false;
		}

	}

	return validJSON;
}



