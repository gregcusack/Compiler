

#ifndef SCOPE_H
#define SCOPE_H
#include <vector>
#include "Symbol.h"
typedef std::vector<Symbol *> Symbols;

class Scope {
    Scope *_enclosing;
    Symbols _symbols;
    typedef std::string string;
 public:
    //Scope (Scope *enclosing = 0);
    Scope();
    Scope(Scope *enclosing);
    Scope *enclosing() const;
    const Symbols &symbols() const;
    void insert(Symbol *new_sym);
    Symbol *find(const string &name) const;
    Symbol *lookup(const string &name) const; //calls find in loop
};

//ostream& operator<<(ostream &ostr, const Scope &scope);

#endif /*Scope.h*/
