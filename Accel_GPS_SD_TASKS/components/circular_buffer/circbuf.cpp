#include "circbuf.h"

// Note: circbuf fields are std::string *buf and int front, back, size

bool allocate(circbuf &cb, int size)
{
   // initialize buffer as empty before attempting allocation
   cb.front = -1;
   cb.back = -1;
   cb.size = 0;
   cb.buf = NULL;

   // only attempt allocation on valid size
   if (size > 0) {
      cb.buf = new std::string[size];
      if (!cb.buf) {
         std::cerr << "ERROR: allocation attempt failed (insufficient memory?)" << std::endl;
         return false;
      }
      cb.size = size;
      return true;
   }

   std::cerr << "ERROR: allocation attempted with a negative size" << std::endl;
   return false;
}

void deallocate(circbuf &cb)
{
   // only deallocate if buffer contains valid pointer
   if ((cb.buf != NULL) && (cb.size > 0)) {
      delete [] cb.buf;
   }
}

bool doubleSize(circbuf &cb)
{
   // ensure we were given a valid buffer
   if ((cb.buf == NULL) || (cb.size < 1)) {
      std::cerr << "ERROR: attempting to resize an empty buffer" << std::endl;
      return false;
   }

   // attempt and check the new allocation
   int newsize = 2 * cb.size;
   std::string *newbuf = new std::string[newsize];
   if (newbuf == NULL) {
      std::cerr << "ERROR: attempted resize failed (insufficient memory?)" << std::endl;
      return false;
   }

   // copy the old contents into the front of the new buffer
   int currsize = getCurrSize(cb);
   for (int pos = 0; pos < currsize; pos++) {
      // compute current position in original, accounting for possible wrap around
      int bufferIndex = (cb.front + pos) % cb.size;
      newbuf[pos] = cb.buf[bufferIndex];
   }
   cb.front = 0;
   cb.back = currsize-1;

   // deallocate the old buffer array
   delete [] cb.buf;

   // update the buffer's size and array pointer with the new versions
   cb.buf = newbuf;
   cb.size = newsize;
   return true;
}

int getCurrSize(circbuf cb)
{
   if (cb.front == -1) {
      return 0;
   }
   // basic size computation (no wrap around)
   int currsize = 1 + cb.back - cb.front;

   // adjust for cases involving wrap around
   if (cb.back < cb.front) {
      currsize += cb.size;
   }
   return currsize;
}

bool insert(circbuf &cb, std::string s)
{
   // ensure we were given validly allocated buffer
   if ((cb.size < 1) || (cb.buf == NULL)) {
      std::cerr << "ERROR: attempted insert into a buffer with no allocated space" << std::endl;
      return false;
   }
   // insert technique depends on whether or not buffer was previously empty
   int currsize = getCurrSize(cb);
   if (currsize == 0) {
      cb.front = 0;
      cb.back = 0;
      cb.buf[0] = s;
      return true;
   } else {
      // compute number of strings currently stored, checking for wraparound
      // resize if necessary
      if (currsize >= cb.size) {
         if (!doubleSize(cb)) {
            std::cerr << "ERROR: buffer full and unable to resize" << std::endl;
            return false;
         }
      }
      // determine position for new string, wrap around if necessary
      cb.back++;
      if (cb.back >= cb.size) {
         cb.back = 0;
      }
      cb.buf[cb.back] = s;
      return true;
   }
   return false;
}

bool remove(circbuf &cb, std::string &s)
{
   // removal varies depending on whether current size was 0, 1, or greater
   int currsize = getCurrSize(cb);
   if (currsize < 1) {
      std::cerr << "ERROR: cannot remove from empty buffer" << std::endl;
      return false;
   }
   // copy first word of buffer
   s = cb.buf[cb.front];
   if (currsize == 1) {
      // that was the only word then set front/back to -1
      cb.front = -1;
      cb.back = -1;
   } else {
      // adjust front of buffer, wrap around if necessary
      cb.front++;
      if (cb.front >= cb.size) {
         cb.front = 0;
      }
   }
   return true;
}

void print(circbuf cb)
{
   // display contents of non-empty buffer, wrap around when necessary
   if (cb.front != -1) {
      int currsize = getCurrSize(cb);
      for (int pos = 0; pos < currsize; pos++) {
         int bufferIndex = (cb.front + pos) % cb.size;
         std::cout << cb.buf[bufferIndex] << std::endl;
      }
   }
}


