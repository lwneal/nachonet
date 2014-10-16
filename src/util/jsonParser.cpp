/*******************************************************************************
File:
Author: 		Josh Siva
Date:
Class:			CS460
Assignment:
Purpose:
*******************************************************************************/

#include "../../include/util/jsonParser.h"
#include <stdlib.h>

const std::string jsonParser::TRUE = "true";
const std::string jsonParser::FALSE = "false";

//Helpful things for the lexer portion
#define IS_DIGIT(c) ((c >= '0') && (c<= '9'))
#define IS_LETTER(c) (((c >= 'a') && (c <= 'z')) ||\
                         ((c >= 'A') && (c <= 'Z')))
#define IS_OTHER_VALID_CHAR(c) ((c == '_') || (c == '\"') || (c == '\\') || \
															 (c == '\b') || (c == '\f') || \
															 (c == 'n') || (c == '\r') || (c == '\t'))
#define IS_VALID_START_STR(c) (IS_LETTER(c) || IS_OTHER_VALID_CHAR(c) || \
															 IS_DIGIT(c))

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

/*******************************************************************************
 * Constructor:	jsonParser
 *
 * Description: Initializes the bare necessities. Does not need a string, so no
 * 							parsing happens here.
 *
 * Parameters:  None
 *
 * Returned:		None
 ******************************************************************************/
jsonParser::jsonParser ()
{
	this->validJSON = false;
	pJSON = new JSON;
	strPos = 0;
}

/*******************************************************************************
 * Constructor:	jsonParser
 *
 * Description: Initializes the JSON object and initializes the raw JSON string
 *
 * Parameters:  rawJSON - a string bursting with delicious JSON flavor
 *
 * Returned:		None
 ******************************************************************************/
jsonParser::jsonParser (std::string rawJSON)
{
	pJSON = new JSON;
	init (rawJSON);
}

/*******************************************************************************
 * Method:			~pJSON
 *
 * Description:	Deletes the JSON object
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
jsonParser::~jsonParser ()
{
	delete pJSON;
}

/*******************************************************************************
 * Method:			init
 *
 * Description: Initializes the parser and starts parsing
 *
 * Parameters:	rawJSON - the JSON string to be parsed
 *
 * Returned:		None
 ******************************************************************************/
void jsonParser::init (std::string rawJSON)
{
	this->rawJSON = rawJSON;
	validJSON = true;
	strPos = 0;
	currentTok.tokenClass = EMPTY;
	prevTok.tokenClass = EMPTY;

	object ();
}

/*******************************************************************************
 * Method:			getObject
 *
 * Description:	Return the JSON object created from the parsed text
 *
 * Parameters:	None
 *
 * Returned:		JSON - the object from the text
 ******************************************************************************/
JSON jsonParser::getObject ()
{
	return *pJSON;
}

/*******************************************************************************
 * Method:			getNextToken
 *
 * Description:	Incrementally breaks the JSON string up into tokens which can be
 * 						 	used later by the parser. The currentToken member is updated
 *
 * Parameters:	None
 *
 * Returned:		None
 ******************************************************************************/
void jsonParser::getNextToken ()
{
	Token returnTok;
	bool tokenNotFound = true, isString, validChar = true;
	char ch;

	while (tokenNotFound)
	{
		ch = rawJSON[strPos++];

		switch (ch)
		{
			case '\0':
				tokenNotFound = false;
				break;
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
				returnTok.tokenClass = QUOTE;
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
			case '\r':
				break;

			default:
				if (QUOTE != prevTok.tokenClass && IS_DIGIT (ch))
				{
					returnTok.tokenClass = INT;

					do
					{
						returnTok.lexeme.push_back(ch);
						ch = rawJSON[strPos++];
					} while (IS_DIGIT (ch));

					strPos--;

					tokenNotFound = false;
				}
				else if (IS_VALID_START_STR (ch))
				{
					returnTok.tokenClass = STR;

					//If the previous token was a quote then this text must be a string
					//otherwise we are probably looking at a bool
					if (QUOTE == prevTok.tokenClass)
					{
						isString = true;
					}
					else
					{
						isString = false;
					}

					do
					{
						returnTok.lexeme.push_back(ch);
						ch = rawJSON[strPos++];

						if (!isString && !IS_LETTER(ch))
						{
							validChar = false;
						}
					} while ('"' != ch && '\\' != ch && validChar);

					strPos--;

					if (0 == returnTok.lexeme.compare(TRUE) ||
							0 == returnTok.lexeme.compare(FALSE))
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

/*******************************************************************************
 * Method:			getObjectString
 *
 * Description:	Given the index of a left curly brace, creates a substring until
 * 							the matching right curly brace. This is used to extract and
 * 							parse a JSON object that is a value.
 *
 * Parameters:	index - the index of the left curly brace
 *
 * Returned:		string - the substring that contains a whole object's text
 ******************************************************************************/
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

/*******************************************************************************
 * Method:			peek
 *
 * Description:	Checks the table of firsts to see if the given non terminal
 * 							can have the current token as the first terminal along some
 * 							expansion
 *
 * Parameters:	nonTerminal - an int that stands for the non terminal to check
 *
 * Returned:		true if the current token is in the First, false otherwise
 ******************************************************************************/
bool jsonParser::peek (int nonTerminal)
{
	return gFirsts[nonTerminal][currentTok.tokenClass];
}

/*******************************************************************************
 * Method:			match
 *
 * Description:	Try to match the current token with the argument. If there is no
 * 							current token then get a new one.
 *
 * Parameters:	tokenClass - an int that represents the class of the token we
 * 													 want to match
 *
 * Returned:		true if there is a match, false otherwise
 ******************************************************************************/
bool jsonParser::match (int tokenClass)
{
	bool isMatch = false;

	if (EMPTY == currentTok.tokenClass)
	{
		getNextToken();
	}

	if(tokenClass == currentTok.tokenClass)
	{
		isMatch = true;
		prevTok = currentTok;
		currentTok.tokenClass = EMPTY;
	}

	return isMatch;
}

/*******************************************************************************
 * Method:			object
 *
 * Description:	object -> { } | { members }
 *
 * Parameters:	None
 *
 * Returned:		true if JSON text is valid, false otherwise
 ******************************************************************************/
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

/*******************************************************************************
 * Method:			members
 *
 * Description:	members -> pair | pair , members
 *
 * Parameters:	None
 *
 * Returned:		true if JSON text is valid, false otherwise
 ******************************************************************************/
bool jsonParser::members ()
{
	if (validJSON)
	{
		pair ();

		if (match (COMMA))
		{
			getNextToken ();

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

/*******************************************************************************
 * Method:			pair
 *
 * Description:	pair -> string : value
 *
 * Parameters:	None
 *
 * Returned:		true if JSON text is valid, false otherwise
 ******************************************************************************/
bool jsonParser::pair ()
{
	jsonData newVal;
	std::string key;

	if (validJSON)
	{
		string (&newVal);

		key = newVal.value.strVal;

		if (match (COLON))
		{
			getNextToken ();

			if (peek (VALUE))
			{
				value (&newVal);

				pJSON->setValue(key, newVal);
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

/*******************************************************************************
 * Method:			array
 *
 * Description:	array -> [ ] | [ elements ]
 *
 * Parameters:	pVal - pointer to the jsonData we are constructing
 *
 * Returned:		true if JSON text is valid, false otherwise
 ******************************************************************************/
bool jsonParser::array (jsonData *pVal)
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

/*******************************************************************************
 * Method:			elements
 *
 * Description:	elements -> value | value , elements
 *
 * Parameters:	pVal - pointer to the jsonData we are constructing
 *
 * Returned:		true if JSON text is valid, false otherwise
 ******************************************************************************/
bool jsonParser::elements (jsonData *pVal)
{
	jsonData element;

	if (validJSON)
	{
		value (&element);

		pVal->value.array.push_back(element);

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

/*******************************************************************************
 * Method:			value
 *
 * Description:	value -> string | num | object | array | bool
 *
 * Parameters:	pVal - pointer to the jsonData we are constructing
 *
 * Returned:		true if JSON text is valid, false otherwise
 ******************************************************************************/
bool jsonParser::value (jsonData *pVal)
{
	jsonParser * pNewParser;
	std::string objString;

	if (validJSON)
	{
		if (peek (STRING))
		{
			pVal->type = STR_TYPE;
			string (pVal);
		}
		else if (peek (NUM))
		{
			pVal->type = INT_TYPE;
			num (pVal);
		}
		else if (peek (OBJECT))
		{
			pVal->type = OBJ_TYPE;
			objString = getObjectString (currentTok.firstCharPos);
			pNewParser = new jsonParser (objString);
			pVal->value.pObject = new JSON;

			*(pVal->value.pObject) = pNewParser->getObject();
			delete pNewParser;

			this->strPos += objString.length();
			this->strPos--;
			getNextToken ();
		}
		else if (peek (ARRAY))
		{
			pVal->type = VEC_TYPE;
			array (pVal);
		}
		else if (BOOL == currentTok.tokenClass)
		{
			pVal->type = BOOL_TYPE;

			if (0 == currentTok.lexeme.compare (TRUE))
			{
				pVal->value.boolVal = true;
			}
			else
			{
				pVal->value.boolVal = false;
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

/*******************************************************************************
 * Method:			string
 *
 * Description: string -> "" | "chars"
 *
 * Parameters:	pVal - pointer to the jsonData we are constructing
 *
 * Returned:		true if JSON text is valid, false otherwise
 ******************************************************************************/
bool jsonParser::string (jsonData *pVal)
{
	if (validJSON)
	{
		if (match (QUOTE))
		{
			getNextToken ();
			if (STR == currentTok.tokenClass)
			{
				pVal->value.strVal = currentTok.lexeme;
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

/*******************************************************************************
 * Method:			num
 *
 * Description: num -> number
 * 							Does not handle scientific notation
 *
 * Parameters:	pVal - pointer to the jsonData we are constructing
 *
 * Returned:		true if JSON text is valid, false otherwise
 ******************************************************************************/
bool jsonParser::num (jsonData *pVal)
{
	std::string numStr;

	if (validJSON)
	{
		if (match (NEG))
		{
			numStr.push_back ('-');
			getNextToken ();
		}

		if (INT == currentTok.tokenClass)
		{
			numStr += currentTok.lexeme;
			match (INT);

			if (match (DECIMAL))
			{
				pVal->type = FLT_TYPE;

				numStr.push_back('.');

				getNextToken ();

				if (INT == currentTok.tokenClass)
				{
					numStr += currentTok.lexeme;
					match (INT);
				}

				numStr.push_back('0');

				pVal->value.floatVal = atof (numStr.c_str());
			}
			else
			{
				pVal->value.intVal = atoi (numStr.c_str());
			}

		}
		else
		{
			validJSON = false;
		}

	}

	return validJSON;
}



