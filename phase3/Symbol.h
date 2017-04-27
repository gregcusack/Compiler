
#ifndef SYMBOL_H
#define SYMBOL_H
#include "Type.h"
class Symbol {
    typedef std::string string;
    string _name;
    Type _type;
    
 public:
    const string &get_name() const;
    const Type &get_type() const;    
    
    //constructor
    Symbol(const string &name, Type &type);
};

//overload operator, make sure to comment out other << operators when you uncomment this one
std::ostream& operator<< (std::ostream &ostr, const Symbol &symbol);

#endif /*Symbol_H*/
