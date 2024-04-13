#include<stdbool.h>
#include<stdint.h>
#include"inc/tm4c1294ncpdt.h"
#include"IEEE_SPI.h"
#include"Acel_ADXL345.h"
#include"driverlib/sysctl.h"

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%    CONFIGURACIÓN DEL ACELERÓMETRO    %%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
//void Acel_Config_I2C(int ADD){
//    //Apagado del sensor
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ADD, ACE_POWER_CTL);
//    I2C_Escribir_Byte(0x0);
//    I2C_Stop();
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
//    //Configuración del Baud Rate a 400 [Hz]
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ADD, ACE_BW_RATE);
//    I2C_Escribir_Byte(0xD);             //Tasa de 800 [Hz]
//    I2C_Stop();
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
//    //Uso de FIFO en modo Stream
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ADD, ACE_FIFO_CTL);
//    I2C_Escribir_Byte(0x80);             //Tasa de 800 [Hz]
//    I2C_Stop();
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
//    //Uso de +-2g 10 bits
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ADD, ACE_DATA_FORMAT);
//    I2C_Escribir_Byte(0x0);             //Tasa de 800 [Hz]
//    I2C_Stop();
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
//    //Modo de medición encendido
//    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
//    I2C_StartW(ADD, ACE_POWER_CTL);
//    I2C_Escribir_Byte(0x8);             //Hab de modo encendido
//    I2C_Stop();
//    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
//}

void Acel_Config_SPI(void){
    //Apagado del sensor
    SPI_Escribir(ACE_POWER_CTL,0x0);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
    //Uso de +-2g 10 bits
    SPI_Escribir(ACE_DATA_FORMAT,0x0);
    SysCtlDelay(54);
    //Configuración del Baud Rate a 3200 [Hz]
//    SPI_Escribir(ACE_BW_RATE,0xD);                  //Tasa de 800 [Hz]
    SPI_Escribir(ACE_BW_RATE,0xF);                  //Tasa de 3200 [Hz]
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
    //Uso de FIFO en modo Stream
    SPI_Escribir(ACE_FIFO_CTL,0x80);
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]                                //Libertad del bus entre STOP y START 1.3 [us]
    //Modo de medición encendido
    SPI_Escribir(ACE_POWER_CTL,0x8); //Hab de modo encendido
    SysCtlDelay(54);                                    //Libertad del bus entre STOP y START 1.3 [us]
}



