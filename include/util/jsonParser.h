/*******************************************************************************
File:				jsonParser.h
Author: 		Josh Siva
Date:				2/24/14
Project:		NachoNet
Purpose:		Defines an object that parses json text into a std::map
*******************************************************************************/

#pragma once
#include "json.h"
#include <string>
#include <vector>

class JSON;

typedef struct jsonData
{
	jsonData& operator=(const jsonData& that)
	{
		//value = that.value;
		value.array = that.value.array;
		value.boolVal = that.value.boolVal;
		value.floatVal = that.value.floatVal;
		value.intVal = that.value.intVal;
		value.strVal = that.value.strVal;
		value.pObject = that.value.pObject;
		type = that.type;

		return *this;
	}

	typedef struct jsonVal
	{
		jsonVal ()
		{
			pObject = NULL;
			intVal = 0;
			floatVal = 0.0;
			boolVal = false;
		}

		/*jsonVal (const jsonVal & that)
		{
			intVal = that.intVal;
			strVal = that.strVal;
			floatVal = that.floatVal;
			array = that.array;
			boolVal = that.boolVal;

			pObject = new JSON;
			*pObject = *(that.pObject);
		}

		~jsonVal ()
		{
			if (NULL != pObject)
			{
				delete pObject;
				pObject = NULL;
			}
		}

		jsonVal & operator= (const jsonVal& that)
		{
			if (this != &that)
			{
				intVal = that.intVal;
				strVal = that.strVal;
				floatVal = that.floatVal;
				array = that.array;
				boolVal = that.boolVal;

				if (NULL != pObject)
				{
					delete pObject;
				}

				pObject = new JSON;
				*pObject = *(that.pObject);
			}
			return *this;
		}*/

		int intVal;
		std::string strVal;
		float floatVal;
		JSON *pObject;
		std::vector<jsonData> array;
		bool boolVal;
	} jsonVal;

	int type;
	jsonVal value;
} jsonData;

typedef struct Token
{
	int tokenClass;
	std::string lexeme;
	int firstCharPos;
} Token;

class jsonParser
{
	public:
		jsonParser ();
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
    bool array (jsonData *pVal);
    bool elements (jsonData *pVal);
    bool value (jsonData *pVal);
    bool string (jsonData *pVal);
    bool num (jsonData *pVal);

    static const std::string TRUE;
    static const std::string FALSE;

		static enum NonTerminals
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

    static enum TokenClass
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
    	EMPTY
    } TokenClass;

    static enum DataType
		{
			INT_TYPE,
			FLT_TYPE,
			STR_TYPE,
			OBJ_TYPE,
			VEC_TYPE,
			BOOL_TYPE
		} DataType;



    static const int MAX_NONTERMINALS = 8;
    static const int MAX_FIRSTS = 12;

	private:
    std::string getObjectString (int index);

		JSON * pJSON;
    bool validJSON;
    std::string rawJSON;
    int strPos;
    Token currentTok;
    Token prevTok;



};
