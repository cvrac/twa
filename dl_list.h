#ifndef	DL_LIST_H
#define	DL_LIST_H

#include "record.h"

typedef struct lnode *listptr;
typedef struct dl_list *dllistptr;



dllistptr	dllist_constructor();
dllistptr	dllist_destructor();
int		dllist_empty();
listptr		dllist_insert(dllistptr mylist, record data);
void		dllist_first();
void		dllist_last();
int 		dllist_top(int k);
int		dllist_bottom(int k);
void		dllist_print( dllistptr mylist);
void		dllist_avg(dllistptr mylist);
long		dllist_getid(listptr node);
void		dllist_printnode(listptr temp);
void		dllist_ascrange(dllistptr mylist, listptr node1, listptr node2);
void            dllist_descrange(dllistptr mylist, listptr node1, listptr node2);
void		dllist_update(dllistptr mylist, listptr temp, float amount);
float		get_amount(listptr temp);
int		dllist_position(dllistptr mylist, long id);
int		dllist_getsize(dllistptr mylist);
#endif 
