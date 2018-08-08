/* 
 * File:   mainApp.h
 * Author: Golikov D.S.
 *
 * Created on 23 04 2018 ?., 14:35
 */

#ifndef MAINAPP_H
#define	MAINAPP_H
/************************************************************************/
/*---------------------DEFINE-------------------------------------------*/
/************************************************************************/
/*util*/
#define true 1
#define false 0
/*чтение и запись ключа ds1990*/
#define READ 0
#define WRITE 1
#define FIRST 1
#define SECOND 2
#define END 127
#define TM_0 13
#define TM_1 3
#define TM_2 4
#define TM_3 5
#define TM_4 6
#define TM_5 7
#define TM_6 8
#define TM_7 9
#define READ_RS_485  0x06
#define WRITE_RS_485 0x86

/*******************************/
#define _XTAL_FREQ 16000000
/*******************************/

/*relay пины управления реле*/
#define RELAY_ERROR PORTBbits.RB5
#define RELAY_FIRE1 PORTCbits.RC3
#define RELAY_FIRE2 PORTCbits.RC2
#define RELAY_OTV_GO PORTCbits.RC1
#define RELAY_FIRE_FS PORTCbits.RC0

/*IO управление приемопередатчиком и связью DMCU*/
#define DE PORTCbits.RC4                //управление чтение/запись микросхемы max485
#define MCU PORTBbits.RB4               //сигнал на главный процессор что DMCU считало состояние и готово к передаче данных

/*пины для контроля состояние переферии ППКПУ*/
#define DOOR PORTBbits.RB1              //дверь ППКПУ
#define POWER_220 PORTAbits.RA4         //наличие напряжения
#define BATTERY PORTBbits.RB3           //АКБ
#define SHUNT PORTCbits.RC5             //шунт АКБ
//#define DELAY_KEY 150
/*********************************************/
#define LOWER_VOLTAGE_ON_POWER_LINE 202     //предельное напряжение на линии питаня (1, 2, 3, 4) 10 В 
#define LOWER_VOLTAGE_ON_BATTERY 0x01B0     //предельное напряжение на АКБ 21 В


unsigned char dmcuWoR;                  //направление передачи (чтение/запись) линии связи 485
unsigned char indexOutByte;             //индекс отправного байта на линию 485
unsigned char mSec;                     //милисикунда 
unsigned char sec;                      //секунда
unsigned char shunt;                    //проверка АКБ с шунтом, делаем рас в минуту (выстовляем в таймере, сбрасываем установкой шунта)
unsigned char akb;                    //проверка АКБ с шунтом, делаем рас в минуту (выстовляем в таймере, сбрасываем установкой шунта)

union dmcu6_relay_register_t {
		struct {			
            // Управление реле выходных сигналов:
			unsigned char error:1;          // ... "Неисправность",
			unsigned char vnim:1;           // ... "Внимание",
			unsigned char pozar:1;          // ... "Пожар",
			unsigned char aspt:1;           // ... "АСПТ сработало",
			unsigned char pozps:1;          // ... "Пожар ПС".
			unsigned char reserved:3;       // Зарезервировано для будущего использования.
		};
		unsigned char as_byte; 
	} relay;   
    
   
struct dmcu6_stat_t {		
		union {
			struct {
				unsigned char er_pl:4;      // Ошибка линий питания 1...4.
				unsigned char er_dc:1;      // Неисправность питания (напряжение ниже нормы).
				unsigned char er_bat:1;     // Неисправность АКБ.
				unsigned char er_ac:1;      // Неисправность (отсутствие напряжения) сети.
				unsigned char reserved7:1;  // Зарезервировано для будущего использования.
			};
			unsigned char as_byte; 
		} stat0;
		union {
			struct {
				unsigned char tm:1;         // Считан код ключа. Устанавливается на время касания ключом порта.
				unsigned char od:1;         // Дверь ППКУП открыта.
				unsigned char reserved:6;   // Зарезервировано для будущего использования.
			};
			unsigned char as_byte; 
		} stat1;
	}state;

    
void setRX();                           //устанавливает линию связи 485 на прием данных
void setTX();                           //устанавливает линию связи 485 на отправку данных
unsigned char getPowerLine();           //получить состояние питания сети 220 В неисправна/исправна - true/false
unsigned char getDc();                  //получить состояние питания 24 в неисправна/исправна - true/false
unsigned char getBat();                 //получить состояние батареи неисправна/исправна - true/false
unsigned short adc(unsigned char an);   //получить значение ацп: принимает номер канала ацп, возвращает значение ацп
#endif	/* MAINAPP_H */

