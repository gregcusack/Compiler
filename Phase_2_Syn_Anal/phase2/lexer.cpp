/*
 * File:	lexer.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the lexical analyzer for Simple C.
 */

# include <cstdio>
# include <cctype>
# include <string>
# include <iostream>
# include <stdio.h>
# include "lexer.h"
//# include "Syn_Anal.cpp"

using namespace std;
int numerrors, lineno = 1;


/* Later, we will associate token values with each keyword */

static string keywords[] = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while",
};

# define numKeywords (sizeof(keywords) / sizeof(keywords[0]))


/*
 * Function:	report
 *
 * Description:	Report an error to the standard error prefixed with the
 *		line number.  We'll be using this a lot later with an
 *		optional string argument, but C++'s stupid streams don't do
 *		positional arguments, so we actually resort to snprintf.
 *		You just can't beat C for doing things down and dirty.
 */

void report(const string &str, const string &arg)
{
    char buf[1000];

    snprintf(buf, sizeof(buf), str.c_str(), arg.c_str());
    cerr << "line " << lineno << ": " << buf << endl;
    numerrors ++;
}


/*
 * Function:	lexan
 *
 * Description:	Read and tokenize the standard input stream.  The lexeme is
 *		stored in a buffer.
 */

int lexan(string &lexbuf)
{
    int p;
    unsigned i;
    static int c = cin.get();


    /* The invariant here is that the next character has already been read
       and is ready to be classified.  In this way, we eliminate having to
       push back characters onto the stream, merely to read them again. */

    while (!cin.eof()) {
	lexbuf.clear();


	/* Ignore white space */

	while (isspace(c)) {
	    if (c == '\n')
		lineno ++;

	    c = cin.get();
	}


	/* Check for an identifier or a keyword */

	if (isalpha(c) || c == '_') {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isalnum(c) || c == '_');

	    for (i = 0; i < numKeywords; i ++)
		if (keywords[i] == lexbuf)
		    break;

	    if (i < numKeywords)
		// cout << "keyword:" << lexbuf << endl;
		return (400+i);
	    else
		return ID;
		//cout << "identifier:" << lexbuf << endl;
	    //return true;*/


	/* Check for a number */

	} else if (isdigit(c)) {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isdigit(c));

	    //cout << "number:" << lexbuf << endl;
	    return NUM;
	    //What do we return here?  Do we return ID???


	/* There must be an easier way to do this.  It might seem stupid at
	   this point to recognize each token separately, but eventually
	   we'll be returning separate token values to the parser, so we
	   might as well do it now. */

	} else {
	    lexbuf += c;

	    switch(c) {


	    /* Check for '||' */

	    case '|':
		c = cin.get();

		if (c == '|') {
		    lexbuf += c;
		    c = cin.get();
		    //cout << "operator:" << lexbuf << endl;
		    return OR;
		}

		cout << "illegal:" << lexbuf << endl;
		//return true;
		//what about with illegal entries??


	    /* Check for '=' and '==' */

	    case '=':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
		    return EQL; //val is '=='
		}
		else
		    return ASGN; //val is '=' (assignment operator)
		
		//cout << "operator:" << lexbuf << endl;
		//return true;


	    /* Check for '&' and '&&' */

	    case '&':
		c = cin.get();

		if (c == '&') {
		    lexbuf += c;
		    c = cin.get();
		    return AND; //val is '&&'
		}
		else
		    return ADDR; //val is a '&' (address operator)
		
		//cout << "operator:" << lexbuf << endl;
		//return true;


	    /* Check for '!' and '!=' */

	    case '!':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
		    return NEQ; //val is '!='
		}
		else
		    return NOT; //val is '!'
		
		//cout << "operator:" << lexbuf << endl;
		//return true;


	    /* Check for '<' and '<=' */

	    case '<':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
		    return LEQ; //val is '<='
		}
		else
		    return LTN; //val is '<'
		
		//cout << "operator:" << lexbuf << endl;
		//return true;


	    /* Check for '>' and '>=' */

	    case '>':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
		    return GEQ; //val is '>='
		}
		else
		    return GTN; //val is '>'
		
		//cout << "operator:" << lexbuf << endl;
		//return true;


	    /* Check for '-', '--', and '->' */

	    case '-':
		c = cin.get();

		if (c == '-') {
		    lexbuf += c;
		    c = cin.get();
		    return DECR; //val is --
		} else if (c == '>') {
		    lexbuf += c;
		    c = cin.get();
		    return ARROW; //val is '->'
		}
		else
		    return SUB; //val is '-'

		//cout << "operator:" << lexbuf << endl;
		//return true;


	    /* Check for '+' and '++' */

	    case '+':
		c = cin.get();

		if (c == '+') {
		    lexbuf += c;
		    c = cin.get();
		    return INCR; //val is '++'
		}
		else
		    return ADD; //val is '+'

		//cout << "operator:" << lexbuf << endl;
		//return true;


	    /* Check for simple, single character tokens */

	    case '*':
		c = cin.get();
		return MUL; //returning mul here since it comes before deref??
	    case '%':
	   	c = cin.get();
		return REM; // val is '%'	
	   
	    case ':':
	       	c = cin.get();
		return COL; // val is ':'
	    case ';':
		c = cin.get();
		return SEMIC; // val is ';'

	    case '(':
	       c = cin.get();
	       return LPAREN;       
	    case ')': 
	       c = cin.get();
	       return RPAREN;
	    case '[':
	       c = cin.get();
	       return LBRACK;
	    case ']':
	       c = cin.get();
	       return RBRACK;
	    case '{':
	       c = cin.get();
	       return LCURL;
	    case '}': 
	       c = cin.get();
	       return RCURL;
	    case '.': 
	       c = cin.get();
	       return DOT;
	    case ',':
		c = cin.get();
		return COMMA;
		//cout << "operator:" << lexbuf << endl;
		//return true;


	    /* Check for '/' or a comment */

	    case '/':
		c = cin.get();

		if (c == '*') {
		    do {
			while (c != '*' && !cin.eof()) {
			    if (c == '\n')
				lineno ++;

			    c = cin.get();
			}

			c = cin.get();
		    } while (c != '/' && !cin.eof());

		    c = cin.get();
		    break;

		} else {
		    return DIV; //val is '/' aka a division, not a comment
		    //cout << "operator:" << lexbuf << endl;
		    //return true;
		}


	    /* Check for a string literal */

	    case '"':
		do {
		    p = c;
		    c = cin.get();
		    lexbuf += c;
		} while ((c != '"' || p == '\\') && c != '\n' && !cin.eof());

		if (c == '\n' || cin.eof())
		    report("malformed string literal");

		c = cin.get();
		//cout << "string:" << lexbuf << endl;
		return STRING;


	    /* Check for a character literal */

	    case '\'':
		do {
		    p = c;
		    c = cin.get();
		    lexbuf += c;
		} while ((c != '\'' || p == '\\') && c != '\n' && !cin.eof());

		if (c == '\n' || cin.eof() || lexbuf.size() == 2)
		    report("malformed character literal");

		c = cin.get();
		//cout << "character:" << lexbuf << endl;
		return CHAR;


	    /* Handle EOF here as well */

	    case EOF:
		return ENDOFF; //at end of file


	    /* Everything else is illegal */

	    default:
		c = cin.get();
		break;
	    }
	}
    }
    
    return ENDOFF;//what do we return here?
}


/*
 * Function:	main
 *
 * Description:	Read and tokenize and standard input stream.
 */

