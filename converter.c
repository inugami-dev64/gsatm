#define __CONVERTER_C
#include "converter.h"

void initConverter() {
    // TODO: Initialise hashmap for ISO 4217 currency codes
    newHashmap (
        &cur_map, 
        (int) 2 * ARR_LEN(iso4217)
    );
    
    for(size_t i = 0; i < ARR_LEN(iso4217); i++) {
        pushToHashmap (
            &cur_map, 
            &iso4217[i],
            3,
            cur_names[i]
        );
    }
}



