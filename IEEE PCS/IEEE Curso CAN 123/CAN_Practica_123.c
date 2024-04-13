#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "IEEE_CAN.h"
#include "driverlib/sysctl.h"

uint64_t Rx;

//--------------------------------------------------------------------
//%%%%%%    INICIALIZACI�N DE PUERTOS ASOCIADOS AL CAN0    %%%%%%%%%%%
//                 CAN0Rx: PB4    CAN0Tx: PB5
//--------------------------------------------------------------------
void Config_Puertos(void){                  //(TM4C123GH6PM)
    SYSCTL_RCGCGPIO_R|=0x2;                 //Reloj Puerto B
    while((SYSCTL_PRGPIO_R&0x2)==0){}
    GPIO_PORTB_AFSEL_R=0x30;                //PB4 y PB5 funci�n alterna
    GPIO_PORTB_PCTL_R=0x880000;             //Funci�n CAN a los pines PB4-PB5
    GPIO_PORTB_DEN_R=0x30;                  //Hab funci�n digital PB4 y PB5
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    INICIALIZACI�N CAN0     %%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Config_CAN(void){
    SYSCTL_RCGCCAN_R=0x1;                   //Reloj modulo 0 CAN
    while((SYSCTL_PRCAN_R&0x1)==0){}
                                            //Bit Rate= 1 Mbps      CAN clock=25 [Mhz]
    CAN0_CTL_R=0x41;                        //Deshab. modo prueba, Hab. cambios en la config. y hab. inicializacion
    CAN0_BIT_R=0x4901;                      //TSEG2=0   TSEG1=2    SJW=0    BRP=4
                                            //Lenght Bit time=[TSEG2+TSEG1+3]*tq
                                            //               =[(Phase2-1)+(Prop+Phase1-1)+3]*tq
    CAN0_CTL_R&=~0x41;                      //Hab. cambios en la config. y deshab. inicializacion
    CAN0_CTL_R|=0x2;                        //Hab de interrupci�n en el m�dulo CAN
    NVIC_EN1_R|=((1<<(39-32)) & 0xFFFFFFFF);//(TM4C123GH6PM)
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%    INTERRUPCI�N DEL CAN0    %%%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Inter_CAN0(void){
    uint8_t NoInt;
    NoInt=CAN0_INT_R;                       //Lectura del apuntador de interrupciones
    CAN0_STS_R&=~0x10;                     //Limpieza del bit de recepcion
    Rx=CAN_Rx(NoInt);                   //Recepci�n de datos
}

//------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    PROGRAMA PRINCIPAL    %%%%%%%%%%%%%%%%%%%%
//------------------------------------------------------------------
void main(void){
    Config_Puertos();
    Config_CAN();
    //Localidad 1 Rx con Msk
    CAN_Memoria_Arb(0x111,false,0x4);                       //ID, TxRx, Localidad
    CAN_Memoria_CtrlMsk(0xFFF,3,false,true,false,0x4);      //Mask, DLC, TxIE, RxIE, Remote, Localidad
    //Localidad 2 Tx
    CAN_Memoria_Arb(0x777,true,0x5);                        //ID, TxRx, Localidad
    CAN_Memoria_CtrlMsk(0,3,false,false,false,0x5);         //Mask, DLC, TxIE, RxIE, Remote, Localidad
    CAN_Memoria_Dato(0x123456,0x5);                         //Dato, Localidad
    //Localidad 3 Trama remota Tx
    CAN_Memoria_Arb(0x333,true,0x6);                        //ID, TxRx, Localidad
    CAN_Memoria_CtrlMsk(0xFFF,6,false,false,true,0x6);      //Mask, DLC, TxIE, RxIE, Remote, Localidad
    CAN_Memoria_Dato(0x119999999911,0x6);                   //Dato, Localidad
    while(1){
        SysCtlDelay(59);                //Retraso 11 [us] n=fsys*tdes/3
        CAN_Tx(0x5);
    }
}
