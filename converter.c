#define __CONVERTER_C
#include "converter.h"

void initConverter() {
    // TODO: Initialise hashmap for ISO 4217 currency codes
    newHashmap (
        &cur_map, 
        (int) 2 * ARR_LEN(iso4217)
    );
    
    cis = (CurrencyInfo*) calloc (
        ARR_LEN(iso4217),
        sizeof(CurrencyInfo)
    );
    for(size_t i = 0; i < ARR_LEN(iso4217); i++) {
        cis[i].code = iso4217[i];
        cis[i].name = cur_names[i];
        if(!strcmp(cis[i].code, "EUR")) {
            cis[i].ex.mantissa = 10000;
            cis[i].ex.val_exp = -4;
        }

        pushToHashmap (
            &cur_map, 
            iso4217[i],
            3,
            &cis[i]
        );
    }
}

void destroyConverter() {
    free(cis);
    free(cur_map.map_data);
}

Hashmap *getCurrencyMap() {
    return &cur_map;
}
