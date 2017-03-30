/*******************************************************************************
   CEFET-MG - Eletr�nica
   Controle de tempo e pot�ncia em um forno el�trico atrav�s de PWM

            
                        MSP430              LCD 16 X 2 8 bits
                  -----------------          ------------
                 |                 |        |            |
                 |                 |        |            |
                 |             P3.7|------->|RS          |
                 |                 |        |            |
                 |             P3.6|------->|E           |
                 |                 |        |            | 
                 |                 |        |            |
                 |                 |        |            |
                 |                 |        |            |
                 |                 |        |            |
                 |                 |        |            |
                 |             P5.0|------->|D0          |
                 |                 |        |            |
                 |             P5.1|------->|D1          |
                 |                 |        |            |
                 |             P5.2|------->|D2          |
                 |                 |        |            |
                 |             P5.3|------->|D3          |
                 |                 |        |            |
                 |             P5.4|------->|D4          |
                 |                 |        |            |
                 |             P5.5|------->|D5          |
                 |                 |        |            |
                 |             P5.6|------->|D6          |
                 |                 |        |            |
                 |             P5.7|------->|D7          |
                 |                 |        |            |
                  -----------------          ------------
      
   Camilla Freitas
   Lorrane Lucille de Souza Cantero
   Lucas de Sousa Arcanjo
   Maria J�lia Assis Sales

   Setembro/2016
   Desenvolvido com o IAR Embedded Workbench Vers�o: 6.40.4

*******************************************************************************/

#include <msp430.h>
#include <stdbool.h>
#include "lcd8bits.h"
#include "matriz4x3.h"

#define POTENCIA        0
#define TEMPO           1
#define ptam            3
#define ttam            4
#define fator           6555

void valida (bool );
void v_invalido (bool );
void pwm_active (void);
int pow (int , int );
int atoi (char *, signed char);

int i = 0, k = 0, pwm_in = 0, tempoint = 0, tcontrol = 0, tcontrol1 = 0;
char percentual[ptam] = "00", tempo[ttam] = "000", tecla = 0, estado = 0;
bool pvalido = false, tvalido = false;

void main (void)
{
  // Desabilita do Watchdog
  WDTCTL = WDTPW | WDTHOLD;                    
  
  // Inicializa��o do LCD
  LCM_init();
  
  // Inicializa��o do Teclado
  Teclado_init();
  
  // Configura��es I/O
  P1DIR |= 0xFF;                        // Define todos os pinos da porta 1 como sa�da
  P1SEL |= BIT2;                        // Define P1.2 como fun��o especial (TA1 - Sa�da PWM)
  
  // Inicializa��o do Timer A
  TACTL = TASSEL_0 | MC_0;
  CCR0 = 512;
  
  // Configura��es gerais de inicializa��o
  PrintStr("Forno - CEFET MG");
  __delay_cycles(100000);
  clear();
  MoveCursor(0, 0);
  PrintStr("Pressione");
  MoveCursor(1,0);
  PrintStr("qualquer tecla");
  
  P1OUT |= BIT1;                        // LED indicador de status ligado
  
  // Standby
  __bis_SR_register(LPM0_bits | GIE);   // Modo LPM0 e Interrup��es habilitadas
}

#pragma vector = PORT2_VECTOR           // Endere�o de interrup��o de P2IN para o MSP430F1611
__interrupt void Teclado_ISR (void)
{
  tecla = cap_tecla();
  
  switch (estado)
  {
  // Estado inicial (PRESSIONE QUALQUER TECLA)
  case 0:
    clear();
    MoveCursor(0, 0);
    PrintStr("Potencia:");
    MoveCursor(1, 0);
    Cursor(TRUE);
    estado++;
    break;
    
  // Informa��o de pot�ncia
  case 1:
    if (tecla == 'E')                   // Enter
    {
      valida(POTENCIA);                 // Valida o valor percentual e converte o vetor para o tipo int
      if (pvalido == true)              // Para um valor percentual entre 10% e 90%
      {
        clear();
        Cursor(FALSE);
        MoveCursor(0, 0);
        PrintStr("Tempo:");
        MoveCursor(1, 0);
        Cursor(TRUE);
        
        estado++;
      } else                            // Para um valor percentual inv�lido
          v_invalido(POTENCIA);
    }
    else if (tecla == 'A')              // Anula
    {
      if (i == 0)
        return;
      else
      {
        Erase();
        i--;
      }
    }
    else                                // Teclas num�ricas
    {
      SendByte(tecla, TRUE);
      percentual[i] = tecla;
      i++;
    }
    break;
    
  // Informa��o de tempo
  case 2:
    i = 0;
    if (tecla == 'E')                   // Enter
    {
      valida(TEMPO);                    // Verifica se o tempo se encontra no intervalo e o converte para o tipo int
      if (tvalido == true)              // Para um valor de tempo entre 10 e 120 minutos
      { 
        clear();
        Cursor(FALSE);
        
        MoveCursor(0, 0);
        PrintStr("Potencia: ");
        PrintStr(percentual);
        PrintStr("%");
        MoveCursor(1, 0);
        PrintStr("Restam: ");
        PrintStr(tempo);
        PrintStr(" min");
        
        //Configurar timer para exibir o tempo atualizado no display
        pwm_active();
        estado++;
      }
      else                            // Para um valor de tempo inv�lido
        v_invalido(TEMPO);
    }
    else if (tecla == 'A')              // Anula
    {
      if (i == 0)
        return;
      else
      {
        Erase();
        i--;
      }
    }
    else                                // Teclas num�ricas
    {
      SendByte(tecla, TRUE);
      tempo[i] = tecla;
      i++;
    }
    break;
    
  // Forno em funcionamento  
  case 3:
    break;
    
  // Aguardando o pressionamento da tecla enter para novo ciclo
  case 4:
    if (tecla == 'E')
      estado = 0;
    else
      return;
    break;
  }
}

#pragma vector = TIMERB0_VECTOR           // Endere�o de interrup��o do Timer B CCR0 para o MSP430F1611
__interrupt void TimerB0_ISR (void)
{
  // Contador para decremento do tempo
  tcontrol++;
  if (tcontrol == 60)
  {
    tcontrol = 0;
    tempoint--;
    
    if (tempoint == 0)                    // Fim do ciclo de opera��o (tempoint = 0)
    {
      TBCCTL1 |= CCIE;                    // Habilita interrup��es do bloco
      estado = 4;                         // Estado de opera��o: Aguardando a tecla enter para iniciar novo ciclo
    }
    else                                  // Ciclo de opera��o em progresso (tempoint > 0)
    {
      MoveCursor(1, 8);                   // Localiza��o do tempo restante no display
      PrintInt(tempoint);
    }
  }
  
  // Rotina para pisca-led
  P1OUT ^= BIT1;
}

#pragma vector = TIMERB1_VECTOR           // Endere�o de interrup��o do Timer B CCR1 e CCR2 para o MSP430F1611
__interrupt void TimerB1_ISR (void)
{
  if (estado == 0)
  {
    // Desabilita interrup��es dos blocos
    TBCCTL1 &= ~CCIE;
    TBCCTL1 &= ~CCIE;
    estado = 0;                          // Estado: FINALIZADO
    P1OUT |= BIT1;
    return;
  }
  
  // Altera o estado do LED
  P1OUT ^= BIT1;
}

void valida (bool param)
{
  if (param == POTENCIA)
  {
    pwm_in = atoi(percentual, ptam);
  
    if ((pwm_in >= 10) && (pwm_in <= 90))
      pvalido = true;
    else
      pvalido = false;
  } else if (param == TEMPO)
  {
    tempoint = atoi(tempo, ttam);
  
    if ((tempoint >= 120) && (tempoint <= 10))
      tvalido = true;
    else
      tvalido = false;
  }
}

void v_invalido (bool param)
{
  clear();
  Cursor(FALSE);
  MoveCursor(0, 0);
  PrintStr("Valor invalido!");
  __delay_cycles(100000);         //VERIFICAR DELAY
  
  clear();
  MoveCursor(0, 0);
  if (param == POTENCIA)
    PrintStr("Potencia:");
  else
    PrintStr("Tempo:");
  MoveCursor(1, 0);
  Cursor(TRUE);
}

void pwm_active (void)
{
  // Habilita contagem do Timer A no modo up to CCR0 para PWM (P1.2)
  TACCR1 = pwm_in * fator;              // Define Ton
  TACTL |= MC_1 | TBCLR;
                                        // Modo de contagem: up to TACCR0
                                        // Limpa contador do timer
  
  CCTL1 = OUTMOD_7;                     // Modo de opera��o PWM (Reset/Set)
  
  // Habilita contagem do Timer B para interrup��es regulares de ajuste do display
  // Bloco TBCCR0
  TBCCTL0 |= CCIE;                     // Habilita interrup��es do bloco
  TBCCR0 = 32768;                       // Interrup��es a cada 1,0s
  
  // Bloco TBCCR1
  //TBCCTL1 |= CCIE;                      // Habilita interrup��es do bloco
  TBCCR1 = 16384;                       // Interrup��es a cada 0,5s
  
  // Bloco TBCCR2
  TBCCTL2 |= CCIE;
  TBCCR2 = 8192;
  
  // Bloco do Timer B
  TBCTL = TBSSEL_1 | MC_1 | TBCLR;
                                        // Fonte de clock: ACLK (32768 Hz)
                                        // Modo de contagem: up to TBCCR0
                                        // Limpa contador do timer
}

int atoi (char *vet, signed char length)
{
  int output = 0, x = 0;
  for (length -= 1; length >= 0; length--)
  {
    x = vet[length] - 0x30;
    output += x * pow(10, 1 - length);
  }
  
  return output;
}

int pow (int x, int n)
{
  int i = 0 , pot = 1;
  while (i < n) 
  {
    pot = pot * x; 
    i = i + 1;
  }
  
  return pot;
}