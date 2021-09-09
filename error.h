//
// Created by Tianze_Liu on 2/25/20.
//

#ifndef MYSH_ERROR_H
#define MYSH_ERROR_H

enum {
    RESULT_NORMAL,
    ERROR_FORK,
    ERROR_MISS_PARAMETER,

    /* error info of redir */
    ERROR_MANY_IN,
    ERROR_MANY_OUT,
    ERROR_OUTFILE_NOT_EXIST,
    ERROR_INFILE_NOT_EXIST,
    /* error info of pipeline */
    ERROR_PIPE,
    ERROR_PIPE_MISS_PARAMETER
};

#endif //MYSH_ERROR_H
