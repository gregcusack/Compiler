#include "Scope.h"
//#include "Symbol.cpp"
//#include "Type.cpp"
using namespace std;

//Scope::Scope (Scope *enclosing = 0);

Scope *_outerMost;
/*
Scope::Scope() {
    Scope *_enclosing = NULL;
}*/

Scope::Scope(Scope *enclosing) : _enclosing(enclosing) {
    //_outerMost = this;
}

//returns enclosing scope
Scope *Scope::enclosing() const{
    return _enclosing;
}

//Create vector full of symbol addresses
const Symbols &Scope::symbols() const {
    /*Symbols *new_scope;
    return *new_scope;*/
    return _symbols;
}
//Insert a pointer to a symbol in the vector
void Scope::insert(Symbol *new_sym) {
    _symbols.push_back(new_sym);
}


//Searches given scope (used in a decl. or a variable)
Symbol *Scope::find(const string &name) const {
    //cout << "Looking for name" << endl;
    for(Symbols::size_type i = 0; i < _symbols.size(); i++) {
	//cout << "name iteration" << endl;
	if(name == _symbols[i]->get_name()) {
	    //cout << "Found Symbol name: " << name << endl;
	    return _symbols[i];
	}
	//cout << "Look again" << endl;
    }
    //case if we don't find it
    //In checker, we need to check if not NULL
    //cout << "Symbol not found: " << name << endl;
    return NULL;   
}

//Searches given scope and all enclosing scopes (used in a use of variable)
Symbol *Scope::lookup(const string &name) const { 
    cout << "here10" << endl;
    //Scope *temp = _outerMost;
    Scope *temp;
    if(enclosing() == NULL) {
	//cout << "In File scope, found: " << name << endl;
	return find(name);
    }
    else {
	Symbol *x = find(name);
	//cout << "just looked" << endl;
	if(x == NULL) {
	    temp = enclosing();
	    return temp->lookup(name);   
	}
	else {
	    return x;
	}
    }
}

/*
int main(void) {
    Type t("int", 1, SCALAR, -1);
    Type t1("int", 0, ARRAY, 10);
    Type t2("char", 3, FUNCTION, -1);
    Type t3("int", 1, SCALAR, -1);

    Symbol s("x", t);
    Symbol s1("y", t1);
    Symbol s2("z", t2);

    Type *t = new Type("int", 1, SCALAR, -1);
    Symbol *s = new Symbol("x", *t);
    cout << *s;
    //cout << s1;
    //cout << s2;
    
    Scope* x = new Scope(NULL);
    Scope* x1 = new Scope(x);
    Scope* x2 = new Scope(x1);
    x->insert(s);
    //x.insert(&s);
    //x.insert(&s);
    //x1.insert(&s1);
    //x2->insert(&s2);
    
    //x.lookup(s1.get_name());
    x2->lookup(s->get_name());
    //cout << "Next" << endl;
    //x2->lookup(s1.get_name());
    cout << "Next1" << endl;
    //x2->lookup(s2.get_name());
    cout << "Done" << endl;
    delete t, s, x;
    delete x1;
    delete x2;
    
    return 0;
}*/
