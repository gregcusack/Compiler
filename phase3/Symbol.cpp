#include "Symbol.h"
//#include "Type.cpp"
#include <iostream>
using namespace std;

const string &Symbol::get_name() const {
    return _name;
}

const Type &Symbol::get_type() const {
    return _type;
}

Symbol::Symbol(const string &name, Type &type) : _name(name), _type(type) {}


//error here!
//need to uncomment the Type.h << overload and make some changes
ostream& operator<< (ostream &ostr, const Symbol &symbol) {
    ostr << "(" << symbol.get_name() <<  ", " << symbol.get_type() << " )" <<endl;
    return ostr;
}
/*
int main (void) {
    Type t("int", 1, SCALAR, -1);
    Symbol s("x", t);
    cout << s;// << t;

    Type x("int", 1, ARRAY, 10);
    Symbol r("m", x);
    cout << r;
    return 0;
}*/
