/*
 * File:	Type.cpp
 *
 * Description:	This file contains the member function definitions for
 *		types in Simple C.  A type is either a scalar type, an
 *		array type, or a function type.
 *
 *		Note that we simply don't like putting function definitions
 *		in the header file.  The header file is for the interface.
 *		Actually, we prefer opaque pointer types in C where you
 *		don't even get to see what's inside, much less touch it.
 *		But, C++ lets us have value types with access control
 *		instead of just always using pointer types.
 *
 *		Extra functionality:
 *		- equality and inequality operators
 *		- predicate functions such as isArray()
 *		- stream operator
 *		- the error type
 */

# include <cassert>
# include "tokens.h"
# include "Type.h"
# include <iostream>
using std::string;
using std::ostream;


/*
 * Function:	Type::Type (constructor)
 *
 * Description:	Initialize this type as an error type.
 */

Type::Type()
    : _specifier("-error-"), _kind(ERROR)
{
}


/*
 * Function:	Type::Type (constructor)
 *
 * Description:	Initialize this type object as a scalar type.
 */

Type::Type(const string &specifier, unsigned indirection)
    : _specifier(specifier), _indirection(indirection), _kind(SCALAR)
{
}


/*
 * Function:	Type::Type (constructor)
 *
 * Description:	Initialize this type object as an array type.
 */

Type::Type(const string &specifier, unsigned indirection, unsigned long length)
    : _specifier(specifier), _indirection(indirection), _length(length)
{
    _kind = ARRAY;
}


/*
 * Function:	Type::Type (constructor)
 *
 * Description:	Initialize this type object as a function type.
 */

Type::Type(const string &specifier, unsigned indirection, Parameters *parameters)
    : _specifier(specifier), _indirection(indirection), _parameters(parameters)
{
    _kind = FUNCTION;
}


/*
 * Function:	Type::operator ==
 *
 * Description:	Return whether another type is equal to this type.  The
 *		parameter lists are checked for function types, which C++
 *		makes so easy.  (At least, it makes something easy!)
 */

bool Type::operator ==(const Type &rhs) const
{
    if (_kind != rhs._kind)
	return false;

    if (_kind == ERROR)
	return true;

    if (_specifier != rhs._specifier)
	return false;

    if (_indirection != rhs._indirection)
	return false;

    if (_kind == SCALAR)
	return true;

    if (_kind == ARRAY)
	return _length == rhs._length;

    if (!_parameters || !rhs._parameters)
	return true;

    return *_parameters == *rhs._parameters;
}


/*
 * Function:	Type::operator !=
 *
 * Description:	Well, what do you think it does?  Why can't the language
 *		generate this function for us?  Because they think we want
 *		it to do something else?  Yeah, like that'd be a good idea.
 */

bool Type::operator !=(const Type &rhs) const
{
    return !operator ==(rhs);
}


/*
 * Function:	Type::isArray
 *
 * Description:	Return whether this type is an array type.
 */

bool Type::isArray() const
{
    return _kind == ARRAY;
}


/*
 * Function:	Type::isScalar
 *
 * Description:	Return whether this type is a scalar type.
 */

bool Type::isScalar() const
{
    return _kind == SCALAR;
}


/*
 * Function:	Type::isFunction
 *
 * Description:	Return whether this type is a function type.
 */

bool Type::isFunction() const
{
    return _kind == FUNCTION;
}


/*
 * Function:	Type::isStruct
 *
 * Description:	Return whether this type has a struct specifier.
 */

bool Type::isStruct() const
{
    return _kind != ERROR && _specifier != "int" && _specifier != "char";
}


/*
 * Function:	Type::isError
 *
 * Description:	Return whether this type is an error type.
 */

bool Type::isError() const
{
    return _kind == ERROR;
}


/*
 * Function:	Type::specifier (accessor)
 *
 * Description:	Return the specifier of this type.
 */

const string &Type::specifier() const
{
    return _specifier;
}


/*
 * Function:	Type::indirection (accessor)
 *
 * Description:	Return the number of levels of indirection of this type.
 */

unsigned Type::indirection() const
{
    return _indirection;
}


/*
 * Function:	Type::length (accessor)
 *
 * Description:	Return the length of this type, which must be an array
 *		type.  Is there an better way than calling assert?  There
 *		certainly isn't an easier way.
 */

unsigned long Type::length() const
{
    assert(_kind == ARRAY);
    return _length;
}


/*
 * Function:	Type::parameters (accessor)
 *
 * Description:	Return the parameters of this type, which must be a
 *		function type.
 */

Parameters *Type::parameters() const
{
    assert(_kind == FUNCTION);
    return _parameters;
}



/*
 * Function:	operator <<
 *
 * Description:	Write a type to the specified output stream.  At least C++
 *		let's us do some cool things.
 */

ostream &operator <<(ostream &ostr, const Type &type)
{
    ostr << type.specifier();

    if (type.indirection() > 0) {
	ostr << " ";

	for (unsigned long i = 0; i < type.indirection(); i ++)
	    ostr << "*";
    }

    if (type.isArray())
	ostr << "[" << type.length() << "]";

    else if (type.isFunction())
	ostr << "()";

    return ostr;
}

Type Type::promote() const {
    std::cout << "In promote" << std::endl;
    if(_kind == SCALAR && _indirection == 0 && _specifier == "char") {
	//return integer;
	std::cout << "promoting" << std::endl;
	return Type("int", 0);
    }
    if(_kind == ARRAY)
	return Type(_specifier, _indirection+1);
    std::cout << "No need to promot" << std::endl;
    return *this;
}

bool Type::isSimple() const {
    Type t = promote();
    std::cout << "Type: " << t << std::endl;
    std::cout << "here1" << std::endl;
    if(t._kind==SCALAR && t._indirection == 0 && t._specifier == "int"){
	std::cout << "returning scalar,0,int" << std::endl;
	return true;
    }
    std::cout << "here2" << std::endl;
    return (t._kind == SCALAR && t._indirection > 0);
}

bool Type::isCompatible(const Type &right) const {
    /*if(this->isSimple() && right.isSimple()) {
	if(*this == right)
	    return true;
    }
    return false;
    */
    std::cout << *this << std::endl;
    Type l = this->promote();
    std::cout << "after promote L: " << l << std::endl;
    std::cout << right << std::endl;
    Type r = right.promote();
    std::cout << r << std::endl;
    if(l._kind == SCALAR && r._kind == SCALAR) {
	//std::cout << "here3" << std::endl;
	if(l._indirection == 0 && r._indirection == 0) {
	    //std::cout << "here4" << std::endl;
	    if(l._specifier == "int" && r._specifier == "int"){
		//std::cout << "here5" << std::endl;
		return true;
	    }
	    //std::cout << "here6" << std::endl;
	    return false;
	}
	if(l._indirection == r._indirection) {
	    if(l._specifier == r._specifier) 
		return true;
	    return false;
	}
	//std::cout << "here8" << std::endl;
	return false;
    }
    //std::cout << "here9" << std::endl;
    return false;
}


