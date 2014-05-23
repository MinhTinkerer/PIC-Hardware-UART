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

#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_STRING_SIZE	16
#define SERIAL_BUFFER_SIZE  64

// Circular buffer implimentation
typedef struct circular_buffer
{
	unsigned char buffer[SERIAL_BUFFER_SIZE];
	volatile unsigned int head;
	volatile unsigned int tail;
}circular_buffer;

circular_buffer rx_buffer = { {0}, 0, 0 };
circular_buffer tx_buffer = { {0}, 0, 0 };


// Functions to enabble/disable RX/TX interrupts
inline void rx_ei();
inline void rx_di();
inline void tx_ei();
inline void tx_di();

// Function to store the char into the RX buffer.
inline void store_char(unsigned char c);

// Initialize serial port settings and enable interrupts
void serialBegin(int baudRate);

// Writes a character to the serial buffer
void serialWrite(unsigned char data);

// Gives the total number of bytes in the RX buffer ready to be read
unsigned char serialAvailable(void);

// Shutdown serial RX and RX by disabling the interrupts.
void serialEnd();

// Flush the data in the RX buffer 
void serialFlush();

// Take a look at the top elemet in the RX buffer
int serialPeek(void);

// Reads and returns a char from the serial RX buffer
char serialRead();

// Places a srting in the serial TX buffer!
void serialPrint(unsigned char* buf);

// Prints a string and appends a '\n' at last
void serialPrintln(unsigned char* buf);

// Reads a srting of length len from the seral RX buffer
void serialReadln(unsigned char* string, unsigned char len);

// Search a source strig and look for a target string
unsigned char StrLookFor( unsigned char* search_str, unsigned char* target_str);

#endif
