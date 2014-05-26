#include <xc.h>
#include "stddef.h"
#include "serial.h"
#pragma config WDT=OFF, OSC=HS, LVP=OFF, IESO=OFF, FCMEN=ON, XINST=OFF

void main()
{	
	ADCON1 = 0x0F;
	serialBegin(9600);
	//serialPrintln("This is a test sting");
	
	while(1){
    	if(serialAvailable() > 0)
    	{
    	    test = serialRead();
    	    serialWrite(test);
    	}
   }   		
}	
