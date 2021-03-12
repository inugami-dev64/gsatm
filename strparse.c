#define __STRPARSE_C
#include "strparse.h"

/* Find total line count from buffer */
 size_t str_Lc(char *buf) {
    // TODO: Find total line count
    size_t line_c = 1;
    char *line_beg = buf, *line_end = buf;
    while((line_end = strchr(line_end, 0x0A))) {
        line_c++;
        line_end++; if(line_end < &buf[strlen(buf) - 1] && *line_end == 0x00)
            break;
    }

    return line_c;
 }


/* 
 * Read n bytes from str, then reallocte memory and push
 * the substring to p_out_strs
 */
 void str_PushSplitStr (
    char *str, 
    size_t n, 
    char ***p_out_strs, 
    size_t *p_out_str_c
) {
    (*p_out_str_c)++;
    char **tmp = (char**) realloc (
        (*p_out_strs),
        (*p_out_str_c) * sizeof(char)
    );

    if(!tmp) REALLOC_ERR("string array");
    
    (*p_out_strs) = tmp;
    (*p_out_strs)[(*p_out_str_c) - 1] = (char*) calloc (
        n + 1,
        sizeof(char)
    );

    strncpy (
        (*p_out_strs)[(*p_out_str_c) - 1],
        str,
        n
    );
}


/* 
 * Check if string only contains uppercase or lowercase alphabetical characters 
 * O(n)
 */
 bool str_IsAlphabetical (
    char *str, 
    size_t len
) {
    for(size_t i = 0; i < len; i++) {
        if(!(str[i] < 0x5b && str[i] > 0x40) && !(str[i] < 0x7b && str[i] > 0x60))
            return false;
    }

    return true;
}


/* Make all characters from string uppercase */
 void str_ToUpperCase (
    char *str,
    size_t len
) {
    for(size_t i = 0; i < len; i++) {
        if(str[i] < 0x7b && str[i] > 0x60)
            str[i] -= 0x20;
    }
}


/* Remove all line comments */
 void str_RmLineComments (
    char cm_sym, 
    char *buf
) {
    // TODO: Remove all comments from line that start with identifier cm_sym
    char *beg = buf, *end = buf;
    while((beg = strchr(end, (int) cm_sym))) {
        end = strchr(beg, 0x0A);
        if(!beg) return;
        if(!end) end = buf + strlen(buf);
        for(size_t i = 0; i < end - beg; i++)
            *(beg + i) = 0x20;

        end++;
        if(end >= buf + strlen(buf) || *end == 0x00)
            break;
    }
}

