#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dl_list.h"
#include "record.h"

struct dl_list {
	int size;
	listptr first;
	listptr last;
	listptr curr;
};

struct lnode {
	record *data;
	listptr next;
	listptr prev;
};

void print_data(record rec);
void set_amount(listptr temp, float amt);

dllistptr dllist_constructor () {

	dllistptr mylist;
	mylist = malloc(sizeof(struct dl_list));
	assert(mylist != NULL);
	
	mylist->first = NULL;
	mylist->last = NULL;
	mylist->curr = NULL;
	mylist->size = 0;

	return mylist;

}

int dllist_empty ( dllistptr mylist )
{
	return ( mylist->size == 0 );	
}


dllistptr dllist_destructor ( dllistptr mylist ) {

	if ( mylist != NULL && !dllist_empty(mylist)) {
		listptr pt = mylist->first;
		while ( pt != mylist->last ) {
			listptr target = pt;
			pt = target->next;
			free(target->data);
			free(target);
			target = NULL;
		}
		free(mylist->last->data);
		free(mylist->last);
	}
	mylist->first = NULL;
	mylist->last = NULL;
	mylist->curr = NULL;
	mylist->size = 0;
	free(mylist);
	mylist = NULL;

	return mylist;
}


void dllist_topk ( dllistptr mylist, int topk ) {
	mylist->curr = mylist->first;


	for ( ; topk > 0 ; topk--) {
		if ( mylist->curr != NULL )  
			dllist_printnode(mylist->curr);
		else break;
		mylist->curr = mylist->curr->next;
	}		

	printf("\n");

}


void dllist_botk ( dllistptr mylist, int botk ) {
	mylist->curr = mylist->last;


	for ( ; botk > 0 ; botk--) {
		if ( mylist->curr != NULL ) 
			dllist_printnode(mylist->curr);
		else break;
		mylist->curr = mylist->curr->prev;
	}
	printf("\n");
}

float get_amount(listptr temp) {
	return temp->data->amount;
}

void set_amount(listptr temp, float amt) {
	temp->data->amount += amt;
}

void dllist_printnode (listptr temp) {
	print_data (*(temp->data));
}

void print_data (record rec) {
	printf("%ld: %s~%s~%s~%d~%s~%s~%-9.2f\n", \
			rec.custid, rec.LastName, rec.FirstName, \
			rec.Street, rec.HouseID, rec.City, rec.postcode, \
			rec.amount);
}

void dllist_print(dllistptr mylist) {

	for ( mylist->curr = mylist->last; mylist->curr != mylist->first; mylist->curr = mylist->curr->prev) 
		dllist_printnode(mylist->curr);

}


listptr dllist_insert ( dllistptr mylist, record data ) {
	listptr temp = malloc(sizeof(struct lnode));
	temp->data = malloc(sizeof(struct record));
	*(temp->data) = data;	

	if ( dllist_empty(mylist) ) {
		mylist->first = temp;
		mylist->last = temp;
		mylist->curr = temp;
	} else {

		if ( get_amount(mylist->first) < data.amount ) {
			temp->next = mylist->first;
			mylist->first->prev = temp;
			mylist->first = temp;
			mylist->curr = mylist->first;
		} else if ( get_amount(mylist->last) > data.amount ) {
			temp->prev = mylist->last;
			mylist->last->next = temp;
			mylist->last = temp;
			mylist->curr = mylist->first;
		} else {
			mylist->curr = mylist->first;
			while ( get_amount(mylist->curr) >= data.amount && mylist->curr != mylist->last) 
				mylist->curr = mylist->curr->next;

			mylist->curr->prev->next = temp;
			temp->prev = mylist->curr->prev;
			temp->next = mylist->curr;
			mylist->curr->prev = temp;
		}
	}
	mylist->size++;
	return temp;
}


void dllist_avg(dllistptr mylist) {
	float average, sum = 0.0;
	mylist->curr = mylist->first;

	while (mylist->curr != mylist->last) {
		sum += get_amount(mylist->curr);
		mylist->curr = mylist->curr->next;
	}
	sum += get_amount(mylist->last);
	average = (float) sum / mylist->size;

	printf("%.6f\n", average);
}

long dllist_getid(listptr node) {
	return node->data->custid;
}

void dllist_ascrange(dllistptr mylist, listptr node1, listptr node2) {
	mylist->curr = node1->next;

	while (mylist->curr != node2) {
		dllist_printnode(mylist->curr);
		mylist->curr = mylist->curr->next;
	}
	printf("\n");
}


int dllist_position(dllistptr mylist, long id) {
	int pos = 1;
	mylist->curr = mylist->last;
	while (mylist->curr != NULL) {
		if (dllist_getid(mylist->curr) == id) 
			break;
		pos++;
		mylist->curr = mylist->curr->prev;
	}
	return pos;	
}

void dllist_update(dllistptr mylist, listptr temp, float amount) {

	if (mylist->first == temp) 
		mylist->first = temp->next;
	else if (mylist->last == temp) 
		mylist->last = temp->prev;
	else {
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
	}
	temp->next = NULL;
	temp->prev = NULL;
	
	set_amount(temp, amount);


	if (get_amount(mylist->first) < get_amount(temp)) {
		temp->next = mylist->first;
		mylist->first->prev = temp;
		mylist->first = temp;
		mylist->curr = mylist->first;
	} else if (get_amount(mylist->last) > get_amount(temp)) {
		temp->prev = mylist->last;
		mylist->last->next = temp;
		mylist->last = temp;
		mylist->curr = mylist->first;
	} else {
		mylist->curr = mylist->first;
		while ( get_amount(mylist->curr) >= get_amount(temp) && mylist->curr != mylist->last)
			mylist->curr = mylist->curr->next;

		mylist->curr->prev->next = temp;
		temp->prev = mylist->curr->prev;
		temp->next = mylist->curr;
		mylist->curr->prev = temp;
	}

}

int dllist_getsize(dllistptr mylist) {
	return mylist->size;
}
