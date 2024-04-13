#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include "IEEE_CAN.h"
#include "driverlib/sysctl.h"

uint64_t Rx[2];

//--------------------------------------------------------------------
//%%%%%%    INICIALIZACIÓN DE PUERTOS ASOCIADOS AL CAN0    %%%%%%%%%%%
//                 CAN0Rx: PA0    CAN0Tx: PA1
//--------------------------------------------------------------------
void Config_Puertos(void){                      //(TM4C1294NCPDT)
    SYSCTL_RCGCGPIO_R|=0x1;                     //Reloj Puerto A
    while((SYSCTL_PRGPIO_R&0x1)==0){}
    GPIO_PORTA_AHB_AFSEL_R=0x3;                 //PA0 y PA1 función alterna
    GPIO_PORTA_AHB_PCTL_R=0x77;                 //Función CAN a los pines PA0-PA1
    GPIO_PORTA_AHB_DEN_R=0x3;                   //Hab función digital PA0 y PA1
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    INICIALIZACIÓN CAN0     %%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Config_CAN(void){
    SYSCTL_RCGCCAN_R=0x1;                       //Reloj modulo 0 CAN
    while((SYSCTL_PRCAN_R&0x1)==0){}
                                                //Bit Rate= 1 Mbps      CAN clock=16 [Mhz]
    CAN0_CTL_R=0x41;                            //Deshab. modo prueba, Hab. cambios en la config. y hab. inicializacion
    CAN0_BIT_R=0x4901;                          //TSEG2=4   TSEG1=9    SJW=0    BRP=0
                                                //Lenght Bit time=[TSEG2+TSEG1+3]*tq
                                                //               =[(Phase2-1)+(Prop+Phase1-1)+3]*tq
    CAN0_CTL_R&=~0x41;                          //Hab. cambios en la config. y deshab. inicializacion
    CAN0_CTL_R|=0x2;                            //Hab de interrupción en el módulo CAN
    NVIC_EN1_R|=((1<<(38-32)) & 0xFFFFFFFF);    //(TM4C1294NCPDT)
}

void CAN_Error(void){
    static int ent=0;
    if(CAN0_STS_R&0x80){
        if(ent){
            NVIC_APINT_R|=0x4;                      //Reinicio de todo el sistema
        }else{
            CAN0_CTL_R=0x41;                        //Hab. cambios en la config. y hab. inicializacion
            CAN0_CTL_R|=0x80;                       //Hab. modo prueba
            CAN0_TST_R|=0x4;                        //Hab. Modo silencio
            CAN0_CTL_R&=~0x41;                      //Hab. cambios en la config. y deshab. inicializacion
            SysCtlDelay(333333);
            CAN0_CTL_R=0x41;                        //Hab. cambios en la config. y hab. inicializacion
            CAN0_TST_R&=~0x4;                       //Deshab. Modo silencio
            CAN0_CTL_R&=~0x41;                      //Hab. cambios en la config. y deshab. inicializacion
            ent++;
        }
    }
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%    INTERRUPCIÓN DEL CAN0    %%%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Inter_CAN0(void){
    uint8_t NoInt;
    NoInt=CAN0_INT_R;                           //Lectura del apuntador de interrupciones
    CAN0_STS_R&=~0x10;                          //Limpieza del bit de recepcion
    if(NoInt==0x1){
        Rx[0]=CAN_Rx(NoInt);                //Recepción de datos
    }
    if(NoInt==0x3){
        Rx[1]=CAN_Rx(NoInt);                //Recepción de datos
    }
    //    CAN_Error();
}

//------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    PROGRAMA PRINCIPAL    %%%%%%%%%%%%%%%%%%%%
//------------------------------------------------------------------
void main(void){
    Config_Puertos();
    Config_CAN();
    //Localidad 1 Rx con Msk
    CAN_Memoria_Arb(0x777,false,0x1);                   //ID, TxRx, Localidad
    CAN_Memoria_CtrlMsk(0xFFF,3,false,true,false,0x1);  //Mask, DLC, TxIE, RxIE, Remote, Localidad
    //Localidad 2 Tx
    CAN_Memoria_Arb(0x111,true,0x2);                    //ID, TxRx, Localidad
    CAN_Memoria_CtrlMsk(0,3,false,false,false,0x2);     //Mask, DLC, TxIE, RxIE, Remote, Localidad
    CAN_Memoria_Dato(0x789012,0x2);                     //Dato, Localidad
    //Localidad 3 Trama remota Rx
    CAN_Memoria_Arb(0x333,false,0x3);                   //ID, TxRx, Localidad
    CAN_Memoria_CtrlMsk(0xFFF,6,false,true,false,0x3);  //Mask, DLC, TxIE, RxIE, Remote, Localidad
    while(1){
        SysCtlDelay(333);                //Retraso 11 [us] n=fsys*tdes/3
        CAN_Tx(0x2);
        SysCtlDelay(333);                //Retraso 11 [us] n=fsys*tdes/3
        CAN_Tx(0x3);
    }
}
