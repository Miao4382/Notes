#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXLINES 5000  // max number of lines to be sorted

/* Global Variable */
char* lineptr[MAXLINES];  // an array of char pointers to text lines
int numeric = 0;  // flag of -n option; 1 if numeric sort requested 
int reverse = 0;  // flag of -r option, 1 if reverse sort requested
int fold = 0;  // flag of -f option; 1 if fold upper/lower requested (so 'a' and 'A' are equal)

/* Function Prototype */
int readlines(char* lineptr[], int maxlines);
void writelines(char* lineptr[], int nlines);
void qsorts(char* v[], int left, int right, int (*comp)(void*, void*)); 
void rqsorts(char* v[], int left, int right, int (*comp)(void*, void*));
int numcmp(const char*, const char*);

// with optional arguments
int main(int argc, char* argv[]) {
  int nlines;  // hold the total number of lines read
  char ch;  // check char in optional arguments 
  
  // check inputed optional arguments
  while (--argc > 0 && *(++argv)[0] == '-')
    while (ch = *++argv[0]) {
      switch (ch) {
      case 'n':
        numeric = 1;
        break;
      case 'r':
        reverse = 1;
        break;
      case 'f':
        fold = 1;
        break;
      default:
        printf("sort: illegal option %c\n", ch);
        return -1;
      }
    }
  
  // read lines and sort according to flag
  if ((nlines = readlines(lineptr, MAXLINES)) >= 0) {
    // sorting order
    if (reverse)
      rqsorts((char **) lineptr, 0, nlines - 1, (int (*)(void*, void*))(numeric ? numcmp : strcmp));
    else
      qsorts((char **) lineptr, 0, nlines - 1, (int (*)(void*, void*))(numeric ? numcmp : strcmp));
    
    writelines(lineptr, nlines);
    return 0;
  }
  
  else {
    printf("error: input too big to sort\n");
    return 1;
  }

}

#define MAXLEN 1000  // max length of any input line
int getlines(char*, int);
char* alloc(int);  // memory allocator, declared here, compiled in a separate file (using example 5-4: a memory allocator toy)

/* readlines(): 
How does this work:
- use getlines() function to get each line from the input buffer and store in line[] variable; Also store the length of each line (not including )
- call alloc() to request a chunk of memory from a pre-allocated memory space (reservoir), a pointer to this chunk of memory will hold the address
- call strcpy() to copy content in line[] to the requested chunk of memory
- store the memory address hold in p to lineptr[]'s slots
store each read-line to the C-string array pointed by lineptr
The maximum input line number is the maxlines parameter
*/
int readlines(char* lineptr[], int maxlines) {
  int len;  // hold the length of line being read 
  int nlines;  // hold the number of lines read, should be smaller than maxlines
  
  char* p;  // a pointer used to hold the address of allocated 
  char line[MAXLEN];  // temp char array to hold currently read line, pass in getline() function
  
  nlines = 0;
  
  while ((len = getlines(line, MAXLEN)) > 0)  // if len > 0, a line has been read
    /* 
    - if line num exceeded max, or memory allocation failed (not enough room)
    - the amount of memory is len, which is returned by getlines()
    - this length contains the tailing '\n' character. So len = # of char + 1
    - but we'll replace '\n' with '\0'
    - also, line[] filled by getlines() has following structure:
      ['a', 'b', 'c', '\n', '\0']
      after replacing '\n' with '\0':
      ['a', 'b', 'c', '\0', '\0']
      We'll only allocate room for four chars, but the fifth one will still be copied to the master reservoir (used by alloc() to dispense memory). It will be over-written by the first letter of next line, because alloc() thinks this slot is the next empty slot, and the next request: p = alloc(len) will return p pointing to this.
    */
    if (nlines >= maxlines || (p = alloc(len)) == NULL)
      return -1;  // indicate readlines() function failed to read all the lines
    else {
      line[len - 1] = '\0';  // replace '\n' with '\0', so line[] is a C-string
      strcpy(p, line);  // copy content in line to spaces pointed by p
      lineptr[nlines++] = p;  // the address stored in p is now in lineptr[] 
    }
    
  return nlines;  // return the number of lines read from the input buffer
}

/* writelines(): 
Print the lines stored in the master reservoir by dereferencing pointer stored in lineptr[] 
*/
void writelines(char* lineptr[], int nlines) {
  for (int i = 0; i < nlines; i++)
    printf("%s\n", lineptr[i]);
}

/* swap(): 
Swap two elements in an array of pointers. Used by qsorts()
Parameters:
  void* v[]: an array of pointers, potentially to any type 
  int i: index of first element 
  int j: index of second element
  
swap v[i] and v[j]
*/
void swap(char* v[], int i, int j) {
  char* temp;
  temp = v[j];
  v[j] = v[i];
  v[i] = temp;
}


/* qsorts(): 
Sort v[left] ... v[right] into increasing order
Parameters:
  void* v[]: void* is a generic pointer type, here, it means pointer accepts an array of pointers to any type (like template?)
  int left: left boundary of sorting 
  int right: right boundary of sorting
  int (*comp)(void*, void*): a pointer to a function which accepts two void* arguments (two pointers to any type), the return type of this function is int.
  
Pay attention that comp is declared as a pointer to function. We can use it as a normal function by dereferencing it : (*comp)

The parenthese is needed when writting the parameter or actual using function in the body of qsorts. If no parenthese, int *comp(void*, void*) means a function named copm that returns a pointer to integer (not a pointer to function!)
*/
void qsorts(char* v[], int left, int right, int (*comp)(void*, void*)) {
  int last;
  
  if (left >= right)
    return;

  swap(v, left, (left + right) / 2);
  last = left;
  
  for (int i = left + 1; i <= right; i++) {
    if (fold) {
      char s1[MAXLEN];
      char s2[MAXLEN];
      strcpy(s1, v[i]);
      strcpy(s2, v[left]);
      *s1 = tolower(*s1);
      *s2 = tolower(*s2);
      if ((*comp)(s1, s2) < 0)
        swap(v, i, ++last);
    }
    
    else if ((*comp)(v[i], v[left]) < 0) {
      swap(v, i, ++last);
    }     
  }

  
  swap(v, left, last);
  qsorts(v, left, last - 1, comp);
  qsorts(v, last + 1, right, comp);
}

/* rqsorts()
Sort the array in reverse order
 */
void rqsorts(char* v[], int left, int right, int (*comp)(void*, void*)) {
  int last;
  
  if (left >= right)
    return;
  
  swap(v, left, (left + right) / 2);
  last = left;
  
  for (int i = left + 1; i <= right; i++) {
    if (fold) {
      char s1[MAXLEN];
      char s2[MAXLEN];
      strcpy(s1, v[i]);
      strcpy(s2, v[left]);
      *s1 = tolower(*s1);
      *s2 = tolower(*s2);
      if ((*comp)(s1, s2) > 0)
        swap(v, i, ++last);
    }
    
    else if ((*comp)(v[i], v[left]) > 0) {
      swap(v, i, ++last);
    }     
  }
  
  swap(v, left, last);
  rqsorts(v, left, last - 1, comp);
  rqsorts(v, last + 1, right, comp);  
}

/* numcmp(): 
compare s1 and s2 on a leading numeric value
*/
int numcmp(const char* s1, const char* s2) {
  double v1, v2;
  
  v1 = atof(s1);
  v2 = atof(s2);
  
  if (v1 < v2)
    return -1;
  else if (v1 > v2)
    return 1;
  else 
    return 0;
}

/* foldcmp()
All comparing terms will be transformed to lower case version and sort
*/
int foldcmp(const char* s1, const char* s2) {
  char s1m[MAXLEN];
  char s2m[MAXLEN];
  strcpy(s1m, s1);
  strcpy(s2m, s2);
  *s1m = tolower(*s1m);
  *s2m = tolower(*s2m);
  return strcmp(s1m, s2m);
}
 
int getlines(char* line, int max) {
  char ch;
  int count;
  
  // count < max - 1 for storing the last null terminator
  for (count = 0; count < max - 1 && (ch = getchar()) != EOF && ch != '\n'; ++count)
    *(line++) = ch;
  
  if (ch == '\n') {
    *(line++) = ch;
    count++;
  }
  
  *line = '\0';  // add null terminator to the array 
  
  return count;  // not containing the '\0' terminator 
}