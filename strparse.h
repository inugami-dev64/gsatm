#ifndef __STRPARSE_H
#define __STRPARSE_H

#ifdef __STRPARSE_C
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
    #include <stdint.h>
    #include "lib_export.h"
    #include "err_def.h"

    typedef uint8_t bool;
    #define true 1
    #define false 0
#endif

#if !defined(__USER_C) || !defined(_WIN32)
/* Find total line count from buffer */
LIB_EXPORT size_t CALL_CON str_Lc(char *buf);


/* Read n bytes from str and push the data to p_out_strs */
LIB_EXPORT void CALL_CON str_PushSplitStr (
    char *str, 
    size_t n, 
    char ***p_out_strs, 
    size_t *p_out_str_c
);


/* 
 * Check if string only contains uppercase or lowercase alphabetical characters 
 * O(n)
 */
LIB_EXPORT bool CALL_CON str_IsAlphabetical (
    char *str, 
    size_t len
);


/* Make all characters from string uppercase */
LIB_EXPORT void CALL_CON str_ToUpperCase (
    char *str,
    size_t len
);


/* Remove all line comments */
LIB_EXPORT void CALL_CON str_RmLineComments (
    char cm_sym, 
    char *buf
);

#endif

#endif
