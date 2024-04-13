#ifndef IEEE_SPI_H_
#define IEEE_SPI_H_

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%    COMANDOS PARA SPI MAESTRO    %%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
void SPI_Escribir(int Reg_Add, int Dato);
int SPI_Leer(int Reg_Add);

//--------------------------------------------------------------------
//%%%%%%%%%%%%%%%%    COMANDOS PARA SPI ESCLAVO    %%%%%%%%%%%%%%%%%%%
//--------------------------------------------------------------------
//void I2CS_Direccion(int Slave_Add1, int Slave_Add2);

#endif /* IEEE_SPI_H_ */
