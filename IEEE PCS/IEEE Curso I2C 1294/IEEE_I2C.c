#include<stdbool.h>
#include<stdint.h>
#include"inc/tm4c1294ncpdt.h"
#include"driverlib/sysctl.h"
#include"IEEE_I2C.h"

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%    COMANDOS PARA I2C MAESTRO    %%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
unsigned char Reconocimiento(void){
    unsigned char error;
    SysCtlDelay(54);
    while(I2C0_MCS_R&0x1){}; //Mientras el controlador esté ocupado BUSBSY
      if(I2C0_MCS_R&0x2==1){ //Verifiación de algún error ERROR
          error=1;
          return error;
      }
      return 0;
}
void I2C_StartW(int Slave_Add, int Dato){   //RW: (0) Transmitir    (1) Recibir
    I2C0_MSA_R=(Slave_Add << 1);             //Carga dirección de escalvo y escritura o lectura
    I2C0_MDR_R=Dato;                            //Envío de un Byte
    I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_START);     //Escritura de START
    Reconocimiento();
}
void I2C_StartR(int Slave_Add){   //RW: (0) Transmitir    (1) Recibir
    I2C0_MSA_R=(Slave_Add << 1)|0x1;             //Carga dirección de escalvo y escritura o lectura
    I2C0_MCS_R=(I2C_MCS_RUN|I2C_MCS_START);     //Escritura de START
    Reconocimiento();
}
void I2C_Escribir_Byte(int Byte){
    I2C0_MDR_R=Byte;                            //Envío de un Byte
    I2C0_MCS_R=(I2C_MCS_RUN);                   //Transmisión de datos
    Reconocimiento();
}
int I2C_Leer_Byte(void){
    int Byte;
//    I2C0_MCS_R=(I2C_MCS_ACK|I2C_MCS_RUN); //Transmisión de datos
//    Reconocimiento();
    Byte=I2C0_MDR_R;                            //Lectura de dato del bus
    return Byte;
}
void I2C_Stop(void){
    I2C0_MCS_R=I2C_MCS_STOP;      //Escritura de STOP
    Reconocimiento();
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%    COMANDOS PARA I2C ESCLAVO    %%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void I2CS_Direccion(int Slave_Add1, int Slave_Add2){   //RW: (0) Transmitir    (1) Recibir
    I2C0_SOAR_R=Slave_Add1&0x7F;             //Carga dirección de escalvo y escritura o lectura
    if(Slave_Add2){                                 //Si Slave_Add2 != 0
        I2C0_SOAR2_R=Slave_Add2&0x7F;
        I2C0_SOAR2_R|=0x80;
    }
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%%%    I2C HIGH SPEED    %%%%%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void I2C_HighSpeed(void){
    I2C0_MTPR_R|=0x80;    //TPR: Timer Period=1    Fclk=40 [MHz]
                          //SCL_PRD=2*(1+TPR)*(SCL_LP+SCL_HP)/Fclk;   SCL_LP=2 y SCL_HP=1
                          //SCL_freq=3.33 [MHz]
    I2C0_MSA_R=0x08;             //Carga dirección de escalvo y escritura o lectura
    I2C0_MCS_R=(I2C_MCS_HS|I2C_MCS_RUN|I2C_MCS_START);     //Hab del modo High Speed
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%%%    I2C HIGH SPEED    %%%%%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void I2C_GlitchSupp(void){
    //  (TM4C123GH6PM)
//    I2C0_MCR_R|=0x40;              //Habiltación de Supresión de GLitch GFE
//    I2C0_MCR2_R|=0x40;             //Ancho del pulso de 4 ciclos de reloj GFPW
    //  (TM4C1294NCPDT)
    I2C0_MTPR_R|=0x40000;          //Ancho del pulso de 4 ciclos de reloj PULSEL
}

