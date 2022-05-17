#include <stdlib.h>
#include "table.h"
using namespace std;

//tu jest glowna petla stolu
DWORD WINAPI TurnBoardOn(void* arg) {
        Board* table=((Argument*)arg)->board;
        free((Argument*)arg);//zwalnianie pamieci                
	return 0;
}      
