/*******************************************************************************
File:				jsonParser.h
Author: 		Josh Siva
Date:				2/24/14
Project:		NachoNet
Purpose:		Defines an object that parses json text into a std::map
*******************************************************************************/

#pragma once
#include "json.h"

/*enum DataType
    {
    	INT_TYPE,
    	FLT_TYPE,
    	STR_TYPE,
    	OBJ_TYPE,
    	VEC_TYPE,
    	BOOL_TYPE
    } DataType;*/

typedef union jsonVal
{
	/*DataType type;
	void * pData;*/
	int intVal;
	std::string strVal;
	float floatVal;
	JSON object;
	std::vector<jsonVal> array;
	bool boolVal;
} jsonVal;

typedef struct Token
{
	int tokenClass;
	std::string lexeme;
	int firstCharPos;
} Token;

class jsonParser
{
	public:
		jsonParser (std::string rawJSON);
		~jsonParser ();
		void init (std::string rawJSON);
		JSON getObject ();

		void getNextToken();
    bool peek (int nonTerminal);
    bool match (int tokenClass);

    bool object ();
    bool members ();
    bool pair ();
    bool array (jsonVal *pVal);
    bool elements (jsonVal *pVal);
    bool value (jsonVal *pVal);
    bool string (jsonVal *pVal);
    bool num (jsonVal *pVal);
    bool frac (jsonVal *pVal);

    const char * TRUE = "true";
    const char * FALSE = "false";

		enum NonTerminals
    {
    	OBJECT = 0,
    	MEMBERS,
    	PAIR,
    	ARRAY,
    	ELEMENTS,
    	VALUE,
    	STRING,
    	NUM
    } NonTerminals;

    enum TokenClass
    {
    	L_BRACE = 0,
    	R_BRACE,
    	L_BRKT,
    	R_BRKT,
    	QUOTE,
    	COMMA,
    	COLON,
    	DECIMAL,
    	STR,
    	INT,
    	NEG,
    	BOOL,
    	NULL
    } TokenClass;



    const int MAX_NONTERMINALS = 8;
    const int MAX_FIRSTS = 11;

	private:
    std::string getObjectString (int index);

		JSON json;
    bool validJSON;
    std::string rawJSON;
    int strPos;
    Token currentTok;



};
