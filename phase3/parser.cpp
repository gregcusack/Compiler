/*
 * File:	parser.c
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include <string>
# include <stack>
# include "tokens.h"
# include "lexer.h"
//# include "Type.cpp"
//# include "Type.h"
//# include "Symbol.h"
# include "Scope.h"
//# include "Checker.h"
//# include "Type.cpp"
//# include "Symbol.cpp"
//# include "Symbol.cpp"
//# include "Scope.cpp"

using namespace std;

static int lookahead, nexttoken;
static string lexbuf, nextbuf;

static void expression();
static void statement();
string spec;
int ptrCount;

Scope *_innerScope = NULL;
Scope *_outerScope = NULL;

struct _structures {
    string name;
    bool complete;
};

#define STRUCTARR 100

_structures array[STRUCTARR];
int _structCounter = 0;
string globStruct;

/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

bool isDeclareVar(string name, Type &type) {
    if (_outerScope == _innerScope) { //at file scope level
	Symbol *x = _innerScope->lookup(name);
	if(x!=NULL) {
	    Type l = x->get_type();
	    if(l != type) {
		cout << "Error in func" << endl;
		report("conflicting types for '%s'", name);
		return false;
	    }
	    return false;
	}
	else 
	    return true;
    }
    else{
	//Symbol *x = _innerScope->lookup(name);
	Symbol *x = _innerScope->find(name);
	if(x != NULL) {
	    report("redecleration of '%s'", name);
	    return false;
	}
	else 
	    return true; //for now, need to modify this
    }
}


static void error()
{
    if (lookahead == DONE)
	report("syntax error at end of file");
    else
	report("syntax error at '%s'", lexbuf);

    exit(EXIT_FAILURE);
}


/*
 * Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
    if (lookahead != t)
	error();

    if (nexttoken) {
	lookahead = nexttoken;
	lexbuf = nextbuf;
	nexttoken = 0;
    } else
	lookahead = lexan(lexbuf);
}


/*
 * Function:	peek
 *
 * Description:	Return the next token in the input stream and save it so
 *		that match() will later return it.
 */

static int peek()
{
    if (!nexttoken)
	nexttoken = lexan(nextbuf);

    return nexttoken;
}


/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.
 */

static bool isSpecifier(int token)
{
    //cout << "Specifier: " << token << endl; 
    return token == INT || token == CHAR || token == STRUCT;
}


/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.  Simple C has only ints, chars, and
 *		structure types.
 *
 *		specifier:
 *		  int
 *		  char
 *		  struct identifier
 */

static string specifier()
{
    if (lookahead == INT) {
	match(INT);
	return "int";
    }

    if (lookahead == CHAR) {
	match(CHAR);
	return "char";
    }
    spec = lexbuf;
    match(STRUCT);
    cout << "Specifier: " << spec << " w/ name: " << lexbuf << endl;
    globStruct = lexbuf;
    match(ID);
    //cout << "Specifier: " << spec  << endl;
    return spec;
}


/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

int pointers()
{
    ptrCount = 0;
    while (lookahead == '*') {
	ptrCount++;
	match('*');
    }
    return ptrCount;
}


/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, with optional pointer declarators.
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ num ]
 */

static void declarator(string s ) //takes in specifier from declaration
{
    ptrCount = pointers();
    cout << "Declare: " << lexbuf << " as " << s;
    //add struct check here
    //
    cout << "here are pointers" << ptrCount << endl; 
    if(!(s.compare("struct"))) { //&& !(array[_structCounter].complete)) {
     	cout << "here1" << endl;
	//check to see if symbol table	    
        if(ptrCount == 0) {
	    cout << "I am here!!!!!!!!!!!1" << endl;
	    report("incomplete type for '%s'", lexbuf);
	}
    }
    string tempBuf = lexbuf;
    match(ID);

    if (lookahead == '[') {
	match('[');
	//cout << " array of size " << lexbuf;
	Type* _newType = new Type(spec, ptrCount, ARRAY, atoi(lexbuf.c_str())); //create array
	Symbol* _newSym = new Symbol(tempBuf, *_newType);
	bool b = isDeclareVar(tempBuf, *_newType);
	if(b)
	    _innerScope->insert(_newSym);
	match(NUM);
	match(']');
    } else {
	Type* _newType = new Type(spec, ptrCount, SCALAR, -1);
	Symbol* _newSym = new Symbol(tempBuf, *_newType);
	bool b = isDeclareVar(tempBuf, *_newType);
	if(b)
	    _innerScope->insert(_newSym);
    }
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ';'
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
    spec = specifier();
    declarator(spec);

    while (lookahead == ',') {
	match(',');
	declarator(spec);
    }

    match(';');
}


/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
    while (isSpecifier(lookahead))
	declaration();
}


/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *
 *		primary-expression:
 *		  ( expression )
 *		  identifier ( expression-list )
 *		  identifier ( )
 *		  identifier
 *		  character
 *		  string
 *		  num
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static void primaryExpression()
{
    if (lookahead == '(') {
	match('(');
	expression();
	match(')');

    } else if (lookahead == CHARACTER) {
	cout << "Use: " << lexbuf << endl;
	match(CHARACTER);

    } else if (lookahead == STRING) {
	cout << "Use: " << lexbuf << endl;
	match(STRING);

    } else if (lookahead == NUM) {
	cout << "Use: " << lexbuf << endl;
	match(NUM);

    } else if (lookahead == ID) {
	cout << "Use: " << lexbuf << endl;
	if (_innerScope->lookup(lexbuf) == NULL)
	    report("'%s' undeclared", lexbuf);
	match(ID);

	if (lookahead == '(') {
	    match('(');

	    if (lookahead != ')') {
		expression();

		while (lookahead == ',') {
		    match(',');
		    expression();
		}
	    }

	    match(')');
	}

    } else
	error();
}


/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 *		  postfix-expression . identifier
 *		  postfix-expression -> identifier
 */

static void postfixExpression()
{
    primaryExpression();

    while (1) {
	if (lookahead == '[') {
	    match('[');
	    expression();
	    match(']');
	    cout << "index" << endl;

	} else if (lookahead == '.') {
	    match('.');
	    match(ID);
	    cout << "dot" << endl;

	} else if (lookahead == ARROW) {
	    match(ARROW);
	    match(ID);
	    cout << "arrow" << endl;

	} else
	    break;
    }
}


/*
 * Function:	prefixExpression
 *
 * Description:	Parse a prefix expression.
 *
 *		prefix-expression:
 *		  postfix-expression
 *		  ! prefix-expression
 *		  - prefix-expression
 *		  * prefix-expression
 *		  & prefix-expression
 *		  sizeof prefix-expression
 *		  sizeof ( specifier pointers )
 */

static void prefixExpression()
{
    if (lookahead == '!') {
	match('!');
	prefixExpression();
	cout << "not" << endl;

    } else if (lookahead == '-') {
	match('-');
	prefixExpression();
	cout << "neg" << endl;

    } else if (lookahead == '*') {
	match('*');
	prefixExpression();
	cout << "deref" << endl;

    } else if (lookahead == '&') {
	match('&');
	prefixExpression();
	cout << "addr" << endl;

    } else if (lookahead == SIZEOF) {
	match(SIZEOF);

	if (lookahead == '(' && isSpecifier(peek())) {
	    match('(');
	    specifier();
	    pointers();
	    match(')');
	} else
	    prefixExpression();

	cout << "sizeof" << endl;

    } else
	postfixExpression();
}


/*
 * Function:	castExpression
 *
 * Description:	Parse a cast expression.  If the token after the opening
 *		parenthesis is not a type specifier, we could have a
 *		parenthesized expression instead.
 *
 *		cast-expression:
 *		  prefix-expression
 *		  ( specifier pointers ) cast-expression
 */

static void castExpression()
{
    if (lookahead == '(' && isSpecifier(peek())) {
	match('(');
	specifier();
	pointers();
	match(')');
	castExpression();
	cout << "cast" << endl;

    } else
	prefixExpression();
}


/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.
 *
 *		multiplicative-expression:
 *		  cast-expression
 *		  multiplicative-expression * cast-expression
 *		  multiplicative-expression / cast-expression
 *		  multiplicative-expression % cast-expression
 */

static void multiplicativeExpression()
{
    castExpression();

    while (1) {
	if (lookahead == '*') {
	    match('*');
	    castExpression();
	    cout << "mul" << endl;

	} else if (lookahead == '/') {
	    match('/');
	    castExpression();
	    cout << "div" << endl;

	} else if (lookahead == '%') {
	    match('%');
	    castExpression();
	    cout << "rem" << endl;

	} else
	    break;
    }
}


/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */

static void additiveExpression()
{
    multiplicativeExpression();

    while (1) {
	if (lookahead == '+') {
	    match('+');
	    multiplicativeExpression();
	    cout << "add" << endl;

	} else if (lookahead == '-') {
	    match('-');
	    multiplicativeExpression();
	    cout << "sub" << endl;

	} else
	    break;
    }
}


/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static void relationalExpression()
{
    additiveExpression();

    while (1) {
	if (lookahead == '<') {
	    match('<');
	    additiveExpression();
	    cout << "ltn" << endl;

	} else if (lookahead == '>') {
	    match('>');
	    additiveExpression();
	    cout << "gtn" << endl;

	} else if (lookahead == LEQ) {
	    match(LEQ);
	    additiveExpression();
	    cout << "leq" << endl;

	} else if (lookahead == GEQ) {
	    match(GEQ);
	    additiveExpression();
	    cout << "geq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static void equalityExpression()
{
    relationalExpression();

    while (1) {
	if (lookahead == EQL) {
	    match(EQL);
	    relationalExpression();
	    cout << "eql" << endl;

	} else if (lookahead == NEQ) {
	    match(NEQ);
	    relationalExpression();
	    cout << "neq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static void logicalAndExpression()
{
    equalityExpression();

    while (lookahead == AND) {
	match(AND);
	equalityExpression();
	cout << "and" << endl;
    }
}


/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static void expression()
{
    logicalAndExpression();

    while (lookahead == OR) {
	match(OR);
	logicalAndExpression();
	cout << "or" << endl;
    }
}


/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static void statements()
{
    while (lookahead != '}')
	statement();
}


/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  return expression ;
 *		  while ( expression ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  expression = expression ;
 *		  expression ;
 */

static void statement()
{
    if (lookahead == '{') {
	match('{');
	cout << "Open statement scope" << endl;
	Scope* _statementScope = new Scope(_innerScope);
	_innerScope = _statementScope;
	declarations();
	statements();
	match('}');
	cout << "Close statement scope" << endl;
	_innerScope = _statementScope->enclosing();
	delete _statementScope;

    } else if (lookahead == RETURN) {
	match(RETURN);
	expression();
	match(';');

    } else if (lookahead == WHILE) {
	match(WHILE);
	match('(');
	expression();
	match(')');
	statement();

    } else if (lookahead == IF) {
	match(IF);
	match('(');
	expression();
	match(')');
	statement();

	if (lookahead == ELSE) {
	    match(ELSE);
	    statement();
	}

    } else {
	expression();

	if (lookahead == '=') {
	    match('=');
	    expression();
	}

	match(';');
    }
}


/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always a scalar
 *		variable with optional pointer declarators.
 *
 *		parameter:
 *		  specifier pointers identifier
 */

static void parameter()
{
    spec = specifier();
    ptrCount = pointers();
    if(!(spec.compare("struct"))) {
	if (ptrCount == 0)
	    report("pointer type required for '%s'", lexbuf);
    }

    cout << "Function input param: " << spec << " " << lexbuf << " w/ " << ptrCount << " ptr(s)" << endl; 
    Type* _newType = new Type(spec, ptrCount, SCALAR, -1);
    Symbol* _newSym = new Symbol(lexbuf, *_newType);
    bool b = isDeclareVar(lexbuf, *_newType);
    if(b)
	_innerScope->insert(_newSym);
    match(ID);
}


/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  parameter-list
 *
 *		parameter-list:
 *		  parameter
 *		  parameter , parameter-list
 */

static void parameters()
{
    if (lookahead == VOID) {
	cout << "no parameters" << endl;
	match(VOID);
    }

    else {
	parameter();

	while (lookahead == ',') {
	    match(',');
	    parameter();
	}
    }
}


/*
 * Function:	topLevelDeclaration
 *
 * Description:	Parse a top level declaration, which is either a structure
 *		type definition, function definition, or global variable
 *		declaration.
 *
 *		type-definition:
 *		  struct identifier { declaration declarations } ;
 *
 *		global-declaration:
 *		  specifier global-declarator-list ;
 *
 *		global-declarator-list:
 *		  global-declarator
 *		  global-declarator , global-declarator-list
 *
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier ( )
 *		  pointers identifier [ num ]
 *
 *		function-definition:
 *		  specifier pointers identifier ( parameters ) { ... }
 */

static void topLevelDeclaration()
{
    spec = specifier();
    if (!(spec.compare("struct")) && lookahead == '{') {
	for(int i = 0; i<STRUCTARR; i++) {
	    if(globStruct == array[i].name) {
		report("redefenition of '%s'", globStruct);
	    }
	}
	match('{');
	
	array[_structCounter].name = globStruct;
	array[_structCounter].complete = false;

	cout << "Open Type-def struct scope" << endl;
	Scope* _structScope = new Scope(_innerScope);
	_innerScope = _structScope;
	declaration();
	declarations();
	match('}');
	array[_structCounter].complete = true; //this may not work
	cout << "Close Type-def struct scope" << endl;
	_innerScope = _structScope->enclosing();
	delete _structScope;
	match(';');

    } else {
	ptrCount = pointers();
	cout << "Declare: " << lexbuf << " as " << spec << " w/ " << ptrCount << " ptr(s) " << endl;
	cout << "here5" << endl;	
	string tempBuf = lexbuf; //ID name
	cout << lexbuf << ", " << ptrCount << endl << endl;
	if(!(spec.compare("struct")) && !(array[_structCounter].complete)) {
	    //check to see if symbol table
	    //if(array[_structCounter].name == lexbuf)	    
	    if(ptrCount == 0)
		report("incomplete type for '%s'", lexbuf);
	}/*
	else if(ptrCount == 0) {
	    report("pointer type required for '%s'", lexbuf);
	}*/
	match(ID);

	//Declare spec array with x ptrs --> needed below
	if (lookahead == '[') {
	    match('['); 
	    Type* _newType = new Type(spec, ptrCount, ARRAY, atoi(lexbuf.c_str())); //create array
	    Symbol* _newSym = new Symbol(tempBuf, *_newType);
	    bool b = isDeclareVar(tempBuf, *_newType);
	    if(b)
		_innerScope->insert(_newSym);
	    
	    match(NUM);
	    match(']');

	} else if (lookahead == '(') {

	    if(!(spec.compare("struct"))) {
		if (ptrCount == 0)
		    report("pointer type required for '%s'", tempBuf);
	   }
	   
	    match('(');

	    if (lookahead == ')') {//function decleration
		Type* _newType = new Type(spec, ptrCount, FUNCTION, -2);
		Symbol* _newSym = new Symbol(tempBuf, *_newType);
		//start a func below this
		Symbol* x = _innerScope->lookup(tempBuf);
		if (x!=NULL) { //found
		    if (x->get_type() != *_newType) {
			report("conflicting types for '%s'", tempBuf);
		    }
		}
		else
		    _innerScope->insert(_newSym);

		match(')');
	    }

	    else { //function defenition
		Type* _newType = new Type(spec, ptrCount, FUNCTION, -3);
		Symbol* _newSym = new Symbol(tempBuf, *_newType);
		//start function again (defFunc or something)
		Symbol *x = _innerScope->lookup(tempBuf);
		if(x != NULL) { //found
		    if(x->get_type() != *_newType) {
			report("conflicting types for '%s'", tempBuf);
		    }
		    else { //types match
			if(x->get_type().length() != -2) { //function decleration which is legal
			    report("redefenition of '%s'", tempBuf);
			}
		    }
		}
		else
		    _innerScope->insert(_newSym);


		cout << "Open Func-def scope" << endl;
		Scope* _funcScope = new Scope(_innerScope);
		_innerScope = _funcScope;
		//need to see values passed in to function

		parameters();
		match(')');
		match('{');
		//cout << "Open Func-def scope" << endl;
		declarations();
		statements();
		match('}');
		cout << "Close Func-def scope" << endl;
		_innerScope = _funcScope->enclosing(); //move innerscope up one so we can delete funcscope
		delete _funcScope;
		return;
	    }
	}
	else { //declaring just an int
	    Type* _newType = new Type(spec, ptrCount, SCALAR, -1);
	    Symbol* _newSym = new Symbol(tempBuf, *_newType);
	    bool b = isDeclareVar(tempBuf, *_newType);
	    if(b)
		_innerScope->insert(_newSym);
	}
	while (lookahead == ',') {
	    match(',');
	    ptrCount = pointers();
	    cout << "Declare: " << lexbuf << " as " << spec << " w/ " << ptrCount << " ptr(s) " <<  endl;
	    if(!(spec.compare("struct")) && !(array[_structCounter].complete)) {
		if(ptrCount == 0)
		    report("incomplete type for '%s'", lexbuf);
	    }/*
	    else if(ptrCount == 0) {
		report("pointer type required for '%s'", lexbuf);
	    }*/
	    string tempBuf = lexbuf;
	    match(ID);

	    if (lookahead == '[') {
		cout << "as array" << endl;
		match('[');
	    	Type* _newType = new Type(spec, ptrCount, ARRAY, atoi(lexbuf.c_str())); //create array
	    	Symbol* _newSym = new Symbol(tempBuf, *_newType);
		bool b = isDeclareVar(tempBuf, *_newType);
		if(b)
		    _innerScope->insert(_newSym);
		match(NUM);
		match(']');

	    } else if (lookahead == '(') {
	       	Type* _newType = new Type(spec, ptrCount, FUNCTION, -1);
		Symbol* _newSym = new Symbol(tempBuf, *_newType);
	    	bool b = isDeclareVar(tempBuf, *_newType);
	    	if(b)
		    _innerScope->insert(_newSym);
	   	
		match('(');
		match(')');
	    }else {
		Type* _newType = new Type(spec, ptrCount, SCALAR, -1);
	   	Symbol* _newSym = new Symbol(tempBuf, *_newType);
	    	bool b = isDeclareVar(tempBuf, *_newType);
	    	if(b)
		    _innerScope->insert(_newSym);
	    }
	}

	match(';');
    }
}


/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main()
{
    //report("conflicting types" ,"%s");
    lookahead = lexan(lexbuf);
    Scope* _fileScope = new Scope(NULL);
    _innerScope = _fileScope;
    _outerScope = _fileScope;
    cout << "Open file scope" << endl;
    while (lookahead != DONE)
	topLevelDeclaration();
    cout << "Close file scope" << endl;
    delete _fileScope;

    exit(EXIT_SUCCESS);
}
