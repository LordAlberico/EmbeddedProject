#include "project.h"

int main()
{
    setup();
	for(;;){
	    mainLoop();
    }
    close();
    return 0;
}
