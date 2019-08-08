#include <stdio.h>
#include <string.h>

#define MAXLINES 5000  // max number of lines to be sorted

char* lineptr[MAXLINES];  // an array of char pointers to text lines

int readlines(char* lineptr[], int maxlines);
void writelines(char* lineptr[], int nlines);
void qsort(char* lineptr[], int left, int right);

int main() {
  int nlines;  // hold the total number of lines read
  
  if ((nlines = readlines(lineptr, MAXLINES)) >= 0) {
    qsort(lineptr, 0, nlines - 1);
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
char* alloc(int);  // memory allocator, declared here, may be compiled elsewhere?

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
Swap two elements in an array of char pointers. Used by qsort()
Parameters:
  char* v[]: an array of char pointers
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


/* qsort(): 
Parameters:
  char* lineptr[]: an array of pointers, each pointer points to a chunk of memory holding a C-string;
  int left: the left boundary of sorting operation
  int right: the right boundary of sorting operation
  
This is a simple version of quick sort. 

Middle element is chosen as the pivot value. We get the pivot element out of the way by swapping it with left element. Use an integer variable named last to hold the index value of elements that is smaller than pivot element. last is initially set to left (where pivot is).

We use one integer i to traverse the array, from [left + 1] to right, if v[i] < v[left] (i.e. if one element is smaller than the pivot value), we swap(++last, i). Don't move elements that is bigger than the pivot value. After the iteration, the structure is:
  p----++++
      l
where l is the last index, we swap(left, last), so the structure becomes:
  ----p++++

Then call qsort() again to sort the first and second half of the array.
*/
void qsort(char* lineptr[], int left, int right) {
  int last;  // the index of last element that is smaller than pivot 
  
  // check if left boundary is greater than right boundary
  if (left >= right)
    return;
  
  // get pivot value out of the way 
  swap(lineptr, left, (left + right) / 2);
  last = left;
  
  // go over the array and swap elements
  for (int i = left + 1; i <= right; i++)
    if (strcmp(lineptr[i], lineptr[left]) < 0)
      swap(lineptr, ++last, i);
  
  // swap back pivot value 
  swap(lineptr, left, last);
  
  // sort two subarrays
  qsort(lineptr, left, last - 1);
  qsort(lineptr, last + 1, right);
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