// Written by Hayden Killoh 1/11/21

/* mymemory.c
 * 
 * provides interface to memory management
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "mymemory.h"


// our memory
Byte        mymemory [MAXMEM] ;
Segment_t * segmenttable = NULL;


void initialize ()
{
  printf ( "initialize> start\n");
  memset(mymemory, '\0', MAXMEM); // set memory to 0
  
  //Dynamically allocate the "head" segment
  segmenttable = (Segment_t*) malloc(sizeof(Segment_t));
  segmenttable->allocated = FALSE; //initially, this is unallocated memory
  segmenttable->start = &mymemory; //The start should be the start of our memory array, or just the reference to its starting pointer
  segmenttable->size = MAXMEM; //Assign our maximum memory size
  segmenttable->next = NULL; //No next node at this point, so assign NULL

  printf ( "initialize> end\n");
}

// implement the mymalloc functionality
void * mymalloc ( size_t size )
{
  printf ( "mymalloc> start\n");
	
	//Check passed size
	if (size <= 0) {
    fprintf(stderr, "Size must be at least 1 byte in size");
		return NULL;
	} else if (size > MAXMEM) {
    fprintf(stderr, "Size is more than the max memory of 1024 bytes");
		return NULL;
	}
  
	//First try and find existing free segment
	Segment_t* segment = findFree(segmenttable, size);
	if (!segment) { //Free segment was not found, out of memory
    fprintf(stderr, "Out of memory!");
		return NULL;
	}
	
	//Construct smaller segment if larger than passed size
	if (segment->size > size) {
		Segment_t* other = (Segment_t*) malloc(sizeof(Segment_t));
		if (!other) { //Panic, out of *real* memory
      fprintf(stderr, "Not enough memory to allocate new segment!");
			return NULL;
		}
		
		//Assign members to new segment
		other->allocated = FALSE;
		other->start = segment->start + size; //Use pointer arithmetic to increment pointer memory position
		other->size = segment->size - size; //Calculate size difference
		other->next = NULL; //To be assigned with insertAfter
		
		//Adjust members of original segment
		segment->allocated = TRUE; //This will now be used
		segment->size -= other->size; //Adjust size of this new smaller chunk
		
		//Insert new segment
		insertAfter(segment, other);
	} else if (segment->size == size) { //Found segment is exact size
    segment->allocated = TRUE; //This will now be used
  }
	
	return segment->start;
}

void myfree ( void * ptr )
{
  printf ( "myfree> start\n");
  
  //Get matching segment
  Segment_t* segment = findSegment(segmenttable, ptr);
  if (!segment) { //Return null if not found
    fprintf(stderr, "Could not find matching segment for pointer %p\n", ptr);
    return;
  }
  
  //Reset segment data to \0
  memset(segment->start, '\0', segment->size);
  
  //Flip allocated flag to FALSE
  segment->allocated = FALSE;
}

void mydefrag ( void** ptrlist) {
  printf ( "mydefrag> start\n");
  //Find the first unallocated segment by looping through the linked list, returning the first occurance
  Segment_t* empty = *ptrlist;
  while (empty) {
    if (empty->allocated == FALSE) {
      break;
    }
    empty = empty->next;
  }
  if (empty->allocated != FALSE) {
    printf("Could not find unallocated segment\n");
  }
  
  //Merge all unallocated segments by looping through the linked list and assinging their allocated flag and adjusting their starting pointers
  Segment_t* current = *ptrlist;
  while (current) {
    if (current->allocated == FALSE && current != empty) {
        //Merge into main empty segment
        empty->size += current->size; //Increment main segment by every other segment's size
        Segment_t* next = current->next;
        delSegment(segmenttable, current); //Delete the segment once all its data has been merged into the main segment
        current = next;
    } else {
        current = current->next;
    }
  }
  
  //Repair starting pointers and transfer bytes to new location
  current = *ptrlist;
  while (current) {    
    current->start = moveSegment(*ptrlist, current);
    current = current->next;
  }
  
  //Set all bytes in the empty segment to 0. In a real system, this wouldn't be needed, but I added it for readability
  memset(empty->start, '\0', empty->size);
}

void* moveSegment(Segment_t* list, Segment_t* segment) {
  char buf[1024]; //Temp char buffer to store the data being moved
  void* memPos;
  
  //put memory in segment into temp buf
  if (segment->allocated == TRUE) {
    memcpy(buf, segment->start, segment->size);
  }

  //We are trying to move the head of the list
  if (segment == list) {
    memPos = &mymemory[0];
  } else { //Find parent segment to determine the starting memory position for this segment
    Segment_t* parent = list;
    while (parent) {
      if (parent->next == segment) {
        break;
      }
      parent = parent->next;
    }
    if (!parent) {
      fprintf(stderr, "Could not find segment parent! %p\n", segment);
      return NULL;
    }
    //Caluclate the memPos from the parent starting index and the parent size
    memPos = parent->start + parent->size;
  }
  
  //Reinsert data
  if (segment->allocated == TRUE) {
    memcpy(memPos, buf, segment->size);
  }
  
  return memPos;
}

int delSegment(Segment_t* list, Segment_t* segment) {
  if (list != segment) {
    //Find the parent segment in the list
    Segment_t* previous = list;
    while (previous) {
      if (previous->next == segment) {
        break;
      }
      previous = previous->next;
    }
    
    //This should not be reachable, somehow the segment is not the head or doesn't exist in the table
    if (list == segment) {
      fprintf(stderr, "Found segment does not exist in the table %p\n", segment);
      return 0;
    }
    
    //Finally remove the matching segment from the list
    previous->next = segment->next;
  } else {
    //Assign a new head segment
    segmenttable = segment->next;
  }
  
  //Finally actually free the segment from real memory
  free(segment);
  
  return 1;
}

// helper functions for management segmentation table
Segment_t * findFree ( Segment_t * list, size_t size )
{
  printf ( "findFree> start\n");
  
  //Make sure passed head node is not null
	if (!list) {
		fprintf(stderr, "Head segment cannot be null!");
    return NULL;
	}
  
  //Find the first free segment in the linked list
	Segment_t* current = list;
	while (current) {
    //Make sure segment is unallocated and big enough to store the data
		if (current->allocated == FALSE && current->size >= size) {
      //Valid segment found, return current pointer
			return current;
		} else {
			current = current->next;
		}
	}
  
	return NULL; //No valid segment found
}

void insertAfter ( Segment_t * oldSegment, Segment_t * newSegment )
{
	if (oldSegment->next) { //Okay to insert
		Segment_t* oldNext = oldSegment->next; //Keep track of old next
		oldSegment->next = newSegment; //Insert newSegment as next in the list
		newSegment->next = oldNext; //Repair the broken chain by assigning newSegment's next to the oldNext
	} else { //End of list
		oldSegment->next = newSegment; //Append newSegment
	}
}

Segment_t * findSegment ( Segment_t * list, void * ptr )
{
  Segment_t* current = list;
  while (current) {
    if (current->start == ptr) {
      return current; //Return the matching segment
    }
    current = current->next; //Jump to next segment in the list
  }
  return NULL; //No matching segment was found
}

int isPrintable ( int c )
{
  if ( c >= 0x20 && c <= 0x7e ) return c ;

  return 0 ;
}

void printmemory ()
{  
  char buff[17]; //Temp buffer for each line
  
  //Loop through entire memory array
  int i;
  int line = 0;
  for (i = 0; i < MAXMEM; i++) {
    //On every 10 character line
    if ((i % 10) == 0) {
      //print the current buffer
      if (i != 0) {
        printf(" | %s\n", buff);
        line++;
      }
      
      //Print the current line number
      printf("[%*d]", 4, line * 10);
    }
    
    //Print each hex code
    printf(" %02x", mymemory[i]);
    
    //clamp the buffer index to its always within 0 and 16
    if (isPrintable(mymemory[i]) == 0) { 
      buff[i % 10] = '.'; //char is not printable
    } else {
      buff[i % 10] = mymemory[i]; //add current char to buff
    }
    buff[(i % 10) + 1] = '\0'; //terminate char buffer
    
  }
  
  //Add whitespace padding if this line has less than an order of 10 chars
  while ((i % 10) != 0) {
    printf("   ");
    i++;
  }
  
  //Print the last line
  printf(" | %s\n", buff);
}

void printsegmenttable()
{
  //Loop over every node in the linked list and print them
  Segment_t* current = segmenttable;
  while(current) {
    printsegmentdescriptor(current); 
    current = current->next;
  }
}

void printsegmentdescriptor ( Segment_t * descriptor )
{
  printf ( "\tallocated = %s\n" , (descriptor->allocated == FALSE ? "FALSE" : "TRUE" ) ) ;
  printf ( "\tstart     = %p\n" , descriptor->start ) ;
  printf ( "\tsize      = %lu\n", descriptor->size  ) ;
  printf("------------\n");
}

