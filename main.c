/*
//mig-0 change:
//disk_write (BYTE pdrv,const BYTE* buff, DWORD sector, UINT count); -> BYTE* buff



//mmc.h 
  // write a 512 Byte big block beginning at the (aligned) address
  //char mmcWriteBlock (const unsigned long address, const unsigned long count, unsigned char *pBuffer)
  //-> char mmcWriteBlock (const unsigned long address, const unsigned long count, const unsigned char *pBuffer);
//hal_SPI.h
  unsigned char spiSendFrame(unsigned char* pBuffer, unsigned int size);
  ->unsigned char spiSendFrame( const unsigned char* pBuffer, unsigned int size);
*/
//#include <hal_hardware_board.h>
//#include <MMC.h>

//#include <msp430.h>
#include <msp430x471x7.h>
#include "ff.h"
#include "com.h"

unsigned int bufferOffset = 0;
unsigned int buffer[510];                     // DCT buffer to fill SD card

int iflag = 0;

FIL fil;
FRESULT rc;
FATFS fatfs;                              // File system object
WORD bw;
char* buffs = "abc";


/*
void sampleADC ( void ) {
  ADC10CTL0 &= ~ENC;                        // Disable ADC Conversion
  while (ADC10CTL1 & BUSY);                 // Wait if ADC10 core is active
  ADC10CTL0 |= ENC | ADC10SC;               // Enable + start conversion
}
*/

void die ( FRESULT rc ) {
  int i;
  //TXString("Died...");
  //TXInt(rc);

  for(;;){
    while(i < 0xFFFF) i++;                 // TODO: Use TIMER to blink light
    P3OUT ^= BIT6;                         // Toggle red light
  }
}

void FAT_init(){
  // Mount the volume
  //TXString("Mount Volume\r\n");
  rc = f_mount(&fatfs, "", 1);
  if (rc) die(rc);
  /*
  TXString("After mount\r\n");

  TXString("Open DATA.BIN\r\n");
  */
  rc = f_open(&fil, "mig.TXT", FA_OPEN_ALWAYS | FA_READ);
  if (rc) die(rc);

  rc = f_lseek(&fil, f_size(&fil));
  if (rc) die(rc);

  //rc = f_sync(&fil);   // Flush cached info to disk
  //if (rc) die(rc);
}

void clock_init(void)
{
  WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
  //dung xung clock mac dinh.
  volatile unsigned int i;
  FLL_CTL0 |= XCAP11PF;                     // Configure load caps

  // Wait for xtal to stabilize
  do
  {
  IFG1 &= ~OFIFG;                           // Clear OSCFault flag
  for (i = 0x47FF; i > 0; i--);             // Time for flag to set
  }
  while ((IFG1 & OFIFG));
  
  
  /*
  //cau hinh UART
  P2SEL |= BIT4+BIT5;                       // P2.4,5 = USCI_A0 RXD/TXD
  UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
  UCA0BR0 = 0x03;                           // 32k/9600 - 3.41
  UCA0BR1 = 0x00;                           //
  UCA0MCTL = 0x06;                          // Modulation
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
  */
  
}

void FAT_close() {
  f_close(&fil);
  //TXString("After close...\r\n");
}

void write_ADC_to_SD() {
  //TXString("call f_write\r\n");

  FAT_init();
  rc = f_write(&fil, buffer, 510, &bw);
  if (rc) die(rc);
  FAT_close();

  //rc = f_sync(&fil);   // Flush cached info to disk
  ////if (rc) die(rc);
}

/*
void handleInterrupt( void ) {

  if((iflag & BIT0) != 0){           // Timer A interrupt
    iflag &= ~BIT0; // clear BIT0 flag
    P3OUT ^= BIT6; // Toggle GRN LED
    sampleADC();
    //TXString("Timer A Interrupt\r\n");
  }

  if((iflag & BIT1) != 0){           // ADC interrupt
    ADC10CTL0 &= ~ENC;                        // Disable ADC Conversion

    iflag &= ~BIT1; // clear BIT1 flag
    

    if(bufferOffset >= 510){                  // If our buffer is full...
      write_ADC_to_SD();                      // Write buffer to SD card    
      bufferOffset = 0;                       // Reset memory offset
    }
    
    ADC10SA = (int)buffer + bufferOffset;     // Write data buffer start address (re-enables DTC)
    bufferOffset += (sizeof(int) * 3);              // Move forward 3 'int's in memory

    P3OUT ^= BIT7; // Toggle RED LED
    //TXString("ADC interrupt\r\n");
  }
}
*/

void main( void )
{
  clock_init();
  //COM_init();
  
  
  //FAT_init();
  rc = f_mount(&fatfs, "", 1);
  __delay_cycles(10000);
  if (rc) die(rc);
  
  rc = f_open(&fil, "mig.TXT", FA_OPEN_ALWAYS | FA_WRITE);
  __delay_cycles(10000);
  if (rc) die(rc);
  
  rc = f_write(&fil, buffs, 3, &bw);
   __delay_cycles(10000);
  if (rc) die(rc);
  FAT_close();
  
  
}
