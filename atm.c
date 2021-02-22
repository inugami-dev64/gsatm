#include "converter.h"
#include <stdlib.h>
#define __USER_C
#include "atm.h"
#include "converter.h"
#include "parse_csv.h"

#define bool uint8_t
#define true 1
#define false 0

static CurrencyInfo *cur_infos = NULL;
static size_t cur_info_c = 0;

void __initAtm() {
    initConverter();
    printf("Welcome to Goldstein Bank ATM!\n");
    fetchExRates(ATM_EX_RATES_FILE);

    char **meta = NULL;
    size_t meta_c = 0;
    parseCurrencyInfo (
        ATM_EX_RATES_FILE,
        &cur_infos, 
        &cur_info_c,
        &meta,
        &meta_c
    );

    char *date = metaExtractDate(meta, meta_c);
    printf("Recieved exchange rates from: %s\n", date);

    // TODO: Free memory for meta data 
    for(size_t i = 0; i < meta_c; i++) {
        if(meta[i])
            free(meta[i]);
    }

    /*if(meta) */
        /*free(meta);*/
}


InputAction __inputParse(char *in, char ***p_in_params) {
    // TODO: Extract user parameters
    char *p_ins = in, *n_ins = in;
    char *words[512] = {0};
    size_t len[512] = {0};
    if(!in)
        return ATM_INPUT_ACTION_UNKNOWN;

    in[strlen(in) - 1] = 0x00;
    
    size_t i = 0;
    char *tmp = NULL;
    while((tmp = strchr(n_ins, 0x20)) || (tmp = strchr(n_ins, 0x00))) {
        n_ins = tmp;
        while(p_ins < n_ins && *p_ins == 0x20)
            p_ins++;

        n_ins++;
        if(p_ins >= n_ins)
            continue;

        words[i] = p_ins;
        len[i] = n_ins - p_ins;
        i++;

        if(!(*n_ins) || !(*p_ins))
            break;
    }

    (*p_in_params) = (char**) calloc (
        i + 1,
        sizeof(char*)
    );

    memcpy (
        (*p_in_params),
        words,
        i + 1
    );
    
    // TODO: Check input actions
    if(i + 1 >= 1 && !strncmp(words[0], "exit", len[0]))
        return ATM_INPUT_ACTION_EXIT;
    if(i + 1 >= 1 && !strncmp(words[0], "help", len[0]))
        return ATM_INPUT_ACTION_HELP;
    if(i + 1 >= 1 && !strncmp(words[0], "list", len[0])) {
        if(i + 1 >= 2 && !strncmp(words[1], "rates", len[1]))
            return ATM_INPUT_ACTION_LIST_RATES;
        else if(i + 1 >= 2 && !strncmp(words[1], "status", len[1]))
            return ATM_INPUT_ACTION_LIST_STATUS;
        else 
            return ATM_INPUT_ACTION_UNKNOWN;
    }
    if(i + 1 >= 4 && !strncmp(words[1], "convert", len[1]) && atof(words[1]))
        return ATM_INPUT_ACTION_CONVERT;

    return ATM_INPUT_ACTION_UNKNOWN;
}


void __inputPoll() {
    char buf[2048];

    // TODO: Poll input from end user
    while(true) {
        memset(buf, 0, 2048);
        printf("ATM> ");
        fflush(stdin);
        fgets(buf, 2048, stdin);
        
        char **params = NULL;
        InputAction ia = __inputParse(buf, &params);

        switch(ia) 
        {
        case ATM_INPUT_ACTION_EXIT:
            exit(EXIT_SUCCESS);
            break;

        case ATM_INPUT_ACTION_HELP:
            printf("%s", main_help_c); 
            break;

        default:
            ATM_UNKNOWN_COMMAND(buf);
            break;
        }
    }
}


int main() {
    __initAtm();
    __inputPoll();

    if(cur_infos)
        free(cur_infos);

    return EXIT_SUCCESS;
}
