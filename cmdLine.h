//
// Created by Tianze_Liu on 2/18/20.
//

#ifndef MYSH_CMDLINE_H
#define MYSH_CMDLINE_H
void initCmdLine(); // Required one time before running getCmdLine
struct command  getCmdLine(); // Returns a list of white-space delimited tokens read from stdin
// Last element in the list is a pointer to NULL
// Results from previous call are invalidated by a new call to getCmdLine
void freeCmdLine(); // Required one time before exiting

/*command line*/
struct command{
    char** tokens;//the content of command line
    int n;//the number of strings which the command line consists of
};
#endif //MYSH_CMDLINE_H
