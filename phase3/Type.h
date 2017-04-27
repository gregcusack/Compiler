#ifndef TYPE_H
#define TYPE_H
#include <string>
#include <iostream>

enum Kind {
    SCALAR,
    ARRAY,
    FUNCTION
};

class Type {
    typedef std::string string;
    string _specifier;
    unsigned _indirection;
    Kind _kind;
    int _length;
 public:
    unsigned indirection() const;
    Kind kind() const;
    const string &specifier() const;
    int length() const;
    
    //constructor
    Type(const string &specifier, unsigned indirection, Kind kind, int length);

    bool operator==(const Type &that) const;
    bool operator!=(const Type &that) const;



};
    //overload << operator
    std::ostream& operator<<(std::ostream &ostr, const Type &type);

#endif /*TYPE_H*/
