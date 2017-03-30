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

#ifndef TECL4x3_
#define TECL4x3_

/******************************************************************************/
// Bibliotecas a serem usadas
#include <msp430.h>

// Frequ�ncia do MCLK
#define freq 750000

// Tempos de delay
#define _5ms 5 * freq

// Portas de dado e controle do Teclado 4x3
#define COL_DIR P2DIR
#define COL_IN  P2IN
#define LIN_DIR P3DIR
#define LIN_OUT P3OUT

// Defini��o (Prot�tipo) das fun��es
void Teclado_init (void);        // Configura��es iniciais do Teclado
char cap_tecla (void);           // Capturar tecla pressionada

#endif /* TECL4x3_*/