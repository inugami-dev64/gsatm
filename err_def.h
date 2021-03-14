#ifndef ERR_DEF_H
#define ERR_DEF_H

/* This file contains macros to most common error messages */
#define REALLOC_ERR(purpose)        fprintf(stderr, "Failed to reallocate memory for %s\n", purpose), \
                                    exit(EXIT_FAILURE)

#define PARSE_ERR(file, line)       fprintf(stderr, "Parsing error in file %s on line %lld\n", file, line), \
                                    exit(EXIT_FAILURE)

#define CMD_ERR_MSG(cmd)            fprintf(stderr, "Unknown command %s\n", cmd)
#define MEM_CORRUPTION(fail_msg)    fprintf(stderr, "%s\nOut of memory, possible data corruption\n", fail_msg), \
                                    exit(EXIT_FAILURE)

#define CUR_CODE_ERR(code)          fprintf(stderr, "No currency with code %s available\n", code), \
                                    exit(EXIT_FAILURE)

#define GEN_ERR(msg)                fprintf(stderr, "%s\nAborting\n", msg), \
                                    exit(EXIT_FAILURE)

#define FILE_ERR(file, func)        fprintf(stderr, "Failed to open file %s in %s\n", file, func), \
                                    exit(EXIT_FAILURE)

#define READ_ERR(file)              fprintf(stderr, "Failed to read from file %s\n", file), \
                                    exit(EXIT_FAILURE)
#endif
