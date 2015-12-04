#ifndef _RECORD_H_
#define _RECORD_H_

#define SIZE 20
#define SSIZE 6

typedef struct record {
	long custid;
	char FirstName[SIZE];
	char LastName[SIZE];
	char Street[SIZE];
	int HouseID;
	char City[SIZE];
	char postcode[SSIZE];
	float amount;
} record;


#endif
