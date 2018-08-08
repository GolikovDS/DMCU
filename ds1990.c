#include "ds1990.h"
#include "mainApp.h"
#include <htc.h>


unsigned char isPressedDs1990(){
    resetWire();
    if(ONE_WIRE_PIN){
        __delay_us (70);
        if(!ONE_WIRE_PIN){          //ключ на линии
            __delay_us (220);            
            if(ONE_WIRE_PIN){       //проверяем наличие ключа на порте
                return true;
            }         
        }
    }    
}

unsigned char writeReadDs1990(){
    outByteWire(0x33);                                  //команда на чтение ROM
    if((tmValue.data[0] = readByteWire()) != 0x01){     //проверка стартового байта - код устройства (для ds1990 - 0x01)
        return false;
    }
    for(int i = 1; i < 8; i++){                         //читаем данные с ключа
        tmValue.data[i] = readByteWire();                      
    }
    if(getCRC8(tmValue.data)){                          //проверка контрольной суммы
        state.stat1.tm = true;
        return true;    
    }
        return false;
}

void resetWire(){  
    ONE_WIRE_PIN = false;
    TRISBbits.RB0 = false;
    ONE_WIRE_PIN = false;
    __delay_us (500);
    TRISBbits.RB0 = true;
    __delay_us (8);
}

void outByteWire(unsigned char data){
  for (int i = 0; i < 8; i++){
    DQ_OUT;
    C_DQ;
    if (data & 0x01){
      DQ_IN;
    }else{
      DQ_OUT;
      C_DQ;
    }
    __delay_us(50);
    data >> = 0x01;
    DQ_IN;
  }
}

unsigned char readByteWire(){
  unsigned char t;
  for(int i = 0; i < 8; i++){
     DQ_OUT;
     C_DQ;
     __delay_us(1);
    t >>= 0x01;
    DQ_IN;
    __delay_us(10);
    ONE_WIRE_PIN = ONE_WIRE_PIN;
    if(ONE_WIRE_PIN){
      t |= 0x80;
    }
    __delay_us(100);
  }
  return t;
}

/*---------------------------------------------------------------*/
/*---------------------CRC---------------------------------------*/
/*---------------------------------------------------------------*/
unsigned char getCRC8(unsigned char* data){
    char CRC = 0;
    for(char i = 0; i < 7; i++){
        CRC = CRCTAB[CRC ^ data[i]];;
    }
    if(data[7] == CRC)
        return true;
    return false;
}