#ifndef CIRCBUF_H
#define CIRCBUF_H

#include <iostream>
#include <string>

// http://csci.viu.ca/~wesselsd/courses/csci161/code/circbuf/
// https://www.youtube.com/watch?v=WLaZNejWE7U

// a dynamically-allocated array of strings,
// stored as a circular (wrap-around) buffer:
//   inserts take place at the back
//   removes are done from the front
//   print goes from front to back

struct circbuf {
   std::string *buf;       // the dynamically allocated array
   int front, back;        // positions of first and last elements,
                           //    -1 if buffer is empty
   int size;               // the size of the allocated array
};

// allocate and initialize a circular buffer of the specified size,
// return true if successful, false otherwise
bool allocate(circbuf &cb, int size);

// deallocate the dynamically allocated array in the buffer
void deallocate(circbuf &cb);

// determine how many strings are currently stored in the buffer
int getCurrSize(circbuf cb);

// replace the contents of the given circular buffer
//    with one that is twice as big
// return true if possible, false otherwise
bool doubleSize(circbuf &cb);

// insert the new string, s, at the end of the buffer,
//    resizing if necessary
// return true if successful, false otherwise
bool insert(circbuf &cb, std::string s);

// remove the last string in the buffer, copying to s
// return true if successful, false otherwise
bool remove(circbuf &cb, std::string &s);

// display the contents of the buffer from front to back
//    (nothing if buffer is empty)
void print(circbuf cb);

#endif
