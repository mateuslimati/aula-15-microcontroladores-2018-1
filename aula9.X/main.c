// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bits (Code protection off)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low Voltage In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code Protection off)
#pragma config WRT = OFF        // FLASH Program Memory Write Enable (Unprotected program memory may not be written to by EECON control)

#define _XTAL_FREQ 16000000

#include <xc.h>
#include <time.h>

char UART_Init(const long int baudrate){

    unsigned int x;
    x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);   //SPBRG for Low Baud Rate
    RCIE = 0;

    //Configurations of TXSTA Register
    SPBRG = x;                                    //Writing Baud Rate Generator Register
    BRGH = 0;                                     //Asynchronous mode: Low speed
    SYNC = 0;                                     //Setting Asynchronous Mode, ie USART
    TXEN = 1;                                     //Transmite Enable 
    TX9 = 0;                                      //Selects 8-bit transmition

    //Configurations of RCSTA Register
    SPEN = 1;                                     //Enables Serial Port
    RX9 = 0;                                      //Selects 8-bit recption
    CREN = 1;                                     //Enables Continuous Reception

    //Configurations of Pins RC6 and RC7 Directions
    TRISC6 = 1;                                   //Def
    TRISC7 = 1;                                   //As Prescribed in Datasheet

    RCIF = 0;
    //RCIE = 1;

    return 1;                                     //Returns 1 to indicate Successful Completion
                                        //Returns 0 to indicate UART initialization failed
}

void UART_Write(char data){
    while(!TRMT);
    TXREG = 'c';
}
void main(){

    UART_Init(9600); 
    
    while(1){
      if(RCIF == 1){
        RCIF = 0;
        if(RCREG != 'a')
        UART_Write(RCREG + 1);
      }  
    }
}