/* 
 * File:   mainApp.c
 * Author: Golikov D.S golikov.d.s@yandex.ru
 *
 * Created on 23 03 2018 ?., 14:35
 */

#include <stdio.h>
#include <stdlib.h>
#include <htc.h>

#include "mainApp.h"
#include "ds1990.h"


//#pragma config CONFIG1H = 0x2
__CONFIG(1, OSC_HS & FCMEN_OFF & IESO_OFF);
//#pragma config CONFIG2L = 0x1F
__CONFIG(2, PWRT_OFF & BOREN_SBORDIS & BORV_3);
//#pragma config CONFIG2H = 0xE
__CONFIG(3, WDT_OFF & WDTPS_128);
//#pragma config CONFIG3H = 0x83
__CONFIG(4, CCP2MX_PORTC & PBADEN_ON & LPT1OSC_OFF & MCLRE_ON);
//#pragma config CONFIG4L = 0x81
__CONFIG(5, STVREN_ON & LVP_OFF & XINST_OFF);
//#pragma config CONFIG5L = 0xF
__CONFIG(6, CP0_OFF & CP1_OFF & CP2_OFF & CP3_OFF);
//#pragma config CONFIG5H = 0xC0
__CONFIG(7, CPB_OFF & CPD_OFF);
//#pragma config CONFIG6L = 0xF
__CONFIG(8, WRT0_OFF & WRT1_OFF & WRT2_OFF & WRT3_OFF);
//#pragma config CONFIG6H = 0xE0
__CONFIG(9, WRTC_OFF & WRTB_OFF & WRTD_OFF);
//#pragma config CONFIG7L = 0xF
__CONFIG(10, EBTR0_OFF & EBTR1_OFF & EBTR2_OFF & EBTR3_OFF);
//#pragma config CONFIG7H = 0x40
__CONFIG(11, EBTRB_OFF);


void init (){

    T0CON = 0b00000111;
    INTCON = 0b11000000;
    INTCON2 = 0b10000000;
    INTCON3 = 0b00000000;
         
    PIE1 = 0b00000000;
         
    ADCON0 = 0b00000001;
    ADCON1 = 0b00010101;
    ADCON2 = 0b10000010;
    
    T0CON = 0b10000000;
    T1CON = 0b00100001;
    T2CON = 0b00100000;
    TMR0IE = true;
       
    TXSTA = 0b01100100; 
    RCSTA = 0b11000000;		
    SPBRG = 8;
         
    CMCON = 0b00000111;
    FSR1H = 0b00000000;
    FSR2H = 0b00000000;
         
    TRISA = 0b00111111;
    TRISB = 0b11001111;
    TRISC = 0b11000000;
        
    LATA = 0b00000000;
    LATB = 0b00000000;
    LATC = 0b00000000;

    PORTA = 0b00000000;
    PORTB = 0b00000000;
    PORTC = 0b00000000;
         
    dmcuWoR = READ;
    indexOutByte = FIRST;
    MCU = true;
    setRX();
}

int main(int argc, char** argv)
{
    init();
    while(true){    
        if(!MCU){
            GIE = false;
            //set state
            state.stat0.er_pl = getPowerLine();
            state.stat0.er_dc = getDc();
            state.stat0.er_bat = getBat();                  //если нет питания 24 в то на шунт не разрежаем
            state.stat0.er_ac = POWER_220;             
            state.stat1.od = DOOR;            
            //TM
            if(state.stat1.tm == false){
                if(isPressedDs1990()) writeReadDs1990();
            }
            GIE = true;
        }  
    }
    return (EXIT_SUCCESS);
}

unsigned char getPowerLine(){
    unsigned char result = 0x00;
    for(int i = 0; i < 4; i++){    
        if(adc(i == 3 ? 4: i) < LOWER_VOLTAGE_ON_POWER_LINE)
            result |= (0x01 << i);        
    }
    return result;
}

unsigned char getDc(){
    if(adc(8) < LOWER_VOLTAGE_ON_POWER_LINE)    
        return true;
    return false;
}

unsigned char getBat(){
    if(shunt && !state.stat0.er_dc){
        if(adc(9) < LOWER_VOLTAGE_ON_BATTERY)    
            return true;
        SHUNT = true;
        __delay_us(500);
        if(adc(9) < LOWER_VOLTAGE_ON_BATTERY) {   
            SHUNT = false;
            shunt = false;
            return true;
        }
        SHUNT = false;
        shunt = false;   
        return false;
    }  
}

unsigned short adc(unsigned char an){
    ADCON0 = 0b00000001 | (an << 2);
    GO = true;
    while (GO); 
    return ((ADRESH << 8) | ADRESL);
}

void setRX(){    
    MCU = false;
    DE = false;
    TXSTAbits.TXEN = false;
    PIE1bits.TXIE = false;
    RCSTA1bits.ADDEN = false;
    RCSTA1bits.CREN = true;
    PIE1bits.RCIE = true;
}

void setTX(){    
    MCU = true;//go to set state
    DE = true;
    TXSTAbits.TXEN = true;
    PIE1bits.TXIE = true;     
    RCSTAbits.CREN = false;
    PIE1bits.RCIE = false;
}
