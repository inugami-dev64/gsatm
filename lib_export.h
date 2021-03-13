#ifndef __LIB_EXPORT_H
#define __LIB_EXPORT_H

#ifdef _WIN32
    #define LIB_EXPORT      __declspec(dllexport)
    #define CALL_CON        __stdcall
#else 
    #define LIB_EXPORT  
    #define CALL_CON    
#endif

#endif
