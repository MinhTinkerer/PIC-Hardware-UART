#include <xc.h>
#include "stddef.h"
#include "serial.h"
#pragma config WDT=OFF, OSC=HS, LVP=OFF, IESO=OFF, FCMEN=ON, XINST=OFF

// High Priority interupt for serial reception
void interrupt serial_RX_ISR()
{
	// Serial reception handler
    if (PIR1bits.RC1IF == 1)
    {
        unsigned char c;
        c = RCREG;
        store_char(RCREG);
        // this variable stores the next location where data is about to be stored
	    int i = (unsigned int)(rx_buffer.head + 1) % SERIAL_BUFFER_SIZE;
  	    // if we should be storing the received character into the location
  	    // just before the tail (meaning that the head would advance to the
  	    // current location of the tail), we're about to overflow the buffer
  	    // and so we don't write the character or advance the head.
  	    if (i != rx_buffer.tail) {
    	    rx_buffer.buffer[rx_buffer.head] = c;
    	    rx_buffer.head = i;
  	    }
  	}
}

// Low Priority interrupt for serial transmission
void interrupt low_priority serial_TX_ISR()
{
	// Serial Transmission handler
    if (PIR1bits.TXIF == 1 && TXSTAbits.TXEN == 1)
	{
        if (tx_buffer.head == tx_buffer.tail)
        {       
            tx_di();
        }    
        else
	    {
            unsigned char c = tx_buffer.buffer[tx_buffer.tail];
            tx_buffer.tail = (tx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
            TXREG = c;
        }
    }
}

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
