/*
 * File:   main.c
 * Author: Mateus
 *
 * Created on 21 de Junho de 2018, 02:24
 */

#include <xc.h>
#include <time.h>
#define _XTAL_FREQ 16000000

#pragma config FOSC = HS
#pragma config PWRT = ON
#pragma config PBADEN = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF

#define TMR2PRESCALE 16

unsigned fre;

unsigned PWM_Max_Duty(){
  return(_XTAL_FREQ/(fre*TMR2PRESCALE));
}

//1. Set the PWM period by writing to the PR2 register.

void PWM_init(unsigned freq){
    PR2 = (_XTAL_FREQ/(freq*4*TMR2PRESCALE)) - 1;         
    fre = freq;
}

//2. Set the PWM duty cicle by writing to the CCPR1L register and CCP1CON<5:4> bit.

void PWM1_duty(unsigned int duty){
  duty = ((double)duty/100)*1022;  
  if(duty<=1022){
    duty = ((double)duty/1023)*PWM_Max_Duty();
    DC1B1 = duty & 2;
    DC1B0 = duty & 1;
    CCPR1L = duty>>2;
  }
}

void PWM2_duty(unsigned int duty){
  duty = ((double)duty/100)*1022;  
  if(duty<=1022){
    duty = ((double)duty/1023)*PWM_Max_Duty();
    DC2B1 = duty & 2;
    DC2B0 = duty & 1;
    CCPR2L = duty>>2;
  }
}

//3. Make the CCP1 pin an output by clearing the TRISC<2> bit.
//4. Set the TMR2 prescale value and enable Timer2 by writing to T2CON.
//5. Configure the CCP1 module for PWM operation.

void PWM_start(){

    TRISC2 = 0; 
    TRISC1 = 0; 
    
    CCP1M3 = 1;             
    CCP1M2 = 1;
    CCP2M3 = 1;             
    CCP2M2 = 1;             
                 
    
    if(TMR2PRESCALE == 1){
        T2CKPS0 = 0;
        T2CKPS1 = 0;
    }
    if(TMR2PRESCALE == 4){
        T2CKPS0 = 1;
        T2CKPS1 = 0;
    }
    else if(TMR2PRESCALE == 16){
        T2CKPS0 = 1;
        T2CKPS1 = 1;
    }

    TMR2ON = 1; 
}

void curvaDir(){
    PWM1_duty(50);
    PWM2_duty(75);    
}


void curvaEsq(){
    PWM1_duty(75);
    PWM2_duty(50);    
}

void Frente_Re(){
    PWM1_duty(75);
    PWM2_duty(75);    
}

void Frente(){

    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 1;
    
}

void Re(){

    PORTDbits.RD0 = 1;
    PORTDbits.RD1 = 0;
    
}

void Parado(){
    
    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 0;
    
}


void init_USART(unsigned long baudrate){
   
    //Configura o clock de comunica��o
    SPBRG = (_XTAL_FREQ/(baudrate*64)) - 1;
    
    //Configura��es do Registrador TXSTA
    BRGH    = 0;                //Configura o modo Ass�ncrono de baixa velocidade
    SYNC    = 0;                //Configura o modo Assincrono
    TXEN    = 1;                //Habilita a transmiss�o 
    TX9     = 0;                //Seleciona o modo de transmiss�o de 8 bits
    
    //Configura��es do Registrador RCSTA
    CREN    = 1;                //Habilita a recep��o cont�nua
    RX9     = 0;                //Seleciona o modo de recep��o de 8 bits
    SPEN    = 1;                //Habilita a porta serial (Chaveia os pinos RC7/RX e RC6/TX como pinos seriais)
        
    //Configura��es das dire��es dos pinos RC7 e RC6
    TRISC6  = 0;                 
    TRISC7  = 1;                 

    RCIE    = 1;                 //Habilita as interrup��es por recep��o USART 
    RCIF    = 0;                 //Desabilita a flag de interrup��o       
    GIE     = 1;                 //Habilita as interrup��es globais                         
    PEIE    = 1;                 //Habilita as interrup��es por perif�ricos
    
}

void sendMsg(char msg){
    
    while(!TRMT);
    TXREG = msg;
}

void interrupt routine(){
    
    RCIE    = 0;
    GIE     = 0;
    PEIE    = 0;
    
    if(RCIF){
        if(RCREG == 'w'){
            sendMsg(RCREG);
            Frente();
            Frente_Re();
            __delay_ms(500);
            Parado();
        }
        else if(RCREG == 's'){
            sendMsg(RCREG);
            Re();
            Frente_Re();
            __delay_ms(500);
            Parado();
        }
        else if(RCREG == 'a'){
            sendMsg(RCREG);
            Frente();
            curvaDir();
            __delay_ms(1000);
            Parado();
        }
        else if(RCREG == 'd'){
            sendMsg(RCREG);
            Frente();
            curvaEsq();
            __delay_ms(1000);
            Parado();
        }
        
    }
    
    RCIE    = 1;
    GIE     = 1;
    PEIE    = 1;
    
}

void main (void){

   //periodo = (PR2 + 1) x ciclo de m�quina x prescaler do Timer2
    //periodo = 256 x 25ns x 16 = 1,024ms
    //frequencia = 1/periodo = 50 Hz
    
    TRISD0 = 0;
    TRISD1 = 0;
    
    Parado();
    PWM_init(976);
    PWM_start(); 
    init_USART(9600);
    while(1);
    
}
 