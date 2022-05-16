
#include <stdlib.h>
#include "../structs.h"


//tu jest glowna petla stolu
void* TurnBoardOn(void*arg){
        Board* table=((Argument*)arg)->board;
        free((Argument*)arg);//zwalnianie pamieci

	return 0;
}      
