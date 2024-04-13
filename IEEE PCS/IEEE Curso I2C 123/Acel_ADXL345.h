/*
 * Acel_ADXL345.h
 *
 *  Created on: 04/12/2019
 *      Author: danie
 */

#ifndef ACEL_ADXL345_H_
#define ACEL_ADXL345_H_
#include"IEEE_I2C.h"
#include"inc/tm4c123gh6pm.h"

//------------- DIRECCCIONES ACELER�METRO -------------------//
static short ACE_X_LSB=0x32;              //8 bits inferiores Aceler�metro Eje X
static short ACE_X_MSB=0x33;              //8 bits inferiores Aceler�metro Eje X
static short ACE_Y_LSB=0x34;              //8 bits inferiores Aceler�metro Eje Y
static short ACE_Y_MSB=0x35;              //8 bits superiores Aceler�metro Eje Y
static short ACE_Z_LSB=0x36;              //8 bits inferiores Aceler�metro Eje Z
static short ACE_Z_MSB=0x37;              //8 bits superiores Aceler�metro Eje Z
static short ACE_ADD0=0x53;                //Direcci�n del Esclavo del Aceler�metro
static short ACE_ADD1=0x1D;                //Direcci�n del Esclavo del Aceler�metro
static short ACE_POWER_CTL=0x2D;            //Tasa de datos y control de modo de encendido
static short ACE_FIFO_CTL=0x38;            //Tasa de datos y control de modo de encendido
static short ACE_BW_RATE=0x2C;            //Tasa de datos y control de modo de encendido
static short ACE_OFFX=0x1E;               //Offset Eje X
static short ACE_OFFY=0x1F;               //Offset Eje Y
static short ACE_OFFZ=0x20;               //Offset Eje Z

//-------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    SYSTICK    %%%%%%%%%%%%%%%%%%%%
//-------------------------------------------------------
void SysTick(Retraso){
    NVIC_ST_RELOAD_R=Retraso;
    NVIC_ST_CURRENT_R=0x0;
    NVIC_ST_CTRL_R|=0x1;
    while((NVIC_ST_CTRL_R&0x10000)==0){}
    NVIC_ST_CTRL_R&=~0x1;
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%    CONFIGURACI�N DEL ACELER�METRO    %%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Acel_Config(int ADD){
    //Apagado del sensor
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no est� en idle
    I2C_StartW(ADD, ACE_POWER_CTL);
    I2C_Escribir_Byte(0x0);
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
    //Configuraci�n del Baud Rate a 400 [Hz]
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no est� en idle
    I2C_StartW(ADD, ACE_BW_RATE);
    I2C_Escribir_Byte(0xD);             //Tasa de 800 [Hz]
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
    //Uso de FIFO en modo Stream
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no est� en idle
    I2C_StartW(ADD, ACE_FIFO_CTL);
    I2C_Escribir_Byte(0x80);             //Tasa de 800 [Hz]
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
    //Modo de medici�n encendido
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no est� en idle
    I2C_StartW(ADD, ACE_POWER_CTL);
    I2C_Escribir_Byte(0x8);             //Hab de modo encendido
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
}

#endif /* ACEL_ADXL345_H_ */
