/* Greg Cusack
 * COEN 175 Phase 1
 * 1/8/16
 * Lexical Analyzer
 * */

#include <iostream>
#include <cctype>
#include <stdio.h>

using namespace std;

int main () {
	
    int val, val_plus_one;
	string s;
	string keywords[] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};

	val = cin.get();
	while(val != EOF) {
		bool flag = false;
		s.clear();
		//Check for number
		if(isdigit(val)) {//know ints a number
			s.clear();  //clear string
			do {
				s += val; //store values in string
				val = cin.get(); //get next digit
			} while(isdigit(val)); //do while you are still reading numbers
			cout << "number:" << s << endl;
		}
		//Check for identifier
		else if (val == '_' || isalpha(val)) {
			s.clear();
			while(!flag) {
				s += val;
				val = cin.get();
				if(!(isalnum(val) || val == '_')) {
					flag = true; //breaks out of loop
				}
			}
			flag = false;
			for (int i=0; i<32; i++) {
				if (s == keywords[i])
					flag = true;
			}
			if (flag)
				cout << "keyword:" << s << endl;
			else
                cout << "identifier:" << s << endl;
		}
		//Check for string
		else if (val == '"') {
			do {
				if(val == '\\') {
					s += val;
					val = cin.get(); //eat up next one
				}
				s += val;
				val = cin.get();
			} while (val != '"'); //do this until we run into end quotation
			val = cin.get();
			s+='"';

			cout << "string:" << s << endl;
		}
		
		//Check for character
		else if (val == '\'') {
            do {
                if (val == '\\') {
                    s += val;
                    val = cin.get();
                }
				s += val;
				val = cin.get();
			} while (val != '\'');
            val = cin.get();
            s += '\'';
            cout << "character:" << s << endl;
        }
        
        //check for comments
        else if (val == '/') {
            val_plus_one = cin.get();
            cin.putback(val_plus_one);
            if (val_plus_one != '*') { //division symbol, not comment
                s += val;
                val = cin.get();
                cout << "operator:" << s << endl;
            }
            else {  //comment
                val = cin.get();
                while (1) {
                    val = cin.get();
                    if (val == '*') {
                        val = cin.get();
                        cin.putback(val);
                        if (val == '/') { //end comment
                            val =  cin.get();
                            break;
                        }
                    }
                }
                val = cin.get();
            }
        }
        
        //Check for operators (no posibility have having a 2-part operator like == or ++)
        else if (val == '*' || val == '%' || val == '.' || val == '(' || val == ')' || val == '[' || val == ']' || val == '{' || val == '}' || val == ';' || val == ':' || val == ',') {
            s += val;
            val = cin.get();
            cout << "operator:" << s << endl;
        }
        
        //check for operators that have posibility of having 2-part operator like == or ++
        else if (val == '=' || val == '<' || val == '>' || val == '+' || val == '-' || val == '&' || val == '!' || val == '|') {
            s+=val;
            val = cin.get();
            if (val == '|' || val == '&' || val == '=' || val == '+' || val == '-' || val == '>') {
                s+=val;
                val = cin.get();
                cout << "operator:" << s <<endl;
            }
            else {
                cout << "operator:" << s << endl;
            }
        }
		
		else {
			val = cin.get(); //nothing matches
		}
	}
}
