//#include <msp430.h>
#include <msp430x471x7.h>
#include "com.h"

#define DEBUG 1

void COM_init() {
  P3SEL |= BIT4 | BIT5;
  UCA0CTL1 = UCSSEL_2;

  // Setup COM clockspeed for 16mhz
  UCA0BR0 = 0x82;
  UCA0BR1 = 0x6;
  UCA0MCTL = UCBRS_6;

  UCA0CTL1 &= ~UCSWRST;
  IE2 |= UCA0RXIE;
  __enable_interrupt();
}

void TXString(char* string){
  if( DEBUG == 0 ) return;
  int i = 0;
  while(string[i] != 0x00) {
    while(!(IFG2&UCA0TXIFG));
    UCA0TXBUF = string[i];
    i++;
  }
}

void itoa(long unsigned int value, char* result, int base) {
  // check that the base if valid
  if (base < 2 || base > 36) { *result = '\0';}

  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
  } while ( value );

  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';
  while(ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr--= *ptr1;
    *ptr1++ = tmp_char;
  }
}

void TXInt(int val){
  char buf[10];
  itoa(val, buf, 10);
  TXString("int: ");
  TXString(buf);
  TXString("\r\n");
}
