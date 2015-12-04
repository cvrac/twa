#ifndef TINY_WAREHOUSE_H
#define TINY_WAREHOUSE_H

#include "record.h"
#include "hash_table.h"
#include "dl_list.h"

typedef struct tiny_warehouse *twaptr;

twaptr twa_constructor(int init_bucks, int buck_size, float load_factor, char *dfile, char *ofile, char *cfile);
void twa_prompt(twaptr twa);

#endif
