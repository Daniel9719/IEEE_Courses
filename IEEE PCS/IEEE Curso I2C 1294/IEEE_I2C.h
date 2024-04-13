#ifndef IEEE_I2C_H_
#define IEEE_I2C_H_

#include<stdbool.h>
#include<stdint.h>
#include"inc/tm4c1294ncpdt.h"
#include"driverlib/sysctl.h"

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%    COMANDOS PARA I2C MAESTRO    %%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
unsigned char Reconocimiento(void);
void I2C_StartW(int Slave_Add, int Dato);
void I2C_StartR(int Slave_Add);
void I2C_Escribir_Byte(int Byte);
int I2C_Leer_Byte(void);
void I2C_Stop(void);

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%    COMANDOS PARA I2C ESCLAVO    %%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void I2CS_Direccion(int Slave_Add1, int Slave_Add2);

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%%%    I2C HIGH SPEED    %%%%%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void I2C_HighSpeed(void);

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%%%%%%%    I2C HIGH SPEED    %%%%%%%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void I2C_GlitchSupp(void);

#endif /* IEEE_I2C_H_ */
