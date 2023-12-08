// Written by Hayden Killoh 1/11/21

#include <stddef.h>

/* mymemory.h
 * 
 * describes structures for memory management
 */

#ifndef MYMEMORY_H
#define MYMEMORY_H

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define MAXMEM       1024
#define MAXSEGMENTS   100

// this typedef defines a byte
typedef unsigned char Byte ;

// this is our memory, we declare it as extern so that it becomes global
extern Byte mymemory [ MAXMEM ] ;

// the segment descriptor, a node in a linked list
typedef struct segmentdescriptor {
   Byte     allocated ;
   void   * start ;
   size_t   size  ;
   struct segmentdescriptor * next ;
} Segment_t ;


extern Segment_t * segmenttable;


// forward references, user interface
void        initialize () ;
void      * mymalloc   ( size_t  ) ;
void        myfree     ( void *  ) ;
void        mydefrag   ( void ** ) ;

// forward references, managing segmentation table
Segment_t * findFree    ( Segment_t *, size_t      ) ;
void        insertAfter ( Segment_t *, Segment_t * ) ;
Segment_t * findSegment ( Segment_t *, void*       ) ;

int         delSegment  ( Segment_t *, Segment_t * ) ;
void *      moveSegment ( Segment_t *, Segment_t * ) ;

// forward references, helper functions
int isPrintable ( int c ) ;
void printmemory () ;
void printsegmenttable() ;
void printsegmentdescriptor() ;

#endif