/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <map>
# include <iostream>
# include "lexer.h"
# include "checker.h"
# include "nullptr.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

static map<string, Scope*> fields;
static Scope *outermost, *toplevel;
static const Type error;

static string undeclared = "'%s' undeclared";
static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string incomplete = "incomplete type for '%s'";
static string nonpointer = "pointer type required for '%s'";

/*
Type integer("int");
Type character("char");
*/

/*
 * Function:	checkIfComplete
 *
 * Description:	Check if the given type is complete.  A non-structure type
 *		is always complete.  A structure type is complete if its
 *		fields have been defined.
 */
static Type checkIfComplete(const string &name, const Type &type)
{
    if (!type.isStruct() || type.indirection() > 0)
	return type;

    if (fields.count(type.specifier()) > 0)
	return type;

    report(incomplete, name);
    return error;
}

static bool checkCompleteness(const string &name, const Type &type) {
    if (!type.isStruct() || type.indirection() > 0)
	return true;

    if (fields.count(type.specifier()) > 0)
	return true;
    return false;
}

/*
 * Function:	checkIfStructure
 *
 * Description:	Check if the given type is a structure.
 */

static Type checkIfStructure(const string &name, const Type &type)
{
    if (!type.isStruct() || type.indirection() > 0)
	return type;

    report(nonpointer, name);
    return error;
}


/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;

    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	defineStructure
 *
 * Description:	Define a structure with the specified NAME and whose fields
 *		are specified by SCOPE.  A structure can be defined only
 *		once.
 */

void defineStructure(const string &name, Scope *scope)
{
    if (fields.count(name) > 0) {
	report(redefined, name);
	delete scope;
    } else
	fields[name] = scope;
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.  This
 *		definition always replaces any previous definition or
 *		declaration.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (symbol != nullptr) {
	if (symbol->type().isFunction() && symbol->type().parameters()) {
	    report(redefined, name);
	    delete symbol->type().parameters();

	} else if (type != symbol->type())
	    report(conflicting, name);

	outermost->remove(name);
	delete symbol;
    }

    symbol = new Symbol(name, checkIfStructure(name, type));
    outermost->insert(symbol);

    return symbol;
}


/*
 * Function:	declareFunction
 *
 * Description:	Declare a function with the specified NAME and TYPE.  A
 *		function is always declared in the outermost scope.  Any
 *		redeclaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, checkIfStructure(name, type));
	outermost->insert(symbol);

    } else if (type != symbol->type()) {
	report(conflicting, name);
	delete type.parameters();
    }

    return symbol;
}


/*
 * Function:	declareParameter
 *
 * Description:	Declare a parameter with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.  The only difference between
 *		declaring a parameter and a variable is that a parameter
 *		cannot be a structure type.
 */

Symbol *declareParameter(const string &name, const Type &type)
{
    return declareVariable(name, checkIfStructure(name, type));
}


/*
 * Function:	declareVariable
 *
 * Description:	Declare a variable with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, checkIfComplete(name, type));
	toplevel->insert(symbol);

    } else if (outermost != toplevel)
	report(redeclared, name);

    else if (type != symbol->type())
	report(conflicting, name);

    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
	report(undeclared, name);
	symbol = new Symbol(name, error);
	toplevel->insert(symbol);
    }

    return symbol;
}

Type checkLogicalOp(const Type &left, const Type &right, const string &op) {
    if(left == Type() || right == Type())
	    return Type();
    if(left.isSimple() && right.isSimple())
	return Type("int", 0);
    report("invalid operands to binary %s", op);
    return Type();
}


Type checkEqualExp(const Type &left, const Type &right, const string &op) {
    if(left == Type() || right == Type()) {
	cout << "Error type in ==/!=" << endl;
	return Type();
    }
    /*if(left.promote() == right.promote())
	return Type("int", 0);*/
    if(left.isCompatible(right))
	return Type("int", 0);
    report("invalid operands to binary %s", op);
    return Type();
}

Type checkRelationExp(const Type &left, const Type &right, const string &op) {
    if(left == Type() || right == Type())
	return Type();
    /*if(left.promote() == right.promote())
	return Type("int", 0);*/
    if(left.isCompatible(right))
	return Type("int", 0);
    report("invalid operands to binary %s", op);
    return Type();
}

Type checkMultDivExp(const Type &left, const Type &right, const string &op) {
    if(left == Type() || right == Type())
	return Type();
    Type l = left.promote();
    Type r = right.promote();
    if(l.specifier() == "int" && r.specifier() == "int")
	return Type("int", 0); //what indirection do i return here?
    report ("invalid operands to binary %s", op);
    return Type();
}

Type checkAddSubExp(const Type &left, const Type &right, const string &op) {
    if(left == Type() || right == Type()){
	cout << "Error Type in +/-" << endl;
	return Type();
    }
    Type l = left.promote();
    Type r = right.promote();
    //check for completeness
    if(l.indirection() > 0 && l.isStruct()) {
	if(!checkCompleteness(l.specifier(), l)) {
	    report("using pointer to incomplete type");
	    return Type();
	}
    }
    //Need to check completeness of T! only need to check if dealing with a struct
    if((l.specifier() == "int" && r.specifier() == "int") && (l.indirection() == 0 && r.indirection() == 0)){
	cout << "int x int -> int" << endl;
	return Type("int", 0);
    }
    if(l.indirection() > 0 && (r.specifier() == "int" && r.indirection() == 0)) {
	cout << "ptr(a) x int -> ptr(a)" << endl;
	return Type(l.specifier(), l.indirection()); //do we return l's indirection???
    }
    if(op == "+") {
	if((l.specifier() == "int" && l.indirection() == 0) && r.indirection() > 0) {
	    cout << "int x ptr(a) -> ptr(a)" << endl;
	    return Type(r.specifier(), r.indirection());
	}
    }
    if(op == "-") {
	if(l.indirection() > 0 && r.indirection() > 0)
	    cout << "ptr(a) x ptr(a) -> int" << endl;
	    return Type("int", 0); //just returning distance between two addresses
    }
    report("invalid operands to binary %s", op);
    return Type();
}

Type checkPrefixExp(const Type &right, const bool &lvalue, const string &op) {
    if(right == Type())
	return Type();
    Type r = right.promote();
    if(op == "*") {
	if(r.indirection() == 0) { //also need to check here if T is complete! need name
	    report("invalid operand to unary %s", op);
	    return Type();
	}
	if(!checkCompleteness(r.specifier(), r)) {
	    report("using pointer to incomplete type");
	    return Type();
	}
	return Type(r.specifier(), r.indirection()-1); //can this be an array or function??
    }
    if(op == "&") {
	if(!lvalue) {
	    report("lvalue required in expression");
	    return Type();
	}
	return Type(right.specifier(), right.indirection()+1);
    }
    if(op == "!") {
	if(r.isSimple()) {
	    return Type("int", 0);
	}
	report("invalid operand to unary %s", op);
	return Type();
    }
    if(op == "-") {
	if(r.specifier() == "int" && r.indirection() == 0) {
	    return Type("int", 0);
	}
	report("invalid operand to unary %s", op);
	return Type();
    }
    if(op == "sizeof") { //does this need to be enumerated number??
	if(!r.isFunction() && checkCompleteness(r.specifier(), r)) { //also need to check completeness!!!!
	    return Type("int", 0);
	}
	report("invalid operand in sizeof expression");
	return Type();
    }
    cout << "returrning error" << endl;
    return Type(); //is this right?? dont know if i have to return anything else
}

Type checkCastExp(const Type &right, const string &specifier, const int &pointers) {
    if(right == Type())
	return Type();
    if(right.isSimple()) {
	if(!specifier.compare("char") || !specifier.compare("int") || right.isStruct())
	    return Type(specifier, pointers);
    }
    report("invalid operand in cast expression");
    return Type();
}

Type checkAssignExp(const Type &left, const Type &right, const bool &lvalue) {
    if(left == Type() || right == Type())
	return Type();
    cout << right << endl<< endl;
    cout << left << endl << endl;
    if(!lvalue) { //check to see if lhs is lvalue
	report("lvalue required in expression");
	return Type();
    }
    if(left.isCompatible(right)) {
	cout << "compatable" << endl;
	return Type(right.specifier(), right.indirection());
    }
    //report("here");
    report("invalid operands to binary =");
    return Type();
}

Type checkArrayExp(const Type &left, const Type &exp, const string &op) {
    if(left == Type()) {
	report("Array Error");
	return Type();
    }
    Type l = left.promote();
    Type e = exp.promote();
    if(l.indirection() == 0) {
	report("invalid operands to binary %s", op);
	return Type();
    }
    if(!checkCompleteness(l.specifier(), l)) {
	    report("using pointer to incomplete type");
	    return Type();
    }
    if(e.specifier() == "int" && e.indirection() == 0) {
	return Type(l.specifier(), l.indirection()-1); //need to figure out what i put here for T
    }
    report("invalid operands to binary %s", op);
    return Type();
}

Type checkDirectRefExp(const Type &left, const string &id) {
    if(left == Type())
	return Type();
    if(left.isStruct()) {
    	if(!checkCompleteness(left.specifier(), left)) {
    	    report("using pointer to incomplete type");
    	    return Type();
	}
	if(fields.count(left.specifier()) > 0) {
	    Symbol *s = fields[left.specifier()]->find(id);
	    if(s == nullptr) {
		report("invalid operands to binary .");
		return Type();
	    }
	    return s->type();
	}
    }
    report("invalid operands to binary .");
    return Type();
}

Type checkIndirectRefExp(const Type &left, const string &id) {
    if(left == Type())
	return Type();
    Type l = left.promote();
    if(l.indirection() == 0 || !l.isStruct() || !checkCompleteness(l.specifier(), l)) {
	report("using pointer to incomplete type");
	return Type();
    }
    if(fields.count(l.specifier()) > 0) {
	Symbol *s = fields[l.specifier()]->find(id);
	if(s == nullptr) {
	    report("invalid operands to binary ->");
	    return Type();
	}
	return s->type();
    }
    report("invalid operands to binary ->");
    return Type();
}


Type checkReturnExp(const Type &exp, const Type &returnType) {
    if(exp == Type())
	return Type();
    if(!exp.isCompatible(returnType)){
	report("invalid return type");
	return Type();
    }
    return exp;
}


Type checkArgList(const vector<Type> &argList, const Symbol *s, const Type &t) {
    //need to check if decleration
    cout << "In Arg List Checker" << endl;
    if(!t.isFunction()) {
	report("called object is not a function");
	return Type();
    }
    //cout << "here: " << s->type().parameters()->size() << endl;
    if(s->type().parameters() == nullptr) { //if its a declaration
	for(unsigned i=0; i<argList.size(); i++) {
	    if(!argList[i].isSimple()) {
		//report("here");
		report("invalid arguments to called function");
		return Type();
	    }
	}
    }
    else { //if its a definition
	if(argList.size() != s->type().parameters()->size()) {
	    report("invalid arguments to called function");
	    return Type();
	}
	for(unsigned i=0; i<argList.size();i++){
	    if(!argList[i].isCompatible((*s->type().parameters())[i])) {
		//report("here");
		report("invalid arguments to called function");
		return Type();
	    }
	}
    }

    return Type(t.specifier(), t.indirection());
}    

Type checkIfWhileExp(const Type &exp) {
   if(exp == Type())
      return Type();
   if(!exp.isSimple()) {
       report("invalid type for test expression");
       return Type();
   }
   return exp;
}












