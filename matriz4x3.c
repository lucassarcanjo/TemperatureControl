/*******************************************************************************
   CEFET-MG - Eletr�nica
   matriz4x3 - Biblioteca de fun��es para MSP430 com um teclado 4 x 3 em matriz.
            
                        MSP430               Teclado 4 x 3
                  -----------------          ------------
                 |                 |        |            |
                 |                 |        |            |
                 |             P2.0|<-------|C0          |
                 |                 |        |            |
                 |             P2.1|<-------|C1          |
                 |                 |        |            |
                 |             P2.2|<-------|C2          |
                 |                 |        |            |
                 |                 |        |            |
                 |             P3.0|------->|L0          |
                 |                 |        |            |
                 |             P3.1|------->|L1          |
                 |                 |        |            |
                 |             P3.2|------->|L2          |
                 |                 |        |            |
                 |             P3.3|------->|L3          |
                 |                 |        |            |
                 |                 |        |            |
                  -----------------          ------------
      
   Lucas de Sousa Arcanjo
   Agosto/2016
   Desenvolvido com o IAR Embedded Workbench Vers�o: 6.40.4
   Vers�o para o kit did�tico do Laborat�rio de Sistemas Microprocessados MSP430F1611.

   Fun��es para acionamento do Teclado 4 x 3 em matriz.

*******************************************************************************/

#include "matriz4x3.h"

/********************************* FUN��ES DO TECLADO *****************************************/ 

/* *********************************************************************************************
   Fun��o: Teclado_init (Inicializa��o do teclado)
   Fun��o de defini��o das portas e interrup��es do teclado em matriz
*/
void Teclado_init (void)         
{
  COL_DIR &= 0xF8;
  LIN_DIR |= 0xCF;
  LIN_OUT |= 0x0F;
  P2IE |= 0x07;
  P2IES &= ~0x07;
}

/* *********************************************************************************************
   Fun��o: cap_tecla (Capturar tecla)
   Fun��o para captura da tecla pressionada retornando o c�digo ASCII da mesma
*/
char cap_tecla (void)
{
  const char teclado[12]="369E2580147A";        // Tabela das teclas da matriz 4 x 3
  char t_tecla = 0, t_count = 0, col, linha;
  
  __delay_cycles(10000);                         // Antibounce

  col = P2IFG & 0x07;
  
  switch (col)
  {
  case 0x01:
    t_count = t_count + 0;
    break;
    
  case 0x02:
    t_count = t_count + 4;
    break;
    
  case 0x04:
    t_count = t_count + 8;
    break;
  }
  
  LIN_OUT = 0x01;
  linha = COL_IN & 0x07;
  if (linha != 0)
    t_count = t_count + 0;

  LIN_OUT = 0x02;
  linha = COL_IN & 0x07;
  if (linha != 0)
    t_count = t_count + 1;
 
  LIN_OUT = 0x04;
  linha = COL_IN & 0x07;
  if (linha != 0)
    t_count = t_count + 2;
 
  LIN_OUT = 0x08;
  linha = COL_IN & 0x07;
  if (linha != 0)
    t_count = t_count + 3;
 
  t_tecla = teclado[t_count];
  
  LIN_OUT |= 0x0F;
  
  while (COL_IN & 0x07);                  //Aguarda a tecla ser solta
  __delay_cycles(10000);
  P2IFG = 0;
  
  return t_tecla;
}