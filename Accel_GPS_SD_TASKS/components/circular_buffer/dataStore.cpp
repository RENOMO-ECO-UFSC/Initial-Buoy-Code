#include <iostream>
#include <string>
#include "circbuf.h"
#include "dataStore.h"

// basic routine to test out our circular buffer,
//    allows use to create an initial buffer of a user-specified size
//    then repeately insert/remove/print contents,
//    deallocating on quit
int main()
{
   char cmd;
   circbuf buffer;
   int size = getSize();
   if (allocate(buffer, size)) {
      do {
         cmd = getCmd();
         process(buffer, cmd);
      } while (cmd != QUIT);
   } else {
      std::cerr << "ERROR: no buffer allocated, terminating program" << std::endl;
   }
   std::cout << std::endl;
}


int getSize()
{
   int size = 4;
   /*do {
      std::cout << "Enter the desired buffer size as a positive integer" << std::endl;
      std::cin >> size;
      if (std::cin.fail()) {
         std::cin.clear();
         std::cin.ignore(MaxLine, '\n');
         std::cerr << "ERROR: that was not an integer, please try again" << std::endl;
      } else if (size < 1) {
         std::cerr << "ERROR: that was not a positive integer, please try again" << std::endl;
      }
   } while (size < 1);*/
   return size;
}


void process(circbuf &cb, char cmd)
{
   std::string w;
   switch (cmd) {
     case HELP:
        printHelp();
        break;
     case INSERT:
        std::cout << "Enter a word to insert in the buffer" << std::endl;
        std::cin >> w;
        if (insert(cb, w)) {
           std::cout << "The word " << w << " was successfully inserted" << std::endl;
        } else {
           std::cerr << "ERROR: failure on attempt to insert word " << w << std::endl;
        }
        break;
     case PRINT:
        std::cout << "The current buffer contents are:" << std::endl;
        print(cb);
        break;
     case QUIT:
        deallocate(cb);
        std::cout << std::endl << "Goodbye!" << std::endl << std::endl;
        break;
     case REMOVE:
        if (remove(cb, w)) {
           std::cout << "The word " << w << " was successfully removed" << std::endl;
        } else {
           std::cerr << "ERROR: failure on attempt to remove front word " << std::endl;
        }
        break;
     default:
        break;
   }
}


char getCmd()
{
  std::cout << "Please enter a command (";
  std::cout << HELP << ",";
  std::cout << INSERT << ",";
  std::cout << PRINT << ",";
  std::cout << QUIT << ",";
  std::cout << REMOVE << "): ";
  char cmd;
  std::cin >> cmd;
  cmd = toupper(cmd);
  switch (cmd) {
     case HELP:
     case INSERT:
     case PRINT:
     case QUIT:
     case REMOVE:
        return cmd;
     default:
        std::cerr << "That was an invalid command: " << cmd << std::endl;
        std::cerr << "Please try again" << std::endl;
  }
  return cmd;
}


void printHelp()
{
   std::cout << "This program allows you to store a collection of words," << std::endl;
   std::cout << "   adding new words to the end of the collection" << std::endl;
   std::cout << "   or removing words from the front of the collection" << std::endl;
   std::cout << "The available commands are:" << std::endl;
   std::cout << "   to display this menu enter " << INSERT << std::endl;
   std::cout << "   to insert a new string enter " << HELP << std::endl;
   std::cout << "   to remove the front string enter " << REMOVE << std::endl;
   std::cout << "   to display the current buffer content enter " << PRINT << std::endl;
   std::cout << "   to end the program enter " << QUIT << std::endl;
}

