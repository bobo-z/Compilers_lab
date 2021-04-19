#include "common.h"

Symbol init()
{
    int i;
    for(i=0;i<TAB_SIZE;i++)
        symbol_tab[i] = NULL;
}
unsigned int hash_bkdr(char* name)
{
    unsigned int hash = 0;
    unsigned int seed = 131;
    while(*name)
    {
        hash = hash * seed + (*name++);
    }
    return hash%32767;
}
