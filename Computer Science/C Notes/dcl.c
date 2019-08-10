#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXTOKEN 100
#define MAXOUT 2000

/* function prototype/declaration */
void dcl(void);
void dirdcl(void);
char gettoken(void);

/* global variables */
int tokentype;  // hold type of last token
char token[MAXTOKEN];  // last token string
char name[MAXTOKEN];  // identifier name 
char datatype[MAXTOKEN];  // data type = char, int, etc
char out[MAXOUT];  // output string

/* main()
convert declaration to words
  - 
*/
int main() {
  while (gettoken() != EOF) {
    
  }
  
  return 0;
}

/* gettoken() 
Return: next token


*/
char gettoken(void) {
  int c;
}


void dcl(void) {
  int ns;
  
  for (ns = 0; gettoken() == '*'; ns++)  // count number of *
    ;
  
  dirdcl();  // call dirdcl
  while (ns-- > 0)  // the number of loop is number of '*'
    strcat(out, " pointer to");  // one '*' for one phrase " pointer to"
  
}