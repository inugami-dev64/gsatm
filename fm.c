#define __FM_C
#include "fm.h"

extern int errno;

/*****************************/
/***** Private functions *****/
/*****************************/

char *__readFileToBuffer(char *file_name) {
    // TODO: Read csv file into char buffer
    FILE *file;
    file = fopen(file_name, "rb");

    if(!file) FILE_ERR(file_name, "__readFileToBuffer");
    
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buf = (char*) calloc (
        len + 1,
        sizeof(char)
    );

    size_t res = fread (
        buf, 
        sizeof(char), 
        len, 
        file
    );

    if(!res) READ_ERR(file_name);

    return buf;
}


/***************************************/
/******** CSV PARSING FUNCTIONS ********/
/***************************************/
LIB_EXPORT void CALL_CON csv_FetchExRates(char *out_file) {
    // TODO: Fetch information about exchange rates
    CURL *curl;
    FILE *file;

    file = fopen(out_file, "wb");
    if(!file)
        FILE_ERR(out_file, "csv_FetchExRates");

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, ATM_EX_RATE_DATA_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    printf("Fetching currency exchange rates...\n");

    curl_easy_perform(curl);
    fclose(file);
}


LIB_EXPORT void CALL_CON csv_ParseCurrencyInfo (
    char *csv_file,
    Hashmap *p_cur_map,
    char ***p_codes,
    size_t *p_code_c,
    char ***p_meta,
    size_t *p_meta_c
) {
    char *buf = __readFileToBuffer(csv_file);

    // TODO: Allocate memory for currency infos
    size_t cap = __ATM_DEFAULT_CURRENCY_C;
    size_t meta_cap = 4;
    
    *p_code_c = 0;
    *p_codes = (char**) calloc (
        cap,
        sizeof(char*)
    );
    
    // TODO: Allocate memory for metadata
    *p_meta_c = 0;
    (*p_meta) = (char**) calloc (
        meta_cap,
        sizeof(char*)
    );

    // TODO: Extract metadata and currency information from buffer
    char *line_beg = buf, *line_end = buf;
    CurrencyInfo *p_cur;
    while((line_end = strchr(line_end, 0x0A))) {
        p_cur = NULL;
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
                    // TODO: Add information to CurrencyInfo instance
                    p_cur = findValue(p_cur_map, qmk[0] + 1, 3); 
                    
                    if(!p_cur) {
                        char code[4] = {0};
                        strncpy(code, qmk[0] + 1, 3);
                        CUR_CODE_ERR(code);
                    }

                    (*p_code_c)++;
                    if((*p_code_c) >= cap) {
                        cap *= 2;
                        char **tmp = (char**) realloc (
                            *p_codes,
                            cap * sizeof(char*)
                        );

                        if(!tmp) REALLOC_ERR("iso4217 codes");
                        *p_codes = tmp;
                    }

                    (*p_codes)[(*p_code_c) - 1] = (char*) malloc(4);
                    *((*p_codes)[(*p_code_c) - 1] + 3) = 0x00;
                    strncpy (
                        (*p_codes)[(*p_code_c) - 1],
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

                        p_cur->ex.val_exp = 1;
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
                            
                            p_cur->ex.val_exp = -(int)(size_t) (qmk[3] - dot - 1);
                        }
                        else p_cur->ex.val_exp = 1;
                    }

                    p_cur->ex.mantissa = atol(ch_mt);
                    free(ch_mt);
                }

            }

            // TODO: Write meta data
            else {
                if(qmk[1] - qmk[0] - 1) 
                    str_PushSplitStr(qmk[0] + 1, qmk[1] - qmk[0] - 1, p_meta, p_meta_c);
                if(qmk[3] - qmk[2] - 1)
                    str_PushSplitStr(qmk[2] + 1, qmk[3] - qmk[2] - 1, p_meta, p_meta_c);
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


LIB_EXPORT char* CALL_CON csv_MetaExtractDate (
    char **meta, 
    size_t meta_c
) {
    // TODO: Find date formated instance
    // Date must be presented in following format: dd.mm.yyyy
    for(size_t i = 0; i < meta_c; i++) {
        char *d1, *d2;
        if((d1 = strchr(meta[i], '.')) && (d2 = strchr(d1 + 1, '.'))) {
            bool is_day, is_month, is_year;
            is_day = false;
            is_month = false;
            is_year = false;
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


/****************************************/
/******** CASH LOADER FUNCTIONS *********/
/****************************************/
LIB_EXPORT void CALL_CON cash_ParseData (
    char *cash_file, 
    Hashmap *p_cm,
    char ***p_codes,
    size_t *p_code_c
) {
    // TODO: Load buffer with cash data and remove comments
    char *buf = __readFileToBuffer(cash_file);
    size_t line_c = str_Lc(buf);
    str_RmLineComments('#', buf);
    size_t line_i = 1;

    CurrencyInfo *p_ci;
    size_t cap = __ATM_DEFAULT_CURRENCY_C;
    size_t note_cap = __ATM_DEFAULT_NOTE_C;
    *p_code_c = 0;
    (*p_codes) = (char**) calloc (
        cap,
        sizeof(char*)
    );

    // TODO: Start iterating buffer line by line
    char *next = buf, *prev = buf;
    char *tmp = buf;
    while((next = strchr(tmp, 0x0A)) || (next = strchr(tmp, 0x00))) {
        if(!(next - prev)) break;
        while(*prev == 0x20 && prev < next) prev++;
        if(prev == next) {
            next++;
            if(next >= &buf[strlen(buf)] || *next == 0x00)
                break;
            prev = next;
            tmp = next;
            continue;
        }
        char code[4];
        memset(code, 0, 4);
        
        if(prev + 3 < next) {
            memcpy(code, prev, 3);
            prev += 3;
        }

        if(!str_IsAlphabetical(code, 3))
            PARSE_ERR(cash_file, line_i);
        
        str_ToUpperCase(code, 3);
        p_ci = findValue(p_cm, code, 3);
        if(!p_ci) CUR_CODE_ERR(code);

        (*p_code_c)++;
        if((*p_code_c) >= cap) {
            cap *= 2;
            char **ra_tmp = (char**) realloc (
                *p_codes,
                cap * sizeof(char)
            );

            if(!ra_tmp) REALLOC_ERR("cash loaded currency codes");
            *p_codes = ra_tmp;
        }

        (*p_codes)[(*p_code_c) - 1] = (char*) malloc(4 * sizeof(char));
        *((*p_codes)[(*p_code_c) - 1] + 3) = 0x00;
        strcpy ( 
            (*p_codes)[(*p_code_c) - 1],
            code
        );

        bool found_hyphen = false;
        while(prev < next && (*prev == 0x20 || *prev == '-')) {
            if(*prev == '-' && !found_hyphen)
                found_hyphen = true;
            else if(*prev == '-' && found_hyphen)
                PARSE_ERR(cash_file, line_i);
            prev++;
        }

        // TODO: Extract banknote information
        uint64_t banknote_c = 0;
        uint64_t *banknote_vals = (uint64_t*) calloc (
            note_cap, 
            sizeof(uint64_t)
        );

        int64_t *val_c = (int64_t*) calloc (
            note_cap,
            sizeof(int64_t)
        );
        char note_val[32], note_c[32];
        
        // TODO: Extract information from cash file to CashStatus structs
        char *info_br = prev;
        char *scs, *nls;
        tmp = prev;

        while (
            (
                strchr(tmp, (int) ';') || 
                strchr(tmp, 0x0A)
            ) && 
            info_br <= next
        ) {
            // Find appropriate separator 
            scs = strchr(tmp, (int) ';');
            nls = strchr(tmp, 0x0A);
            if(scs && nls)
                info_br = scs < nls ? scs : nls;
            else if(!scs)
                info_br = nls;
            else if(!nls)
                info_br = scs;
            
            // Reallocate memory for banknote values if needed
            banknote_c++;
            if(banknote_c >= note_cap) {
                note_cap *= 2;
                uint64_t *tmp_val = (uint64_t*) realloc (
                    banknote_vals,
                    note_cap * sizeof(uint64_t)
                );

                if(!tmp_val) REALLOC_ERR("banknote name values");

                banknote_vals = tmp_val;
                tmp_val = (uint64_t*) realloc (
                    val_c,
                    note_cap * sizeof(uint64_t)
                );

                if(!tmp_val) REALLOC_ERR("banknote counts");
                val_c = (int64_t*) tmp_val;
            }

            // Skip all whitespaces and tabs
            while(prev < info_br && (*prev == 0x20 || *prev == '\t')) 
                prev++;
            
            // Check for any parsing errors in cash file
            char *sp = strchr(prev, 0x20);
            char *hy = strchr(prev, (int) '-');
            char *end;

            if(!hy || sp >= info_br || hy >= info_br)
                PARSE_ERR(cash_file, line_i);

            end = sp < hy ? sp : hy;
            memset(note_val, 0, 32);
            memset(note_c, 0, 32);

            strncpy (
                note_val,
                prev,
                end - prev
            );            

            prev = end;
            while(prev < info_br && (*prev == 0x20 || *prev == '-')) prev++;
            end = prev;
            while(end < info_br && *end < 0x3A && *end > 0x2F) end++;
            
            if(!(end - prev))
                PARSE_ERR(cash_file, line_i);
            else {
                strncpy (
                    note_c,
                    prev,
                    end - prev
                );

                banknote_vals[banknote_c - 1] = atoi(note_val);
                val_c[banknote_c - 1] = atoi(note_c);
            }

            info_br++;
            if(info_br >= next || !*info_br) 
                break;
            prev = info_br;
            tmp = info_br;
        }
        p_ci->cs.banknote_c = banknote_c;
        p_ci->cs.banknote_vals = banknote_vals;
        p_ci->cs.val_c = val_c;

        al_SortBankNotes(&p_ci->cs); 

        line_i++;
        next++;
        tmp = next;
        prev = next; 
        if(next >= &buf[strlen(buf)] || !(*next))
            break;
    }
}
