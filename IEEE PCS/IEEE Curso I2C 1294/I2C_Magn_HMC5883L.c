#include<stdbool.h>
#include<stdint.h>
#include<inc/tm4c1294ncpdt.h>

////------------- DIRECCCIONES MAGNETÓMETRO -------------------//
static short MAG_CONFG_REGA=0x00;         //Registro de configuración A
static short MAG_CONFG_REGB=0x01;         //Registro de configuración B
static short MAG_CONFG_MODO=0x02;         //Registro de Modo de operación
static short MAG_X_MSB=0x3;               //8 bits superiores Magnetómetro X
static short MAG_X_LSB=0x4;               //8 bits inferiores Magnetómetro X
static short MAG_Z_MSB=0x5;               //8 bits superiores Magnetómetro Y
static short MAG_Z_LSB=0x6;               //8 bits inferiores Magnetómetro Y
static short MAG_Y_MSB=0x7;               //8 bits superiores Magnetómetro Z
static short MAG_Y_LSB=0x8;               //8 bits inferiores Magnetómetro Z
static short MAG_ADD=0x1E;                //Dirección del Esclavo del Magnetómetro
static short MAG_STAT_REG=0x9;            //8 bits inferiores Gyro Magnetómetro

int Vector[3];

//--------------------------------------------------------------------
//%%%%%%%%   INICIALIZACIÓN DE PUERTOS ASOCIADOS AL I2C0  %%%%%%%%%%%
//                      SCL: PB2    SDA: PB3
//---------------------------------------------------------------------
void Config_puertos(void){
    SYSCTL_RCGCGPIO_R=0x2;            //Reloj Puerto B
    while((SYSCTL_PRGPIO_R&0x2)==0){}

    //PUERTO B  I2C (PB2 SCL y PB3 SDA)
    GPIO_PORTB_AHB_AFSEL_R|=0xC;         //Función alterna a PB2 y PB3
    GPIO_PORTB_AHB_PCTL_R|=0x2200;       //Función I2C a PB2 y PB3
    GPIO_PORTB_AHB_ODR_R|=0x8;           //PB3 como Open Drain, ya que PB2 ya tiene preconfig. eso
    GPIO_PORTB_AHB_DEN_R|=0xC;           //Hab función digital PB2 y PB3

    //SYSTICK
    NVIC_ST_CTRL_R|=0x4;         //Uso del reloj a 16 [Mhz]
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    INICIALIZACIÓN I2C0    %%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Config_I2C0(void){
    //COMUNICACIÓN I2C0 (Magnetómetro)    I2C6    PA6:SCL (Sin OD)    PA7:SDA (Con OD)
    SYSCTL_RCGCI2C_R=0x1;                   //Reloj Módulo I2C0
    while((SYSCTL_PRI2C_R & 0x1)==0){}      //Reloj I2C
    I2C0_MCR_R=0x10;                        //Hab modo Maestro del I2C
    I2C0_MTPR_R=0x1;                        //TPR: Timer Period=2    Fclk=16 [MHz]
                                            //SCL_PRD=2*(1+TPR)*(SCL_LP+SCL_HP)/Fclk;   SCL_LP=6 y SCL_HP=4
                                            //SCL_freq=400 [kHz]
//    I2C0_MIMR_R|=0x1;                         //Hab interrupción cuando se complete una transacción
    NVIC_EN0_R|=(((1<<8)) & 0xFFFFFFFF);
    NVIC_PRI2_R=(NVIC_PRI2_R&0xFFFFFF00)|0x00000000;  //Prioridad 0 (Mayor)
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%    COMANDOS PARA I2C MAESTRO    %%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
unsigned char Reconocimiento(void){
    unsigned char error;
    while(I2C0_MCS_R&0x1){}; //Mientras el controlador esté ocupado
      if(I2C0_MCS_R&0x2==1){ //Verifiación de algún error
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
//%%%%%%%%%%%%%%%%%    INTERRUPCIÓN DEL I2C0    %%%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Inter_I2C0(void){
    I2C0_MICR_R|=0x1;
}


//--------------------------------------------------------------------
//%%%%%%%%%%%%%%    CONFIGURACIÓN DEL ACELERÓMETRO    %%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Magn_Config(void){
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(MAG_ADD, MAG_CONFG_REGA);
    I2C_Escribir_Byte(0x0);             //Tasa de 800 [Hz]
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(MAG_ADD, MAG_CONFG_REGB);
    I2C_Escribir_Byte(0x1);             //Tasa de 800 [Hz]
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(MAG_ADD, MAG_CONFG_MODO);
    I2C_Escribir_Byte(0x2);             //Tasa de 800 [Hz]
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]
}

//--------------------------------------------------------------------
//%%%%%%%%%%%%    LECTURA DE DATOS DEL ACELERÓMETRO    %%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void Magn_LeerDato(void){
    int i;
//        SysTick(96000);                                 //Output Data Rate 167 [Hz]
    SysTick(20000);                                 //Output Data Rate 800 [Hz]
    //Eje X
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(MAG_ADD, MAG_X_LSB);
    I2C_StartR(MAG_ADD);
    Vector[0]=I2C_Leer_Byte();
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(MAG_ADD, MAG_X_MSB);
    I2C_StartR(MAG_ADD);
    Vector[0]|=(I2C_Leer_Byte()<<8);
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    //Eje Y
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(MAG_ADD, MAG_Y_LSB);
    I2C_StartR(MAG_ADD);
    Vector[1]=I2C_Leer_Byte();
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(MAG_ADD, MAG_Y_MSB);
    I2C_StartR(MAG_ADD);
    Vector[1]|=(I2C_Leer_Byte()<<8);
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    //Eje Z
    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(MAG_ADD, MAG_Z_LSB);
    I2C_StartR(MAG_ADD);
    Vector[2]=I2C_Leer_Byte();
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    while(I2C0_MCS_R&0x40){};                       //Mientras el bus no esté en idle
    I2C_StartW(MAG_ADD, MAG_Z_MSB);
    I2C_StartR(MAG_ADD);
    Vector[2]|=(I2C_Leer_Byte()<<8);
    I2C_Stop();
    SysTick(21);                                    //Libertad del bus entre STOP y START 1.3 [us]

    for(i=0;i<3;i++){
        if((Vector[i] & 0x8000)==0x8000){
            Vector[i]=-((Vector[i]^0xFFFF)+1);              //Conversión a Complem 2
        }
    }
}


//------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%    PROGRAMA PRINCIPAL    %%%%%%%%%%%%%%%%%%%%
//------------------------------------------------------------------
void main(void){
//    int Vector[3];
    Config_puertos();
    Config_I2C0();
    Magn_Config();
    while(1){
        Magn_LeerDato();
    }
}
