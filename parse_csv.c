#define __PARSE_CSV_C
#include "parse_csv.h"

#define bool uint8_t
#define true 1
#define false 0

/*****************************/
/***** Private functions *****/
/*****************************/

char *__readFileToBuffer(char *file_name) {
    // TODO: Read csv file into char buffer
    FILE *file;
    file = fopen(file_name, "rb");

    if(!file) {
        perror("Invalid currency file!\n");
        exit(EXIT_FAILURE);
    }
    
    fseek(file, 0, SEEK_END);
    long len = ftell(file) + 1;
    fseek(file, 0, SEEK_SET);

    char *buf = (char*) calloc (
        len,
        sizeof(char)
    );

    fread (
        buf, 
        sizeof(char), 
        len - 1, 
        file
    );

    return buf;
}


size_t __lc(char *buf) {
    // TODO: Find total line count
    size_t line_c = 1;
    char *line_beg = buf, *line_end = buf;
    while((line_end = strchr(line_end, 0x0A))) {
        line_c++;
        line_end++;
        if(line_end < &buf[strlen(buf) - 1] && *line_end == 0x00)
            break;
    }

    return line_c;
}


void __pushSplitStr (
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

    if(!tmp) {
        perror("Failed to reallocate memory for string array!");
        exit(EXIT_FAILURE);
    }
    
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

/********************************/
/******* Public functions *******/
/********************************/

void fetchExRates(char *out_file) {
    // TODO: Fetch information about exchange rates
    CURL *curl;
    FILE *file;

    file = fopen(out_file, "wb");
    if(!file) {
        perror("Failed to read exchange rate file!\n");
        exit(EXIT_FAILURE);
    }

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, ATM_EX_RATE_DATA_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    printf("Fetching currency exchange rates...\n");

    curl_easy_perform(curl);
    fclose(file);
}


void parseCurrencyInfo (
    char *csv_file,
    CurrencyInfo **p_cur_infos,
    size_t *p_cur_info_c,
    char ***p_meta,
    size_t *p_meta_c
) {
    char *buf = __readFileToBuffer(csv_file);
    size_t line_c = __lc(buf);

    // TODO: Allocate memory for currency infos
    size_t cap = __ATM_DEFAULT_CURRENCY_C;
    (*p_cur_info_c) = 0;
    (*p_cur_infos) = (CurrencyInfo*) calloc (
        cap,
        sizeof(CurrencyInfo)
    );

    // TODO: Allocate memory for metadata
    (*p_meta) = (char**) calloc (
        1,
        sizeof(char*)
    );
    *p_meta_c = 0;
    
    // TODO: Extract metadata and currency information from buffer
    char *line_beg = buf, *line_end = buf;
    while((line_end = strchr(line_end, 0x0A))) {
        char *qmk[4] = {0};
        qmk[0] = strchr(line_beg, '\"');
        if(qmk[0] && *(qmk[0] + 1) != 0x00) 
            qmk[1] = strchr(qmk[0] + 1, '\"');
        if(qmk[1] && *(qmk[1] + 1) != 0x00)
            qmk[2] = strchr(qmk[1] + 1, '\"');
        if(qmk[2] && *(qmk[2] + 1) != 0x00)
            qmk[3] = strchr(qmk[2] + 1, '\"');

        if(qmk[0] && qmk[1] && qmk[2] && qmk[3]) {
            if(qmk[1] - qmk[0] == 4) {
                // TODO: Verify data
                bool is_cur_code = true;
                bool is_val = true;
                char *dot = qmk[3];

                for(char *c = qmk[0] + 1; c < qmk[1]; c++) {
                    if(*c > 0x5A || *c < 0x41) {
                        is_cur_code = false;
                        break;
                    }
                }
                
                for(char *c = qmk[2] + 1; c < qmk[3]; c++) {
                    if((*c > 0x39 || *c < 0x30) && *c != 0x2e) {
                        is_val = false;
                        break;
                    }
                    else if(*c == 0x2e)
                        dot = c;
                }

                if(is_cur_code && is_val) {
                    // TODO: Copy data to CurrencyInfo instance
                    (*p_cur_info_c)++;
                    if((*p_cur_info_c >= cap)) {
                        cap *= 2;
                        CurrencyInfo *tmp = realloc (
                            (*p_cur_infos),
                            cap * sizeof(CurrencyInfo)
                        );
                    }
                    
                    memset((*p_cur_infos)[(*p_cur_info_c) - 1].code, 0x00, 4);
                    strncpy (
                        (*p_cur_infos)[(*p_cur_info_c) - 1].code,
                        qmk[0] + 1,
                        3
                    );
                    
                    char *ch_mt = (char*) calloc (
                        qmk[3] - qmk[2],
                        sizeof(char)
                    );

                    if(dot == qmk[3]) {
                        strncpy (
                            ch_mt,
                            qmk[2] + 1,
                            qmk[3] - qmk[2] + 1
                        );

                        (*p_cur_infos)[(*p_cur_info_c) - 1].val_exp = 1;
                    }

                    else {
                        if(dot - qmk[2] - 1) {
                            strncpy (
                                ch_mt,
                                qmk[2] + 1,
                                dot - qmk[2] - 1 
                            );
                        }

                        if(qmk[3] - dot - 1) {
                            strncpy (
                                ch_mt + (dot - qmk[2] - 1),
                                dot + 1,
                                qmk[3] - dot - 1
                            );
                            
                            (*p_cur_infos)[(*p_cur_info_c) - 1].val_exp = -(int)(size_t) (qmk[3] - dot - 1);
                        }
                        else (*p_cur_infos)[(*p_cur_info_c) - 1].val_exp = 1;
                    }

                    (*p_cur_infos)[(*p_cur_info_c) - 1].mantissa = atol(ch_mt);
                }

            }

            // TODO: Write meta data
            else {
                if(qmk[1] - qmk[0] - 1) 
                    __pushSplitStr(qmk[0] + 1, qmk[1] - qmk[0] - 1, p_meta, p_meta_c);
                if(qmk[3] - qmk[2] - 1)
                    __pushSplitStr(qmk[2] + 1, qmk[3] - qmk[2] - 1, p_meta, p_meta_c);
            }
        }

        line_beg = line_end;
        line_end++;
        if(line_beg >= &buf[strlen(buf) - 1] || *line_end == 0x00)
            break;

    }

    // TODO: Cleanup
    free(buf);
}


char *metaExtractDate (
    char **meta, 
    size_t meta_c
) {
    // TODO: Find date formated instance
    // Date must be presented in following format: dd.mm.yyyy
    for(size_t i = 0; i < meta_c; i++) {
        char *d1, *d2;
        if((d1 = strchr(meta[i], '.')) && (d2 = strchr(d1 + 1, '.'))) {
            bool is_day, is_month, is_year;
            if 
            (
                d1 - 2 >= meta[i] && 
                *(d1 - 1) >= '0' && *(d1 - 1) <= '9' &&
                *(d1 - 2) >= '0' && *(d1 - 2) <= '9'
            ) is_day = true;
        
            if
            (
                *(d2 - 3) == '.' &&
                *(d2 - 1) >= '0' && *(d2 - 1) <= '9' &&
                *(d2 - 2) >= '0' && *(d2 - 2) <= '9'
            ) is_month = true;

            if 
            (
                d2 + 4 < &meta[i][strlen(meta[i])] &&
                *(d2 + 1) >= '0' && *(d2 + 1) <= '9' &&
                *(d2 + 2) >= '0' && *(d2 + 2) <= '9' &&
                *(d2 + 3) >= '0' && *(d2 + 3) <= '9' &&
                *(d2 + 4) >= '0' && *(d2 + 4) <= '9'
            ) is_year = true;

            if(is_day && is_month && is_year) {
                char *out_date = (char*) calloc (
                    11,
                    sizeof(char)
                );

                strncpy (
                    out_date,
                    d1 - 2,
                    10
                );

                return out_date;
            }
        }
    }

    return NULL;
}
