#include <stdio.h>
#include <stdlib.h>

struct orecs{
  unsigned long address;
  int locked;
  unsigned long owner;
  struct orecs* previous_orec;
  struct orecs* next_orec;};

typedef struct orecs orec;

struct o_sets{
	long address;
	struct o_sets* previous_o_set;
	struct o_sets* next_o_set;	
};

typedef struct o_sets o_set;

o_set* init_o_set(long address){
	o_set* new_o_set;
	new_o_set = malloc (sizeof(o_set));
	new_o_set->address = address;
	new_o_set->previous_o_set=0;
	new_o_set->next_o_set=0;
	return new_o_set;
}

o_set* add_to_o_set(o_set* o_set_param, long address){
	o_set* next_o_set;
	(o_set_param->next_o_set) = malloc(sizeof(o_set));
	next_o_set = (o_set_param->next_o_set);
 	next_o_set->address = address;
 	(next_o_set->previous_o_set) = o_set_param;
 	return next_o_set;
}


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
{	//orec* orecs = init_orec(999,0,8);
	//orecs = add_orec (orecs,888,0,2);
	//orecs = add_orec (orecs,888,0,2);
	printf("TESTE1\n");
	o_set* o  = init_o_set(555);
	printf("TESTE2\n");
	if((o->previous_o_set == 0) && (o->next_o_set == 0))
		printf("O-SET vazio\n");
	printf("%ld \n", (o->previous_o_set)->address);
    printf("Hello World \n");
    return 0;
}