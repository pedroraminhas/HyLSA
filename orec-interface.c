#include <stdio.h>
#include <stdlib.h>

struct orecs{
  unsigned long address;
  int locked;
  unsigned long owner;
  struct orecs* previous_orec;
  struct orecs* next_orec;};

typedef struct orecs orec;


orec* add_orec(orec* orecs,unsigned long address,int locked, unsigned long owner) {
 	(orecs->next_orec) = malloc(sizeof(orec));
 	orec* next_orec = orecs->next_orec;
 	next_orec->address = address;
 	next_orec->locked = locked;
 	next_orec->owner = owner;
 	next_orec->previous_orec = orecs;
 	return next_orec;
 }

orec* init_orec(unsigned long address,int locked, unsigned long owner){
	orec* new_orec = malloc (sizeof(orec));
	new_orec->address = address;
 	new_orec->locked = locked;
 	new_orec->owner = owner;
 	new_orec->previous_orec = 0;
 	new_orec->next_orec= 0;
 	return new_orec;
}

orec* fetch_orec (unsigned long address, orec* orecs ) { 
	while(orecs != 0){
		if((orecs->address) == address){
			printf("CHEGUEI\n");
			return orecs;}
		orecs = (orecs->previous_orec);
	}
	printf("NULL OREC\n");
	orec* null_orec = init_orec(0,0,0);
	return null_orec;
} 

int main()
{	
	orec* orecs = init_orec(999,0,8);
	orecs = add_orec (orecs,888,0,2);
	orecs = add_orec (orecs,888,0,2);
	printf("%ld \n", fetch_orec(999,orecs)->owner);
    printf("Hello World \n");
    return 0;
}