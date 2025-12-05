#ifndef ACSRAMDMP_RPC
#define ACSRAMDMP_RPC
#include <stdint.h>
#define ACSRAM_DUMP_MAXCHUNK_SIZE 0x800
struct DumpSram {
    int32_t result;
    uint32_t size;
    uint32_t off;
    uint8_t buffer[ACSRAM_DUMP_MAXCHUNK_SIZE];
};

struct CheckAcram {
    int32_t ram32_write_res;
    int32_t ram32_read_res;
    int32_t ram32_result;
    
    int32_t ram64_write_res;
    int32_t ram64_read_res;
    int32_t ram64_result;
};

enum {
    ACSRAM_RPC_IRX = 0xac5,
    ACSRAM_DUMP_CHUNK = 0xb00b5,
    ACRAM_CHECK_CAPACITY,
};

#endif