/* Greg Cusack
 * COEN 175: Compilers
 * 1/20/16
 * Compiler Phase 2
 * Parser
 */

#include <cstdio>
#include <cctype>
#include <string>
#include <iostream>
#include <stdio.h>
#include "lexer.h"
#include "lexer.cpp"

using namespace std;

//test
int count = 0;

string lexbuf, lexbuf2;
int la = lexan(lexbuf);
void match(int x);
void matchID(void);
void matchOR(void);
void matchAND(void);
void matchEQUIV(void);
void matchGOLTHN(void);
void matchAddSub(void);
void matchMultDivRem(void);
void matchCast(void);
void matchUnaryOp(void);
void matchIndDotArr(void);
void matchFinal(void);
void matchExpressionList(void);

void matchSpecifier(void);
void matchPtrs(void);
void next_token(void);
int nextToken;
bool peekFlag = false;

void topLevel(void);
void matchExpression(void);
void matchFuncDef(void);
void matchParameters(void);
void matchParamList(void);
void matchParam(void);
void matchDeclarations(void);
void matchDeclaration(void);
void matchDecTORList(void);
void matchDeclaraTOR(void);
void matchStatements(void);
void matchStatement(void);
void matchGlobDecList(void);
void matchGlobDec(void);

void next_token(void) {
    nextToken = lexan(lexbuf2);
    peekFlag = true; 
}

void matchSpecifier() {
    if(la == INT) 
	match(INT);   
    else if(la == CHAR)
	match(CHAR);
    else if(la == STRUCT){
	match(STRUCT);
	match(ID);
    }
    else
	cout << "Error: Expected Specifier" << endl;
}

void matchPtrs(void) {
    while(la == DEREF || la == MUL) {
	if (la == DEREF) 
	    match(DEREF); 
	else
	    match(MUL);
	}
}

void match(int x) {
    if (!peekFlag) {
	if (la == x) {
	    la = lexan(lexbuf);
	}
	else
	    cout << "ERROR" << endl;
    }
    else {
	if(la == x) {
	    la = nextToken;
	    peekFlag = false;
	}
	else{
	    cout << "ERROR!!!" << endl;
	    peekFlag = false;
	}
    }
}

void matchExpression(void) {
    matchOR();
}

void matchOR(void) {
    matchAND();
    while(la == OR) {
	match(OR);
	matchAND();
	cout << "or" << endl;
    }
}

void matchAND(void) {
    matchEQUIV();
    while(la == AND) {
        match(AND);
	matchEQUIV();
	cout << "and" << endl;
    }
}

void matchEQUIV(void) {
    matchGOLTHN(); //greater or less than functions
    while (1) {
	if(la == EQL) {
	    match(EQL);
	    matchGOLTHN();
	    cout << "eql" << endl;
	}
	else if (la == NEQ) {
	    match(NEQ);
	    matchGOLTHN();
	    cout << "neq" << endl;
	}
	else
	    break;
    }
}

void matchGOLTHN(void) {
    matchAddSub();
    while (1) {
	if (la == LTN) {
	    match(LTN);
	    matchAddSub();
	    cout << "ltn" << endl;
	}
	else if (la == GTN) {
	    match(GTN);
	    matchAddSub();
	    cout << "gtn" << endl;
	}
	else if (la == LEQ) {
	    match(LEQ);
	    matchAddSub();
	    cout << "leq" << endl;
	}
	else if (la == GEQ) {
	    match(GEQ);
	    matchAddSub();
	    cout << "geq" << endl;
	}
	else
	    break;
    }
}

void matchAddSub(void) {
    matchMultDivRem();
    while(1) {
	if (la == ADD) {
	    match(ADD);
	    matchMultDivRem();
	    cout << "add" << endl;
	}
	else if (la == SUB) {
	    match(SUB);
	    matchMultDivRem();
	    cout << "sub" << endl;
	}
	else
	    break;
    }
}

void matchMultDivRem(void) {
    matchCast();
    while(1) {
	if (la == MUL) {
	    match(MUL);
	    matchCast();
	    cout << "mul" << endl;
	}
	else if (la == DIV) {
	    match(DIV);
	    matchCast();
	    cout << "div" << endl;
	}
	else if (la == REM) {
	    match(REM);
	    matchCast();
	    cout << "rem" << endl;
	}
	else
	    break;
    }
}


void matchCast(void) {
    if (!peekFlag) 
	next_token();
    if (la == LPAREN && (nextToken == INT || nextToken == CHAR || nextToken == STRUCT)) {
       if(nextToken == INT) { 
	   match(LPAREN);
	   matchSpecifier();
	   matchPtrs();
	   match(RPAREN);
	   matchCast();
	   cout << "cast" << endl;
       }
       else if (nextToken == CHAR) {
	   match(LPAREN);
	   matchSpecifier();
	   matchPtrs();
	   match(RPAREN);
	   matchCast();
	   cout << "cast" << endl;
       }
       else if (nextToken == STRUCT) {
	   match(LPAREN);
	   matchSpecifier();
	   matchPtrs();
	   match(RPAREN);
	   matchCast();
	   cout << "cast" << endl;
       }
    }
    else
	matchUnaryOp();
}


void matchUnaryOp(void) {
       if (la == ADDR) {
	   match(ADDR);
	   matchUnaryOp();
	   cout << "addr" << endl;
       }
       else if( la == MUL) {
	   match(MUL);
	   matchUnaryOp();
	   cout << "deref" << endl;
       }
       else if(la == NOT) {
	   match(NOT);
	   matchUnaryOp();
	   cout << "not" << endl;
       }
       else if(la == SUB) {
	   match(SUB);
	   matchUnaryOp();
	   cout << "neg" << endl;
       }
       else if (la == SIZEOF) {
	   if (!peekFlag)
	       next_token();
	   if(nextToken != LPAREN) { //need another check here
	        match(SIZEOF);
		matchUnaryOp();
	   	cout << "sizeof" << endl;
	   }
	   else if(nextToken == LPAREN) {
	       match(SIZEOF);
	       match(LPAREN);
	       matchSpecifier();
	       matchPtrs();
	       match(RPAREN);
	       matchUnaryOp();
	       cout << "sizeof" << endl;
	   }
       }
       else
	   matchIndDotArr();
}


void matchIndDotArr() {
    matchFinal();
    while (1) {
	if (la == LBRACK) {
	    match(LBRACK);
	    matchExpression(); //go back to top
	    match(RBRACK);
	    cout << "index" << endl;
	}
	else if (la == DOT) {
	    match(DOT);
	    matchFinal();
	    cout << "dot" << endl;
	}
	else if (la == ARROW) {
	    match(ARROW);
	    matchFinal();
	    cout << "arrow" << endl;
	}
	else
	    break;
    }
}

void matchFinal() {
    if (la == ID) {
	match(ID);
	if (la == LPAREN) {
	    match(LPAREN);
	    if (la == RPAREN) {
		match(RPAREN);
	    }
	    else {
		matchExpressionList();
		match(RPAREN);
	    }
	}
    }
    else if (la == NUM)
	match(NUM);
    else if (la == STRING)
	match (STRING);
    else if (la == CHAR)
	match (CHAR);
    else if (la == LPAREN) {
	match(LPAREN);
	matchExpression();
	match(RPAREN);
    }
}


void matchExpressionList() {
    matchExpression();
    while (la == COMMA) {
	match(COMMA);
	matchExpression();
    }
}

void topLevel(void) {
    matchSpecifier(); //when we match struct, need to include next ID
    if (la == LCURL) { //know in typedef case
	match(LCURL);
	matchDeclaration();
	matchDeclarations();
	match(RCURL);
	match(SEMIC);
    }
    else { //in global-dec or func-def
	matchPtrs();
	match(ID);
	if (la == LPAREN) {
	    match(LPAREN);
	    if (la == RPAREN) {
		matchGlobDecList();
		match(SEMIC);
	    }
	    else
		matchFuncDef();
	}
	else {  
	    matchGlobDecList();
	    match(SEMIC); 
	}
    }
}

void matchGlobDecList(void) {
    matchGlobDec();
    while (la == COMMA) {
	match(COMMA);
	match(ID);
	matchGlobDecList();
    }

}

void matchGlobDec(void) {
    if (la == RPAREN) {
	match(RPAREN);
    }
    else if(la == LPAREN) {
	match(LPAREN);
	matchGlobDec();
    }
    else if (la == LBRACK) {
	match(LBRACK);
	match(NUM);
	match(RBRACK);
    }
}
    

void matchFuncDef(void) {
    matchParameters();
    match(RPAREN);
    match(LCURL);
    matchDeclarations();
    matchStatements();
    match(RCURL);
}

void matchParameters(void) {
    if (la == VOID) {
	match(VOID);
    }
    else
	matchParamList();
}

void matchParamList(void) {
    matchParam();
    while (la == COMMA) {
	match(COMMA);
	matchParam();
    }
}

void matchParam(void) {
    matchSpecifier();
    matchPtrs();
    match(ID);
}

void matchDeclarations(void) {
    while (la == INT || la == CHAR || la == STRUCT)
	matchDeclaration();
}   

void matchDeclaration(void) {
    matchSpecifier();
    matchDecTORList();
    match(SEMIC);
}

void matchDecTORList(void) {
    matchDeclaraTOR();
    while (la == COMMA) {
	match(COMMA);
	matchDeclaraTOR();
    }
}

void matchDeclaraTOR(void) {
    matchPtrs();
    match(ID);
    if(la == LBRACK) {
	match(LBRACK);
	match(NUM);
	match(RBRACK);
    }
}

void matchStatements(void) {
    while (la != RCURL) {
    	matchStatement();
    }
}

void matchStatement(void) {
    if (la == LCURL) {
	match(LCURL);
	matchDeclarations();
	matchStatements();
	match(RCURL);
    }
    else if (la == RETURN) {
	match(RETURN);
	matchExpression();
	match(SEMIC);
    }
    else if (la == WHILE) {
	match(WHILE);
	match(LPAREN);
	matchExpression();
	match(RPAREN);
	matchStatement(); //recursion
    }
    else if (la == IF) {
	match(IF);
	match(LPAREN);
	matchExpression();
	match(RPAREN);
	matchStatement();
	if (la == ELSE) {
	    match(ELSE);
	    matchStatement();
	}
    }
    else {
	matchExpression();
	if (la == ASGN) {
	    match(ASGN);
	    matchExpression();
	}
	match(SEMIC);
    }
}


//NOTE, matchExpression() calls all of the expression stuff

int main () {
    while (la != ENDOFF) {
	topLevel();
    }
    return 0;
}
