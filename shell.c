// Written by Hayden Killoh 1/11/21

#include <stdio.h>
#include <string.h>
#include "mymemory.h"

int main() {
  printf("shell> start\n");
  
  initialize();
  
  char* ptr1 = (char*) mymalloc(10);
  strcpy(ptr1, "this test");
  printf("shell> content of allocated memory: %s\n", ptr1);

  char* ptr2 = (char*) mymalloc(10);
  strcpy(ptr2, "this test");
  printf("shell> content of allocated memory: %s\n", ptr2);
  
  char* ptr3 = (char*) mymalloc(10);
  strcpy(ptr3, "this test");
  printf("shell> content of allocated memory: %s\n", ptr3);
	
  printmemory();
  printsegmenttable();
  
  myfree(ptr2);
  
  char* a = mymalloc(10);
  char* b = mymalloc(10);
  char* c = mymalloc(10);
  char* d = mymalloc(10);
  
  myfree(a);
  myfree(b);
  myfree(c);
  myfree(d);
  
  printsegmenttable();
  
  mydefrag(&segmenttable);
 
  printmemory();
  printsegmenttable();
  
  printf("shell> end\n");
  
  return 0;
}