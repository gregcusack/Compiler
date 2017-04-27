/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"

Scope *openScope();
Scope *closeScope();

void defineStructure(const std::string&name, Scope *scope);

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareParameter(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);

Type checkLogicalOp(const Type &left, const Type &right, const std::string &op);
Type checkEqualExp(const Type &left, const Type &right, const std::string &op);
Type checkRelationExp(const Type &left, const Type &right, const std::string &op);
Type checkMultDivExp(const Type &left, const Type &right, const std::string &op);
Type checkAddSubExp(const Type &left, const Type &right, const std::string &op);
Type checkPrefixExp(const Type &right, const bool &lvalue, const std::string &op);
Type checkCastExp(const Type &right, const std::string &specifier, const int &pointers);
Type checkArrayExp(const Type &left, const Type &exp, const std::string &op);
Type checkDirectRefExp(const Type &left, const std::string &id);
Type checkIndirectRefExp(const Type &left, const std::string &id);

//Statement Checks
Type checkAssignExp(const Type &left, const Type &right, const bool &lvalue);
Type checkReturnExp(const Type &exp, const Type &returnType);
Type checkIfWhileExp(const Type &exp);

Type checkArgList(const std::vector<Type> &argList, const Symbol *s, const Type &t);


/*
Type integer("int");
Type character("char");
*/

# endif /* CHECKER_H */
