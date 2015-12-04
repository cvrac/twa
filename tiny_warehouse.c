#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "tiny_warehouse.h"

struct tiny_warehouse {
	char *data_file;
	char *operations_file;
	char *config_file;
	htptr table;
	dllistptr list;
};


twaptr 		twa_destructor(twaptr twa);
int		twa_loadfile(twaptr twa, char *file);
int		twa_opfile(twaptr twa, char *file);
void		twa_insert(twaptr twa, record rec);
void		twa_query(twaptr twa, long customerid);
void		twa_top(twaptr twa, int k);
void		twa_bottom(twaptr twa, int k);
void		twa_average(twaptr twa);
void		twa_range(twaptr twa, long custid1, long custid2);
void		twa_percentile(twaptr twa, long custid);
void 		twa_exit(twaptr twa);

//Used for the insertion of records from the data files
int time = 0;

twaptr twa_constructor(int init_bucks, int buck_size, float load_factor, char *dfile, char *ofile, char *cfile) {
	twaptr twa = malloc(sizeof(struct tiny_warehouse));
	if (twa == NULL) {
		printf("errno = %d\n", errno);
		perror("malloc");
	}
	assert(twa != NULL);

	printf("Allocating memory for tiny warehouse hash table\n");
	twa->table = ht_constructor(init_bucks, buck_size, load_factor);
	printf("Allocating memory for tiny warehouse list\n\n");
	twa->list = dllist_constructor();
	twa->data_file = NULL;
	twa->operations_file = NULL;
	twa->config_file = NULL;
	if (dfile != NULL) {
		twa->data_file = malloc(strlen(dfile)+1);
		strcpy(twa->data_file, dfile);
		free(dfile);
		dfile = NULL;
	}
	if (ofile != NULL) {
		twa->operations_file = malloc(strlen(ofile)+1);
		strcpy(twa->operations_file, ofile);
		free(ofile);
		ofile = NULL;
	}
	if (cfile != NULL) {
		twa->config_file = malloc(strlen(cfile)+1);
		strcpy(twa->config_file, cfile);
		free(cfile);
		cfile = NULL;
	}
	return twa;

}

twaptr twa_destructor(twaptr twa) {
	printf("Destruction of tiny warehouse list\n");
	twa->list = dllist_destructor(twa->list);
	printf("Destruction of tiny warehouse table\n");
	twa->table = ht_destructor(twa->table);
	if (twa->data_file != NULL)
		free(twa->data_file);
	if (twa->operations_file != NULL)
		free(twa->operations_file);
	if (twa->config_file != NULL)
		free(twa->config_file);
	twa->data_file = NULL;
	twa->operations_file = NULL;
	twa->config_file = NULL;
	printf("Destruction of tiny warehouse\n\n");
	free(twa);
	twa = NULL;
	return twa;
}	



int twa_loadfile(twaptr twa, char *file) {
	//Load file to the corresponding twa file-string
	FILE *data;
	int inserted = 0, i, numofrecs;
	long lsize;
	record rec;
	listptr totable;
	printf("Opening %s\n\n", twa->data_file);
	if ((data = fopen(twa->data_file, "rb")) == NULL) {
		printf("errno = %d\n", errno);
		perror("fopen");
		return 1;
	}

	fseek(data, 0, SEEK_END);
	lsize = ftell(data);
	rewind(data);
	numofrecs = (int) lsize/sizeof(rec);

	for (i = 0; i < numofrecs; i++) {
		fread(&rec, sizeof(rec), 1, data);
		if (time == 0) {
			totable = dllist_insert(twa->list, rec);
			ht_insert(twa->table, totable);
		} else
			twa_insert(twa, rec);
		inserted++;
	}	
	time++;
	fclose(data);
	printf("%d records inserted\n\n", inserted);
	return 0;
}


void twa_insert(twaptr twa, record rec) {
	//gets a record and inserts it at twa
	//If already exists, then just update the existing record's amount, and place it at the proper position on the list
	listptr temp = NULL;
	if (!ht_search(twa->table, rec.custid, &temp)) {
		temp = dllist_insert(twa->list, rec);
		ht_insert(twa->table, temp);
		printf("Record inserted\n\n");
	} else {
		dllist_update(twa->list, temp, rec.amount);
		printf("Record updated\n\n");
	}
}

void twa_query(twaptr twa, long customerid) {
	//Searches for customerid customer record, and prints the whole record
	listptr data = NULL;
	if (!ht_search(twa->table, customerid, &data))
		printf("Record with custid:%ld does not exist\n\n", customerid);
	else {
		dllist_printnode(data);
		printf("\n");
	}

}

void twa_top(twaptr twa, int k) {
	//prints top k records of the list
	dllist_topk(twa->list, k);
}

void twa_bottom(twaptr twa, int k) {
	//prints bottomk records of the list
	dllist_botk(twa->list, k);
}

void twa_average(twaptr twa) {
	//sums amounts of all the customers and prints the average amount spent
	dllist_avg(twa->list);
	printf("\n");
}

void twa_range(twaptr twa, long custid1, long custid2) {
	//prints every customer from custid1 to custid2 or vice versa
	listptr node1 = NULL, node2 = NULL;
	if (ht_search(twa->table, custid1, &node1) && ht_search(twa->table, custid2, &node2))
		//use list range function to print customers in ascending order of amount
		if (get_amount(node1) >= get_amount(node2))
			dllist_ascrange(twa->list, node1, node2);
		else
			dllist_ascrange(twa->list, node2, node1);
	else if (node1 == NULL)
		printf("%ld does not exist\n", custid1);
	else if (node2 == NULL)
		printf("%ld does not exist\n", custid2);
}

void twa_percentile(twaptr twa, long custid) {
	int pos = dllist_position(twa->list, custid);
	float percentile;
	percentile = 100 * ((float) pos) / dllist_getsize(twa->list);
	printf("%d%%\n\n", (int)percentile);

}

void twa_exit(twaptr twa) {
	//deletion of every struct created, then termination of the program
	printf("Destroying everything\n");
	twa = twa_destructor(twa);
	printf("Terminating\n");
}

int twa_opfile(twaptr twa, char *file) {
	FILE *ofp;
	record rec;
	int ch, i;
	long id, id1;

	if ((ofp = fopen(file, "r")) == NULL) {
		printf("errno = %d\n", errno);
		perror("fopen");
		return 1;
	}

	while (!feof(ofp)) {
		ch = fgetc(ofp);

		switch (ch) {
			case 'i':
				fscanf(ofp, "%ld%s%s%s%d%s%s%f", &rec.custid, rec.FirstName, rec.LastName, rec.Street,
						&rec.HouseID, rec.postcode, rec.City, &rec.amount);
				twa_insert(twa, rec);
				break;
			case 'q':
				fscanf(ofp, "%ld", &id);
				twa_query(twa, id);
				break;
			case 't':
				fscanf(ofp, "%d", &i);
				twa_top(twa, i);
				break;
			case 'b':
				fscanf(ofp, "%d", &i);
				twa_bottom(twa, i);
				break;
			case 'a':
				twa_average(twa);
				break;
			case 'r':
				fscanf(ofp, "%ld%ld", &id, &id1);
				twa_range(twa, id, id1);
				break;
			case 'p':
				fscanf(ofp, "%ld", &id);
				twa_percentile(twa, id);
				break;
		}
	}
	fclose(ofp);
	return 0;
}

void twa_prompt(twaptr twa) {
	if (twa->data_file != NULL)
		if (twa_loadfile(twa, twa->data_file))
			printf("Problem during data file loading\n");
	if (twa->operations_file != NULL && twa->data_file != NULL)
		if (twa_opfile(twa, twa->operations_file))
			printf("Problem during operations file loading\n");

	int ch, i, exit = 1;
	long id, id1;
	record rec;
	char *nfile;
	while (exit) {
		printf(">");
		ch = getchar();

		switch (ch) {
			case 'l':
				ch = getchar();
				nfile = malloc(50);
				scanf("%s", nfile);
				printf("%s %d\n", nfile, (int)strlen(nfile));
				twa->data_file = realloc(twa->data_file, strlen(nfile)+1);
				strcpy(twa->data_file, nfile);
				if (twa_loadfile(twa, twa->data_file))
					printf("Problem during data file loading\n");
				free(nfile);			
				break;
			case 'i':
				scanf("%ld%s%s%s%d%s%s%f", &rec.custid, rec.LastName, rec.FirstName, rec.Street,
						&rec.HouseID, rec.postcode, rec.City, &rec.amount);
				twa_insert(twa, rec);
				break;
			case 'q':
				scanf("%ld", &id);
				twa_query(twa, id);
				break;
			case 't':
				scanf("%d", &i);
				twa_top(twa, i);
				break;
			case 'b':
				scanf("%d", &i);
				twa_bottom(twa, i);
				break;
			case 'a':
				twa_average(twa);
				break;
			case 'r':
				scanf("%ld%ld", &id, &id1);
				twa_range(twa, id, id1);
				break;
			case 'p':
				scanf("%ld", &id);
				twa_percentile(twa, id);
				break;
			case 'e':
				twa_exit(twa);
				exit = 0;
				break;
			default:
				break;
		}
		ch = getchar();
	}
}
