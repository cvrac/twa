#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "record.h"
#include "dl_list.h"


typedef struct hash_table *htptr;

htptr		ht_constructor(int initb, int l, float lf);
htptr		ht_destructor(htptr table);
void		ht_insert(htptr table, listptr data);
int		ht_search(htptr table, long id, listptr *data);
void		ht_print(htptr table);
int		ht_numofel(htptr table);
#endif
