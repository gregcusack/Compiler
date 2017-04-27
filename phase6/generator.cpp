
/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */

# include <sstream>
# include <iostream>
# include "generator.h"
# include "machine.h"
# include "lexer.h"
using namespace std;
//int tempOffset;

int offset = 0;
static unsigned maxargs;
void assignTempOffset(Expression *expr);

struct Label {
    static unsigned counter;
    unsigned number;
    Label() {number = counter++;};
};
unsigned Label::counter = 0;

Label* funcLabelPtr = new Label();

ostream &operator<<(ostream &ostr, const Label &label) {
    ostr << ".L" << label.number;
    return ostr;
}


/*
 * Function:	operator <<
 *
 * Description:	Convenience function for writing the operand of an
 *		expression.
 */

ostream &operator <<(ostream &ostr, Expression *expr)
{
    return ostr << expr->_operand;
}

void Expression::generate() {
   cerr << "Oops, you didn't implement something" << endl;
}

void Expression::generate(bool &indirect) {
    indirect = false;
    generate();
}


/*
 * Function:	Identifier::generate
 *
 * Description:	Generate code for an identifier.  Since there is really no
 *		code to generate, we simply update our operand.
 */

void Identifier::generate()
{
    stringstream ss;


    if (_symbol->_offset != 0)
	ss << _symbol->_offset << "(%ebp)";
    else
	ss << global_prefix << _symbol->name();

    _operand = ss.str();
}


/*
 * Function:	Number::generate
 *
 * Description:	Generate code for a number.  Since there is really no code
 *		to generate, we simply update our operand.
 */

void Number::generate()
{
    stringstream ss;


    ss << "$" << _value;
    _operand = ss.str();
}

void Character::generate() {
    stringstream ss;
    ss << "$" << charval(_value);
    _operand = ss.str();
}

void String::generate() { 
    Label label;
    cout << "\t.data" << endl;
    cout << label << ":" << endl;
    cout << "\t.asciz\t" << _value << endl;
    cout << "\t.text" << endl;

    stringstream ss;
    ss << ".L" << label.number;
    _operand = ss.str();
}


# if STACK_ALIGNMENT == 4

/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call expression, in which each
 *		argument is simply a variable or an integer literal.
 */

void Call::generate()
{
    unsigned numBytes = 0;


    for (int i = _args.size() - 1; i >= 0; i --) {
	_args[i]->generate();
	cout << "\tpushl\t" << _args[i] << endl;
	numBytes += _args[i]->type().size();
    }

    cout << "\tcall\t" << global_prefix << _id->name() << endl;

    if (numBytes > 0)
	cout << "\taddl\t$" << numBytes << ",\t%esp" << endl;
    //cout << "Here" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

# else

/*
 * If the stack has to be aligned to a certain size before a function call
 * then we cannot push the arguments in the order we see them.  If we had
 * nested function calls, we cannot guarantee that the stack would be
 * aligned.
 *
 * Instead, we must know the maximum number of arguments so we can compute
 * the size of the frame.  Again, we cannot just move the arguments onto
 * the stack as we see them because of nested function calls.  Rather, we
 * have to generate code for all arguments first and then move the results
 * onto the stack.  This will likely cause a lot of spills.
 *
 * For now, since each argument is going to be either a number of in
 * memory, we just load it into %eax and then move %eax onto the stack.
 */

void Call::generate()
{
    if (_args.size() > maxargs)
	maxargs = _args.size();

    for (int i = _args.size() - 1; i >= 0; i --) {
	_args[i]->generate();
	cout << "\tmovl\t" << _args[i] << ", %eax" << endl;
	cout << "\tmovl\t%eax, " << i * SIZEOF_ARG << "(%esp)" << endl;
    }

    cout << "\tcall\t" << global_prefix << _id->name() << endl;

    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

# endif

void Field::generate() {
    bool indirect = false;
    this->generate(indirect);

    cout << "\tmovl\t" << this << ",\t%eax" << endl;
    if(this->_type.size() == 4) {
	cout << "\tmovl\t(%eax),\t%eax" << endl;
	assignTempOffset(this);
	cout << "\tmovl\t%eax,\t" << this << endl;
    }
    else {
	cout << "\tmovb\t(%eax),\t%al" << endl; //idk if this is right????
	cout << "\tmovsbl\t%al,\t%eax" << endl;
	assignTempOffset(this);
	cout << "\tmovl\t%eax,\t" << this << endl;
    }

}

void Field::generate(bool &indirect) {
    _expr->generate(indirect);
    if(indirect)
	cout << "\tmovl\t" << _expr << ",\t%eax" << endl;
    else
	cout << "\tleal\t" << _expr << ",\t%eax" << endl;
    assignTempOffset(this);
    //add offset of _id to %eax
    cout << "\taddl\t$" << _id->symbol()->_offset << ",\t%eax" << endl;
    cout << "\tmovl\t%eax,\t" << this << endl;

    indirect = true;

}



void Dereference::generate(bool &indirect) {
    indirect = true;
    _expr->generate();
    _operand = _expr->_operand;
}

void Dereference::generate() {
    _expr->generate();
    cout << "\tmovl\t" << _expr << ",\t%eax" << endl;
    if(_type.size() == 1)
	cout << "\tmovsbl\t(%eax),\t%eax" << endl;
    else
	cout << "\tmovl\t(%eax),\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void Address::generate() {
    bool indirect;
    _expr->generate(indirect);
    if(indirect)
	_operand = _expr->_operand;
    else {
	cout << "\tleal\t" << _expr << ",\t%eax" << endl;
	assignTempOffset(this);
	cout << "\tmovl\t%eax," << this << endl;
    }
}

void Not::generate() {
    _expr->generate();
    cout << "\tmovl\t" << _expr << ",\t%eax" << endl;
    cout << "\tcmpl\t$0,\t%eax" << endl;
    cout << "\tsete\t%al" << endl;
    cout << "\tmovzbl\t%al,\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void Negate::generate() {
    _expr->generate();
    cout << "\tmovl\t" << _expr << ",\t%eax" << endl;
    cout << "\tnegl\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax," << this << endl;
}

void Cast::generate() {
    _expr->generate();
    int SOURCE = _expr->type().size();  // DEST
    int DEST = this->_type.size();  // SOURCE
    if(DEST == 4 && SOURCE == 1) {
	assignTempOffset(this);
	cout << "\tmovb\t" << _expr << ",\t%al" << endl;
	cout << "\tmovsbl\t%al,\t%eax" << endl;
	assignTempOffset(this);
	cout << "\tmovl\t%eax,\t" << this << endl;
    }
    else if(DEST == 1 && SOURCE == 4) {
	assignTempOffset(this);
	//cout << "______________________" << endl;
	cout << "\tmovl\t" << _expr << ",\t%eax" << endl;
	//assignTempOffset(this);
	cout << "\tmovb\t%al,\t" << this << endl;
    }
    else if(DEST == 4 && SOURCE == 4) {
	assignTempOffset(this);
	cout << "\tmovl\t" << _expr << ",\t%eax" << endl;
	cout << "\tmovl\t%eax,\t" << this << endl;
    }
    else if(DEST == 1 && SOURCE == 1) {
	assignTempOffset(this);
	cout << "\tmovb\t" << _expr << ",\t%al" << endl;
	cout << "\tmovb\t%al,\t" << this << endl;
    }
}

void Multiply::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\timull\t" << _right << ",\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void Divide::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tmovl\t" << _right << ",\t%ecx" << endl;
    cout << "\tcltd" << endl;
    cout << "\tidivl\t%ecx" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void Remainder::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tmovl\t" << _right << ",\t%ecx" << endl;
    cout << "\tcltd" << endl;
    cout << "\tidivl\t%ecx" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%edx,\t" << this << endl;
}

void Add::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\taddl\t" << _right << ",\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void Subtract::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tsubl\t" << _right << ",\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void LessThan::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tcmpl\t" << _right << ",\t%eax" << endl;
    cout << "\tsetl\t%al" << endl;
    cout << "\tmovzbl\t%al,\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void GreaterThan::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tcmpl\t" << _right << ",\t%eax" << endl;
    cout << "\tsetg\t%al" << endl;
    cout << "\tmovzbl\t%al,\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void LessOrEqual::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tcmpl\t" << _right << ",\t%eax" << endl;
    cout << "\tsetle\t%al" << endl;
    cout << "\tmovzbl\t%al,\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void GreaterOrEqual::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tcmpl\t" << _right << ",\t%eax" << endl;
    cout << "\tsetge\t%al" << endl;
    cout << "\tmovzbl\t%al,\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void Equal::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tcmpl\t" << _right << ",\t%eax" << endl;
    cout << "\tsete\t%al" << endl;
    cout << "\tmovzbl\t%al,\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}

void NotEqual::generate() {
    _left->generate();
    _right->generate();
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tcmpl\t" << _right << ",\t%eax" << endl;
    cout << "\tsetne\t%al" << endl;
    cout << "\tmovzbl\t%al,\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}


void LogicalOr::generate() {
    _left->generate();
    _right->generate();

    Label label;
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tcmpl\t$0,\t" << "%eax" << endl;
    cout << "\tjne\t" << label << endl;
    cout << "\tmovl\t" << _right << ",\t%eax" << endl;
    cout << "\tcmpl\t$0," << "\t%eax" << endl;
    cout << label << ":" << endl << "\tsetne\t%al" << endl;
    cout << "\tmovzbl\t%al,\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;


}

void LogicalAnd::generate() {
    _left->generate();
    _right->generate();

    Label label;
    cout << "\tmovl\t" << _left << ",\t%eax" << endl;
    cout << "\tcmpl\t$0,\t" << "%eax" << endl;
    cout << "\tje\t" << label << endl;
    cout << "\tmovl\t" << _right << ",\t%eax" << endl;
    cout << "\tcmpl\t$0," << "\t%eax" << endl;
    cout << label << ":" << endl << "\tsetne\t%al" << endl;
    cout << "\tmovzbl\t%al,\t%eax" << endl;
    assignTempOffset(this);
    cout << "\tmovl\t%eax,\t" << this << endl;
}
   

/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for this assignment statement, in which the
 *		right-hand side is an integer literal and the left-hand
 *		side is an integer scalar variable.  Actually, the way
 *		we've written things, the right-side can be a variable too.
 */

void Assignment::generate()
{
    bool indirect;
    _left->generate(indirect);
    _right->generate();
    //cout << "HERE!" << endl;
    //cout << "\tmovl\t" << SIZEOF_ARG << "(%esp)" << ", %eax" << endl;
    //assignTempOffset(_right);
    cout << "\tmovl\t" << _right << ",\t%eax" << endl;
    //cout << "***********" << endl;
    if(indirect) {
	if(_left->type().size() == 4) {
	    //cout << "__________" << endl;
	    cout << "\tmovl\t" << _left << ",\t%ecx" << endl;
	    cout << "\tmovl\t" << "%eax,\t" << "(%ecx)" << endl;
	}
	else {
	    cout << "\tmovl\t" << _left << ",\t%ecx" << endl;
	    cout << "\tmovb\t" << "%al,\t" << "(%ecx)" << endl;
	}
    }
    else {
	if(_left->type().size() == 4) {
	    //cout << "HERE_int" << endl;
	    cout << "\tmovl\t" << "%eax,\t" << _left << endl;
	}
	else {
	    //cout << "here_char" << endl;
	    cout << "\tmovb\t" << "%al,\t" << _left << endl;
	}
    }
/*
    cout << "\tmovl\t" << _right << ", %eax" << endl;
    cout << "\tmovl\t%eax, " << _left << endl;
*/
}





void Return::generate() {
    _expr->generate();
    //cout << "here1" << endl;
    if(_expr->type().size() == 4) {
	cout << "\tmovl\t" << _expr << ",\t%eax" << endl;
    }
    else if(_expr->type().size() == 1) {
	cout << "\tmovb\t" << _expr << ",\t%al" << endl;
	cout << "\tmovsbl\t%al,\t%eax" << endl;
	//cout << "__________________" << endl;
	//cout << "\tmovl\t%eax,\t" << this << endl;

    }
    //cout << "\tmovl\t" << _expr->_operand << ",\t%eax" << endl;
    cout << "\tjmp\t" << *funcLabelPtr << endl;
    //jump to end of epilogue of current function
    //may need to pass label through all fcns to here
}




/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate()
{
    for (unsigned i = 0; i < _stmts.size(); i ++)
	_stmts[i]->generate();
}

void While::generate() {
    //_expr->generate();

    Label loop;
    Label exit;
    cout << loop << ":" << endl;
    _expr->generate();
    cout << "\tmovl\t" << _expr << ",\t%eax" << endl;
    cout << "\tcmpl\t$0,\t" << "%eax" << endl;
    //if equal exit
    cout << "\tje\t" << exit << endl;

    //generate statements in while block
    _stmt->generate();
     
    //expressions
    cout << "\tjmp\t" << loop << endl;
    cout << exit << ":" <<  endl;

}

void If::generate() {
    Label skip;
    Label exit;
    _expr->generate();
    cout << "\tmovl\t" << _expr << ",\t%eax" << endl;
    cout << "\tcmpl\t$0,\t" << "%eax" << endl;
    cout << "\tje\t" << skip << endl;
    
    _thenStmt->generate();
    if(_elseStmt == NULL){
	cout << skip << ":" << endl;
    }
    else {
	cout << "\tjmp\t" << exit << endl;
	cout << skip << ":" << endl;
	_elseStmt->generate();
    }

    cout << exit << ":" << endl;
}


/*
 * Function:	Function::generate
 *
 * Description:	Generate code for this function, which entails allocating
 *		space for local variables, then emitting our prologue, the
 *		body of the function, and the epilogue.
 */

void Function::generate()
{
    //int offset = 0;
    Label label;
    funcLabelPtr = &label;  //point global fcn ptr to new label

    /* Generate our prologue. */

    allocate(offset);
    //allocate(tempOffset);
    //tempOffset = offset;
    cout << global_prefix << _id->name() << ":" << endl;
    cout << "\tpushl\t%ebp" << endl;
    cout << "\tmovl\t%esp,\t%ebp" << endl;
    cout << "\tsubl\t$" << _id->name() << ".size, %esp" << endl << endl;


    /* Generate the body of this function. */

    maxargs = 0;
    _body->generate();

    offset -= maxargs * SIZEOF_ARG;

    while ((offset - PARAM_OFFSET) % STACK_ALIGNMENT)
	offset --;


    /* Generate our epilogue. */

    cout << endl << label << ":" << endl << "\tmovl\t%ebp,\t%esp" << endl;
    cout << "\tpopl\t%ebp" << endl;
    cout << "\tret" << endl << endl;

    cout << "\t.globl\t" << global_prefix << _id->name() << endl;
    cout << "\t.set\t" << _id->name() << ".size, " << -offset << endl;

    cout << endl;
}


/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(const Symbols &globals)
{
    if (globals.size() > 0)
	cout << "\t.data" << endl;

    for (unsigned i = 0; i < globals.size(); i ++) {
	cout << "\t.comm\t" << global_prefix << globals[i]->name();
	cout << ", " << globals[i]->type().size();
	cout << ", " << globals[i]->type().alignment() << endl;
    }
}


void assignTempOffset(Expression* expr) {
    stringstream ss;
    //use global tempOffset?
    offset -= expr->type().size();
    //cout << "here2" << endl;
    ss << offset << "(%ebp)";
    expr->_operand = ss.str();
}

