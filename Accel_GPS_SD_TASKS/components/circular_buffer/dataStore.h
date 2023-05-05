#ifndef DATASTORE_H
#define DATASTORE_H

#include "circbuf.h"

// default line length to clear on bad input
const int MaxLine = 256;

// the set of available user commands
const char HELP = 'H';
const char INSERT = 'I';
const char PRINT = 'P';
const char QUIT = 'Q';
const char REMOVE = 'R';

// get and return a valid user command
char getCmd();

// display the help menu (list of user commands)
void printHelp();

// apply the given command to the buffer
void process(circbuf &cb, char cmd);

// get the user's desired buffer size as a positive integer
int getSize();

#endif
