
#include <htc.h>
#include "mainApp.h"
#include "ds1990.h"


void interrupt uart(){
    unsigned char nData;
    unsigned char indexInputData;
    //таймер
    if(TMR0IF){
        TMR0IF = 0;
        TMR0 = 0xB1F0;
        mSec++;

        if(mSec == 100){ 
            sec++;
            if(sec == 60){
                shunt = true;
                sec = 0;
            }
            mSec = 0;
        }
    }

    //читаем входящие данные
    if(RCIF){ 
        if((RCSTAbits.OERR | RCSTAbits.FERR) > 0){ //if error
            RCSTAbits.CREN = 0;
            WREG = RCREG;
            WREG = RCREG;
            RCSTAbits.CREN = 1;
            return;
	}       
        nData = RCREG;
        
        if(nData == READ_RS_485){            
            dmcuWoR = READ;
            indexInputData = 0;
            return;
        }else if(nData == WRITE_RS_485){
            dmcuWoR = WRITE; 
            return;
        }
        
        if(dmcuWoR == READ){
            if(FIRST == indexInputData++){
                //переноси данные в структуру реле и устанавливаем сами реле
                relay.as_byte = nData;      
                RELAY_ERROR = relay.error;   
                RELAY_FIRE1 = relay.vnim;
                RELAY_FIRE2 = relay.pozar;
                RELAY_OTV_GO = relay.aspt;
                RELAY_FIRE_FS = relay.pozps;
            }
            if(indexInputData == SECOND){
                indexInputData = 0;
            }
            return;
        }else if(dmcuWoR == WRITE){
            
            if(nData == FIRST){
                //пришел запрос от главного процессора на чтение состояния переферии
                setTX();
                indexOutByte = FIRST;
                return;
            }else if(nData == SECOND){
                //пришел запрос от главного процессора на чтение кода ключа
                setTX();
                indexOutByte = TM_0;
                return;
            }
        }
    }    
    
    if(TXIF){  
        switch(indexOutByte){
            /*отправк двух байт состояния переферии*/
            case FIRST: 
                indexOutByte = SECOND;
                TXREG = state.stat0.as_byte;
                break;
            case SECOND:
                indexOutByte = END;
                TXREG = state.stat1.as_byte;
                break;
            /*отправка кода ключа - 8 байт*/
            case TM_0:
                indexOutByte = TM_1;
                TXREG = tmValue.data[0];//start byte
                break;
            case TM_1:
                indexOutByte = TM_2;
                TXREG = tmValue.data[1];
                break;
            case TM_2:
                indexOutByte = TM_3;
                TXREG = tmValue.data[2];
                break;
            case TM_3:
                indexOutByte = TM_4;
                TXREG = tmValue.data[3];
                break;
            case TM_4:
                indexOutByte = TM_5;
                TXREG = tmValue.data[4];
                break;
            case TM_5:
                indexOutByte = TM_6;
                TXREG = tmValue.data[5];
                break;
            case TM_6:
                indexOutByte = TM_7;
                TXREG = tmValue.data[6];
                break;
            case TM_7:
                indexOutByte = END;
                TXREG = tmValue.data[7];
                state.stat1.tm = false;
                break;
            case END:
                indexOutByte = FIRST;
                setRX();
                break;
            default: 
                indexOutByte = FIRST;
                setRX();
                break;
        }
        __delay_us (750);  //it`s need задержка для принятия главным процессором данных       
    }    
}
