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
void match_next_token(int x);
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

void matchSpecifier(void);
void matchPtrs(void);
int next_token(void);
int nextToken;

int next_token(void) {
    nextToken = lexan(lexbuf2);
    return nextToken;
    /*if (nextToken == INT || nextToken == CHAR || nextToken == STRUCT) {
	//cout << "YES" << endl;
	cout << "Next Token: " << nextToken << endl;
	return 1;
    }
    else if(nextToken == LPAREN)
	return LPAREN;
    else
	return nextToken;*/
}

void match_next_token(int x) {
    if(la == x){
	la = nextToken;
	//cout << "nexttoken: " << nextToken << endl;
    }
    else
	cout << "ERROR!!!" << endl;
}

void matchSpecifier() {
    if(la == INT) 
	match(INT);   // || la == CHAR || la == STRING || la == ID) {
    else if(la == CHAR)
	match(CHAR);
    else if(la == STRUCT){
	match(STRUCT);
	//match(ID);
    }
    else
	cout << "Error: Expected Specifier" << endl;
}

void matchPtrs(void) {
    while(la == DEREF || la == MUL) {
	if (la == DEREF) 
	    match(DEREF); //should this be deref or STAR??
	else
	    match(MUL);
	//cout << "matching" << endl;
    }
}

void match(int x) {
    //cout << "LA: " << la << endl;
    if (la == x) {
	//cout << "LA: " << la << endl;
	la = lexan(lexbuf);
    }
    else
	cout << "ERROR" << endl;
}


void matchOR(void) {
    matchAND();
    while(la == OR) {
	match(OR);
	matchAND();
	cout << "OR" << endl;
    }
}

void matchAND(void) {
    matchEQUIV();
    //match(ID);
    while(la == AND) {
        match(AND);
	matchEQUIV();
	//match(ID);
	cout << "AND" << endl;
    }
}

void matchEQUIV(void) {
    matchGOLTHN(); //greater or less than functions
    //match(ID);
    while (1) {
	if(la == EQL) {
	    match(la);
	    matchGOLTHN();
	    cout << "EQL" << endl;
	}
	else if (la == NEQ) {
	    match(la);
	    matchGOLTHN();
	    cout << "NEQ" << endl;
	}
	else
	    break;
    }
}

void matchGOLTHN(void) {
    matchAddSub();
    while (1) {
	if (la == LTN) {
	    match(la);
	    matchAddSub();
	    cout << "LTN" << endl;
	}
	else if (la == GTN) {
	    match(la);
	    matchAddSub();
	    cout << "GTN" << endl;
	}
	else if (la == LEQ) {
	    match(la);
	    matchAddSub();
	    cout << "LEQ" << endl;
	}
	else if (la == GEQ) {
	    match(la);
	    matchAddSub();
	    cout << "GEQ" << endl;
	}
	else
	    break;
    }
}

void matchAddSub(void) {
    matchMultDivRem();
    //match(ID);
    while(1) {
	if (la == ADD) {
	    match(la);
	    matchMultDivRem();
	    cout << "ADD" << endl;
	}
	else if (la == SUB) {
	    match(la);
	    matchMultDivRem();
	    cout << "SUB" << endl;
	}
	else
	    break;
    }
}

void matchMultDivRem(void) {
    //match(ID);
    matchCast();
    //cout << la << endl;
    match(ID);
    while(1) {
	if (la == MUL) {
	    match(la);
	    //match(ID);
	    matchCast();
	    match(ID);
	    cout << "MUL" << endl;
	}
	else if (la == DIV) {
	    match(la);
	    match(ID);
	    matchCast();
	    cout << "DIV" << endl;
	}
	else if (la == REM) {
	    match(la);
	    match(ID);
	    matchCast();
	    cout << "REM" << endl;
	}
	else
	    break;
    }
}


void matchCast(void) {
    matchUnaryOp();
    //match(ID);
    //cout << "here1" << endl;
    //int next = next_token();
    if (la == LPAREN) {
	int next = next_token();
       if(next == INT) { // || next == CHAR || next == STRUCT)) {
	   //cout << "here1" << endl;
	   match_next_token(la);
	   matchSpecifier();
	   matchPtrs();
	   //cout << la << endl;
	   match(RPAREN);
	   matchUnaryOp();
	   //match(ID);
	   cout << "CAST" << endl;
       }
       else if (next == CHAR) {
	   match_next_token(la);
	   matchSpecifier();
	   matchPtrs();
	   match(RPAREN);
	   matchUnaryOp();
	   //match(ID);
	   cout << "CAST" << endl;
       }
       else if (next == STRUCT) {
	   match_next_token(la);
	   matchSpecifier();
	   matchPtrs();
	   match(RPAREN);
	   matchUnaryOp();
	   //match(ID);
	   cout << "CAST" << endl;
       }
    }
    else {
	//cout << la << endl;
	//match(ID);
	//cout << la << endl;
	matchUnaryOp();
    }
}


void matchUnaryOp(void) {
    while (1) {
       if (la == ADDR) {
	   match(la);
	   //match(ID);
	   //matchIndDotArr();
	   cout << "ADDR" << endl;
       }
       else if( la == DEREF) {
	   match(la);
	   //match(ID);
	   //matchIndDotArr();
	   cout << "DEREF" << endl;
       }
       else if(la == NOT) {
	   match(la);
	   //match(ID);
	   //matchIndDotArr();
	   cout << "NOT" << endl;
       }
       else if(la == NEG) {
	   match(la);
	   //match(ID);
	   //matchIndDotArr();
	   cout << "NEG" << endl;
       }
       else if (la == SIZEOF) {
	   int next = next_token();
	   //cout << "Current token: " << la << endl;
	   if(next != LPAREN) { //need another check here
	   	match_next_token(la);
	        //cout << "HERRO" << endl;
	        //match(la);
	   	//matchIndDotArr();
	   	cout << "SIZEOF" << endl;
	   }
	   else if(next == LPAREN) {
	       match_next_token(la);
	       match(LPAREN);
	       matchSpecifier();
	       matchPtrs();
	       match(RPAREN);
	       cout << "SIZEOF" << endl;
	   }
       }
       else {
	   //match(ID);
	   //cout << "matched" << endl;
	   break;
       }
   }
}
    /* //matchIndDotArr();
    string temp;
    if(la == ADDR || la == DEREF || la == NOT || la == NEG || la == SIZEOF) {
	if(la == ADDR)
	    temp = "ADDR";
	else if (la == DEREF)
	    temp = "DEREF";
	else if (la == NOT)
	    temp = "NOT";
	else if (la == NEG)
	    temp = "NEG";
	else
	    temp = "SIZEOF";
	match(la);
	match(ID);
	//matchIndDotArr();
	cout << temp << endl;
    }
}*/

/*
void matchIndDotArr() {
    match(ID);
    string temp;
    if(la == LBRACK) {
	match(LBRACK);
	matchOR(); //go back to top??
	match(RBRACK);
	temp = "INDEX";
    }
    else if(la == DOT) {
	match(la);
	match(ID);
	temp = "DOT";
    }
    else if (la == ARROW) {
	match(la);
	match(ID);
	temp = "ARROW";
    }
    //do we need to match here?
    cout << temp << endl;
} */
    

int main () {
    //string lexbuf;
    while (la != ENDOFF) {
	//cout << la << endl;
	matchOR();
	count++; if (count == 10) { break;}
	//continue;
	//cout << "yo" << endl;
   }
    return 0;
}
