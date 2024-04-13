#include<stdbool.h>
#include<stdint.h>
#include"inc/tm4c1294ncpdt.h"

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%    COMANDOS PARA SPI MAESTRO    %%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void SPI_Escribir(int Reg_Add, int Dato){
    while ((SSI0_SR_R & SSI_SR_BSY) != 0);    //Mientras el controlador est� ocupado BSY
    SSI0_CR1_R&=~0x100;                       //Escritura de Direcci�n
    SSI0_DR_R=(Reg_Add<<8)|Dato;            //Transmisi�n de Direcci�n y Datos
}

int SPI_Leer(int Reg_Add){
    int Byte;
    SSI0_CR1_R|=0x100;                        //Lectura de Direcci�n
    while ((SSI0_SR_R & SSI_SR_BSY) != 0);    //Mientras el controlador est� ocupado BSY
    SSI0_DR_R=(Reg_Add<<8)|0x8000;                   //Transmisi�n de Direcci�n y Datos
    while ((SSI0_SR_R & SSI_SR_BSY) != 0);    //Mientras el controlador est� ocupado BSY
    Byte=SSI0_DR_R&0xFF;                      //Lectura de dato del bus
    return Byte;
}

