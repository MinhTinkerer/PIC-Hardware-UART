/*Copyright (c) 2014 EmbedJournal.

Author: Siddharth Chandrasekaran
Website: http://embedjournal.com/

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#include <xc.h>
#include "serial.h"
#include "stddef.h"

// Functions to enabble/disable RX/TX interrupts
inline void rx_ei(){
	PIE1bits.RCIE=1;
}	
inline void rx_di(){
	PIE1bits.RCIE=0;
}
inline void tx_ei(){
	PIE1bits.TXIE=1;
}
inline void tx_di(){
	PIE1bits.TXIE=0;
}	

// Initialize serial port settings and enable interrupts
void serialBegin(int baudRate)
{
    TRISCbits.TRISC6 = 0;       // configure the pins as output
    TRISCbits.TRISC7 = 1;       // configure the pins as input
    
    switch (baudRate)
    {
        case 1800:  TXSTAbits.BRGH = 0;   // High Baud rate Select bit 
                    SPBRG = 173;          // 1800 Baud rate
                    break;
        case 2400:  TXSTAbits.BRGH = 0;   // High Baud rate Select bit 
                    SPBRG = 64;           // 2400 Baud rate
                    break;
        case 4800:  TXSTAbits.BRGH = 0;   // High Baud rate Select bit 
                    SPBRG = 129;          // 4800 Baud rate
                    break;
        case 7200:  TXSTAbits.BRGH = 1;   // High Baud rate Select bit 
                    SPBRG = 173;          // 7200 Baud rate
                    break;
        case 9600:  TXSTAbits.BRGH = 1;   // High Baud rate Select bit 
                    SPBRG = 129;          // 9600 Baud rate
                    break;
        case 14400: TXSTAbits.BRGH = 1;   // High Baud rate Select bit 
                    SPBRG = 86;           // 14400 Baud rate
                    break;
        case 19200: TXSTAbits.BRGH = 1;   // High Baud rate Select bit 
                    SPBRG = 64;           // 19200 Baud rate
                    break; 
        default:    TXSTAbits.BRGH = 0;   // High Baud rate Select bit 
                    SPBRG = 129;   		  // 9600 Baud rate
    }
    RCSTAbits.CREN = 1;         // RX enable
    TXSTAbits.TXEN = 1;			// TX enable
    RCSTAbits.SPEN = 1;         // Serial Port Enable
    RCONbits.IPEN = 1;			// Enable interrupt priority
    IPR1bits.TXIP = 0;			// TX is set to low priority
    IPR1bits.RCIP = 1;			// TX is set to low priority
    INTCONbits.PEIE = 1;		// Enable Pheripheral interrupts
    rx_ei();				// UART RX interrupt
    ei();				// Global interrupt enable
}

// Writes a character to the serial buffer
void serialWrite(unsigned char data)
{
    int next = (tx_buffer.head + 1) % SERIAL_BUFFER_SIZE;
    // If the output buffer is full, there's nothing to be done 
    // so wait for the interrupt handler to empty it a bit
    while (next == tx_buffer.tail);
    tx_buffer.buffer[tx_buffer.head] = data;
    tx_buffer.head = next;
    // Enable transmit interrupt to send the data in the buffer
    tx_ei();
}

// Gives the total number of bytes in the RX buffer ready to be read
unsigned char serialAvailable(void)
{
	// retuns the total bytes of data available on the RX buffer
	return (unsigned char)(SERIAL_BUFFER_SIZE + rx_buffer.head - rx_buffer.tail) % SERIAL_BUFFER_SIZE;
}

// Shutdown serial RX and RX by disabling the interrupts.
void serialEnd()
{
	// wait for transmission of outgoing data
	while (tx_buffer.head != tx_buffer.tail);

	// clear interrupts corresponding to serial port
    rx_di();
    tx_di();
	// clear any received data
	rx_buffer.head = rx_buffer.tail;
}

// Flush the data in the RX buffer 
void serialFlush()
{
	// wait for transmission of outgoing data
	while (tx_buffer.head != tx_buffer.tail);
	// clear any received data
	rx_buffer.head = rx_buffer.tail;
}

// Take a look at the top elemet in the RX buffer
int serialPeek(void)
{
  if (rx_buffer.head == rx_buffer.tail) {
    return -1;	// quit with an error
  } else {
    // Just return data without removing it from the buffer.
    return rx_buffer.buffer[rx_buffer.tail];
  }
}

// Reads and returns a char from the serial RX buffer
char serialRead()
{
	// if the head isn't ahead of the tail, we don't have any characters
    if (rx_buffer.head == rx_buffer.tail) {
    return -1;	// quit with an error
    } else {
        char c = rx_buffer.buffer[rx_buffer.tail];
        rx_buffer.tail = (unsigned int)(rx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
        return c;
    }
}

// Places a srting in the serial TX buffer!
void serialPrint(unsigned char* buf)
{
    unsigned char space = 0, count = 0;
    unsigned char* temp;
	temp = buf;		// make a local copy to calculate the length.
	while (*(temp++)>0) count++;	// get the lenght of buf
	do{
        space = (unsigned char)(SERIAL_BUFFER_SIZE + tx_buffer.head - tx_buffer.tail) % SERIAL_BUFFER_SIZE;
    }while (space >= count);	// keep doing till the interrupt handler frees up the req space in the buffer
    count = 0;		// reuse variable to conserve memory
    while (*(buf+count)>0){
        int i = (tx_buffer.head + 1) % SERIAL_BUFFER_SIZE;
        tx_buffer.buffer[tx_buffer.head] = *(buf+count);
        tx_buffer.head = i;
        count++;
    }    
    tx_ei();
}

// Prints a string and appends a '\n' at last
void serialPrintln(unsigned char* buf)
{
	serialPrint(buf);
	serialWrite('\n');
	serialWrite('\r');
}

// Reads a srting of length len from the seral RX buffer
void serialReadln(unsigned char* string, unsigned char len)
{
	if (serialAvailable() >= len)
	{
    	while (serialPeek()== '\n'|| serialPeek()== '\r')
    	{
        	serialRead();
        }       
		for (int idx=0; idx<len; idx++)
		{
			*string = serialRead();
			string++;
		}
	}
}

// Search a source strig and look for a target string
unsigned char StrLookFor( unsigned char* search_str, unsigned char* target_str)
{
	unsigned char* temp1;
	unsigned char* temp2;
	unsigned char ctr = 0, len = 0;
	temp1 = target_str;
	temp2 = search_str;
	while (*temp2++ > 0)	len++;
	
	while (*temp1 >0){
		if (*temp1 == *temp2){
			while (*temp1 == *temp2){	// This loop will run at leaset once!
				ctr++;
				temp1++;
				temp2++;
			}
			if ( ctr == len)
				return TRUE;
			temp2 = search_str;
			ctr = 0;
		}
		temp1++;
	}
	return FALSE;
}
