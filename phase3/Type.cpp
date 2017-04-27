#include "Type.h"

using namespace std;

unsigned Type::indirection() const {
    return _indirection;
}

Kind Type::kind() const {
    return _kind;
}

const string &Type::specifier() const {
    return _specifier;
}

int Type::length() const {
    return _length;
}

//constructor
Type::Type(const string &specifier, unsigned indirection, Kind kind, int length) : _specifier(specifier), _indirection(indirection), _kind(kind), _length(length) {}

bool Type::operator!=(const Type &that) const {
    return !operator==(that);
}

bool Type::operator==(const Type &that) const {
    if(_kind != that._kind)
	return false;
    if(_specifier != that._specifier)
	return false;
    if(_indirection != that._indirection)
	return false;
    if(_kind == ARRAY && _length != that._length)
	return false;
    return true;
}


//overload << operator
ostream& operator<< (ostream &ostr, const Type &type) {
    if (type.kind() == ARRAY) 
	ostr << "(" << type.specifier() << ", " << type.indirection() << ", " << type.kind() << ", " << type.length() << ")";
    else
	ostr << "(" << type.specifier() << ", " << type.indirection() << ", " << type.kind() << ")";  //we can print out whatever we want here, so edit this as needed	
	return ostr;
}
/*
int main(void) {
    Type t("INT", 1, SCALAR, -1);
    cout << t;
    Type x("INT", 2, ARRAY, 10);
    cout << x;
}*/







