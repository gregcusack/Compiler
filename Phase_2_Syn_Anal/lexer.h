/*
 * File:	lexer.h
 *
 * Description:	This file contains the public function and variable
 *		declarations for the lexical analyzer for Simple C.
 */

# ifndef LEXER_H
# define LEXER_H

enum token {
    CHARACTER = 256, STRING, NUM, ID, OR, AND, EQL, NEQ, LTN, GTN, LEQ, GEQ, ADD, SUB, 
    MUL, DIV, REM, CAST, ADDR, DEREF, NOT, NEG, INDEX, DOT, ARROW, AUTO = 400, BREAK,
    CASE, CHAR, CONST, CONTINUE, DEFAULT, DO, DOUBLE, ELSE, ENUM, EXTERN, FLOAT, FOR,
    GOTO, IF, INT, LONG, REGISTER, RETURN, SHORT, SIGNED, SIZEOF, STATIC, STRUCT,
    SWITCH, TYPEDEF, UNION, UNSIGNED, VOID, VOLATILE, WHILE, COMMA, SEMIC, COL, ASGN,
    DECR, INCR, LBRACK, RBRACK, LPAREN, RPAREN, LCURL, RCURL, ENDOFF
};

extern int lineno, numerrors;

int lexan(std::string &lexbuf);
void report(const std::string &str, const std::string &arg = "");

# endif /* LEXER_H */
