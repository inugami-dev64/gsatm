#ifndef __STRPARSE_H
#define __STRPARSE_H

#ifdef __STRPARSE_C
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
    #include <stdint.h>
    #include "err_def.h"

    typedef uint8_t bool;
    #define true 1
    #define false 0
#endif

/* Find total line count from buffer */
 size_t str_Lc(char *buf);


/* Read n bytes from str and push the data to p_out_strs */
 void str_PushSplitStr (
    char *str, 
    size_t n, 
    char ***p_out_strs, 
    size_t *p_out_str_c
);


/* 
 * Check if string only contains uppercase or lowercase alphabetical characters 
 * O(n)
 */
 bool str_IsAlphabetical (
    char *str, 
    size_t len
);


/* Make all characters from string uppercase */
 void str_ToUpperCase (
    char *str,
    size_t len
);


/* Remove all line comments */
 void str_RmLineComments (
    char cm_sym, 
    char *buf
);


#endif
