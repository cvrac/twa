#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hash_table.h"
#include "record.h"

typedef struct bucketz *bucket;


typedef struct sRec {
	long int custid;
	listptr data_ptr;
} sRec;



struct bucketz {
	//size of the bucket
	int size;

	//current number of records on the bucket
	int count;

	//records on the bucket	
	sRec *records;

	//overflow bucket pointer
	bucket overflow_bucket;
};


struct hash_table {

	//initial buckets
	int m;

	//current number of buckets
	int cbucks;

	//index for the next bucket to be split
	int p;

	//round, initially 0
	int i;

	//bucket size
	int bsize;

	//number of elements on the table
	int numof_elements;	

	//load factor value
	float load_factor;

	//table of pointers to buckets
	bucket *t_bucket;
};


void split(htptr table);
void fix_index(htptr table, bucket spbuck);
void print(htptr table);
bucket init_buck(int l);

int mypow(int base, int exp) {
	int res = 1;
	for (; exp > 0; exp--)
		res *= base;
	return res;
}

int hash(long id, int round, int bucks) {
	return id % (mypow(2, round) * bucks);
}

float load_factor(int numof_elements, int m, int l) {
	return ((float) numof_elements/(m*l));
}

int tsize(htptr table) {
	return table->cbucks;
}

int b_size(htptr table) {
	return table->t_bucket[0]->size;
}

bucket init_buck(int l) {
	int k;
	bucket buck = malloc(sizeof(struct bucketz));
	if (buck == NULL) {
		printf("Error allocating memory for the bucket\n");
		return NULL;
	}
	buck->size = l;
	buck->count = 0;
	buck->records = malloc(buck->size * sizeof(struct sRec));
	for (k = 0; k < buck->size; k++) {
		buck->records[k].custid = -1;
		buck->records[k].data_ptr = NULL;
	}
	buck->overflow_bucket = NULL;
	return buck;
}

htptr ht_constructor(int initb, int l, float lf) {

	htptr table;

	table = malloc(sizeof(struct hash_table));
	assert(table != NULL);
	table->m = initb;
	table->p = 0;
	table->i = 0;
	table->bsize = l;
	table->cbucks = initb;
	table->numof_elements = 0;
	table->load_factor = lf;
	table->t_bucket = malloc(table->m * sizeof(struct bucketz*));
	int it;
	for (it = 0; it < table->m; it++) 
		table->t_bucket[it] = init_buck(l);
	return table;
}

htptr ht_destructor(htptr table) {
	int it;
	bucket temp, prev;
	for (it = 0; it < table->cbucks; it++) {
		temp = table->t_bucket[it]->overflow_bucket;
		while ( temp != NULL) {
			prev = temp;
			free(temp->records);
			temp->records = NULL;
			temp = temp->overflow_bucket;
			free(prev);
		}
		free(table->t_bucket[it]->records);
		free(table->t_bucket[it]);
	}
	free(table->t_bucket);
	free(table);
	return NULL;

}

int ht_numofel(htptr table) {
	return table->numof_elements;
}

void ht_insert(htptr table, listptr data) {

	long id = dllist_getid(data);;
	int pos = hash(id, table->i, table->m);
	int size = table->t_bucket[0]->size;
	int j;


	//Apply hash function. If result < pointer p, then rehash using next round's function
	if (pos < table->p && table->p != 0) 
		pos = hash(id, table->i+1, table->m);

	bucket temp = table->t_bucket[pos];

	while (temp->overflow_bucket != NULL){
		if (temp->count < temp->size) break; 
		temp = temp->overflow_bucket;
	}

	if (temp->count < size && temp != NULL) {
		for (j = 0; j < temp->size; j++) 
			if (temp->records[j].custid == -1) {
				temp->records[j].custid = id;
				temp->records[j].data_ptr = data;
				temp->count++;
				break;
			}
	} else  {
		//overflow bucket things
		temp->overflow_bucket = init_buck(table->bsize);
		temp->overflow_bucket->records[0].custid = id;
		temp->overflow_bucket->records[0].data_ptr = data;
		temp->overflow_bucket->count++;


	}
	table->numof_elements++;

	if (load_factor(table->numof_elements, table->cbucks, size) > table->load_factor) {
		table->t_bucket = realloc(table->t_bucket, (++table->cbucks * sizeof(struct bucketz*)));
		table->t_bucket[table->cbucks-1] = init_buck(table->bsize);
		
		split(table);
		if ( table->cbucks == mypow(2, table->i+1) * table->m) {
			table->i++;
			table->p = 0;
		} else
			table->p++;
	}

}

int ht_search(htptr table, long id, listptr *data) {
	int res = hash(id, table->i, table->m);
	if (res < table->p && table->p != 0) 
		res = hash(id, table->i+1, table->m);

	bucket temp = table->t_bucket[res];
	int found = 0;
	int i;
	while (!found) {
		if (temp == NULL) break;
		for ( i = 0; i < temp->size; i++ )
			if (temp->records[i].custid == id) {
				found = 1;
				*data = temp->records[i].data_ptr;
				break;
			} else continue;
		temp = temp->overflow_bucket;
	}
	return found ? 1 : 0;

}

void split(htptr table) {
	/*Iterate bucket to be split. If elements hash to this bucket, leave them as they are,
	  else put them at the new bucket.*/
	int k, j, i;
	int pos;
	bucket temp = table->t_bucket[table->p];
	bucket tempx = table->t_bucket[table->cbucks-1];
	int forhash = table->i+1;
	while ( temp  != NULL ) {
		for ( k = 0; k < temp->size; k++) {
			if (temp->records[k].custid == -1) continue;
			pos = hash(temp->records[k].custid, forhash, table->m);
			if ( pos == table->p)
				continue;
			else {
				tempx = table->t_bucket[table->cbucks-1];
				while ( tempx->overflow_bucket != NULL ) {
					if (tempx->count < tempx->size) break;
					tempx = tempx->overflow_bucket;
				}
				if (tempx->count < tempx->size && tempx != NULL) {
					for (j = 0; j < tempx->size; j++)
						if (tempx->records[j].custid == -1) {
							tempx->records[j].custid = temp->records[k].custid;
							tempx->records[j].data_ptr = temp->records[k].data_ptr;
							tempx->count++;
							temp->records[k].custid = -1;
							temp->records[k].data_ptr = NULL;
							temp->count--;
							break;
						}
				} else {
					tempx->overflow_bucket = init_buck(table->bsize);
					tempx->overflow_bucket->records[tempx->overflow_bucket->count].custid = temp->records[k].custid;
					tempx->overflow_bucket->records[tempx->overflow_bucket->count].data_ptr = temp->records[k].data_ptr;
					tempx->overflow_bucket->count++;
					temp->records[k].custid = -1;
					temp->records[k].data_ptr = NULL;
					temp->count--;
				}

			}
		}
		temp = temp->overflow_bucket;
	}
	fix_index(table, table->t_bucket[table->p]);
}


void fix_index(htptr table, bucket spbuck) {
	/*Gets a bucket at an index. Sorts elements on every bucket of this index, so that if any overflow bucket
	  isnt needed anymore, it's deleted*/
	int i, k = 0, j;
	bucket temp = spbuck, tempx = NULL, lastnotempty = NULL;
	while (temp != NULL) {
		tempx = temp->overflow_bucket;
		while ( temp->count != temp->size && tempx != NULL) {

			for ( i = 0; i < tempx->size; i++) 
				if ( tempx->records[i].custid != -1 && temp->count < temp->size) {
					for (j = 0; j < temp->size; j++)
						if (temp->records[j].custid == -1) {
							temp->records[j].custid = tempx->records[i].custid;
							temp->records[j].data_ptr = tempx->records[i].data_ptr;
							temp->count++;
							tempx->records[i].custid = -1;
							tempx->records[i].data_ptr = NULL;
							tempx->count--;
							break;
						}	
				} 
			if (tempx->count == 0 ) tempx = tempx->overflow_bucket;
		}
		if (temp->overflow_bucket != NULL)
			temp = temp->overflow_bucket;
		else
			break;
	}
	temp = spbuck, tempx = NULL;
	while (temp->overflow_bucket != NULL ) {
		lastnotempty = temp;
		temp = temp->overflow_bucket;
	}
	if (spbuck->count == 0 && spbuck->overflow_bucket != NULL) {
		temp = spbuck->overflow_bucket;
		lastnotempty = spbuck; 
	}
	if (lastnotempty != NULL) {
		while ( temp != NULL ) {
			tempx = temp->overflow_bucket;
			if (temp->count <= 0)
			{
				free(temp->records);
				temp->records = NULL;
				temp->overflow_bucket = NULL;
				free(temp);
			} else lastnotempty = temp;
			if (tempx == NULL) break;
			temp = tempx;
		}
		if (lastnotempty->overflow_bucket != NULL)
			lastnotempty->overflow_bucket = NULL;	
	}
	
	
}


void ht_print(htptr table) {
	int i = 0;
	bucket temp;

	for (i = 0; i < table->cbucks; i++) {
		temp = table->t_bucket[i];
		printf("Bucket[%d] : ", i);
		while (temp != NULL) {
			int k;
			for (k = 0;  k < temp->size; k++)
				if (temp->records != NULL)
					if ( temp->records[k].custid != -1)
						printf("%ld ", temp->records[k].custid);
			printf("->");
			if (temp->overflow_bucket != NULL)
				temp = temp->overflow_bucket;
			else break;
		}
		printf("\n");
	}
	printf("%d records on the table\n", table->numof_elements);

}
