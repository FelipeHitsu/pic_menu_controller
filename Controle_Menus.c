/*--------------------------------------------------------------------------------------
Controle de Potencia
Felipe Silva
Maio / 2015
Controle_C.c
/*------------------------------------------------------------------------------------*/
//Configuração
#include <p18f4520.h>
#include <delays.h>

#pragma config OSC=INTIO67, WDT=OFF, PBADEN=OFF, MCLRE=OFF, PWRT=ON, LVP=OFF
/*------------------------------------------------------------------------------------*/
//Definições de Constantes
#define		RS		PORTDbits.RD0
#define		_E		PORTDbits.RD1
#define		LCD		PORTC
#define		S1		PORTBbits.RB4
#define		S2		PORTBbits.RB5
#define		S3		PORTBbits.RB6
#define		Triac		PORTAbits.RA7
/*------------------------------------------------------------------------------------*/
//Protótipo das Funções
void Config_Ports(void);
void Config_Int(void);
void Int_Alta(void);
void Int_Baixa(void);
void Pulse_I(char);
void Pulse_D(char);
void Cursor(char);
void Clear_LCD(void);
void Pos_Cur(char);
void Config_LCD(void);
void Envia_Const(rom char *);
void Conv_Bin_Asc(char,char *);
void Envia_Var(char *);
void Teclado(void);
/*------------------------------------------------------------------------------------*/
//Declaração das Variáveis Globais
char POT = 50;
char LIGA = 0;
char Pot_Asc[4];

#pragma romdata Tabela_Pot
const rom unsigned char Tabela_Pot[101]={
   57342,58170,58427,58610,58759,58886,58998,59099,59192,59279,
   59360,59437,59510,59580,59647,59711,59774,59834,59892,59950,
   60005,60060,60113,60165,60216,60266,60316,60364,60412,60460,
   60506,60552,60598,60643,60688,60732,60776,60820,60863,60906,
   60949,60992,61034,61077,61119,61161,61203,61244,61286,61328,
   61369,61411,61453,61494,61536,61578,61620,61662,61705,61747,
   61790,61832,61875,61919,61962,62006,62051,62096,62141,62186,
   62232,62279,62326,62374,62423,62473,62523,62574,62626,62679,
   62733,62789,62846,62905,62965,63027,63092,63159,63229,63301,
   63379,63460,63546,63639,63741,63853,63979,64128,64311,64568,
   65397};
#pragma romdata



/*------------------------------------------------------------------------------------*/
//Interrupção de Alta Prioridade
#pragma code isr_alta=0x0008
void ISR_Alta_Prioridade(void)
{
   _asm
      GOTO Int_Alta
   _endasm
}
#pragma code
/*------------------------------------------------------------------------------------*/
//Interrupção de Baixa Prioridade
#pragma code isr_baixa=0x0018
void ISR_Baixa_Prioridade(void)
{
   _asm
      GOTO Int_Baixa
   _endasm	
}
#pragma code
/*------------------------------------------------------------------------------------*/
#pragma code MAIN
void main(void)
{	
   Config_Ports();
   Config_Int();
   Config_LCD();
   while(1)
   {
      
      
   }
}
#pragma code
/*------------------------------------------------------------------------------------*/
#pragma interrupt Int_Alta
void Int_Alta(void)
{
   if((INTCONbits.INT0IF == 1)&&(INTCONbits.INT0IE == 1))
   {
      T0CONbits.TMR0ON = 1;
      INTCONbits.INT0IF = 0;
   }
   if((INTCONbits.TMR0IF == 1)&&(INTCONbits.TMR0IE == 1))
   {
      Triac = 1;
      Delay10TCYx(10);
      Triac = 0;
      T0CONbits.TMR0ON = 0;
      TMR0L = (Tabela_Pot[POT]);
      TMR0H = (Tabela_Pot[POT])>>8;
      INTCONbits.TMR0IF = 0;
   }
   
}
/*------------------------------------------------------------------------------------*/
#pragma interruptlow Int_Baixa
void Int_Baixa(void)
{
   if((INTCONbits.RBIF == 1)&&(INTCONbits.RBIE == 1))
   {
      Teclado();
      INTCONbits.RBIF = 0;
   }
   
}
/*------------------------------------------------------------------------------------*/
#pragma code CONFIGURA

void Pulse_I(char T)
{
   RS = 0;
   _E = 1;
   _E = 0;
   if(T == 1)
      Delay10TCYx(4);
   else
      Delay100TCYx(150);
}
void Pulse_D(char T)
{
   RS = 1;
   _E = 1;
   _E = 0;
   if(T == 1)
      Delay10TCYx(4);
   else
      Delay100TCYx(150);
}

void Cursor(char C)
{
   if(C == 'L')
   {
      LCD = 0x0E;
      Pulse_I(1);
      return;
   }
   if(C == 'D')
   {
      LCD = 0x0C;
      Pulse_I(1);
      return;
   }
   if(C == 'P')
   {
      LCD = 0x0D;
      Pulse_I(1);
   }
}
void Clear_LCD(void)
{
   LCD = 0x01;
   Pulse_I(2);
}
void Pos_Cur(char Pos)
{
  if(Pos<16)
  {
     LCD=0x80+Pos;
     Pulse_I(1);
     return;
  }
  else
  {
     LCD=0xC0+Pos-16;
     Pulse_I(1);
  }
}
void Config_LCD(void)
{
   LCD = 0x38;
   Pulse_I(2);
   Pulse_I(2);
   LCD = 0x06;
   Pulse_I(1);
   Cursor('D');
   Clear_LCD();
   
}
void Envia_Const(rom char *str)
{
   while(*str)
   {
      LCD = *str;
      Pulse_D(1);
      str++;
   }
}
void Conv_Bin_Asc(char T,char *Ts)
{
   Ts[0] =(T/100)+0x30;
   Ts[1] =(T%100/10)+0x30;
   Ts[2] =(T%100%10)+0x30;
}
void Envia_Var(char *str)
{
   while(*str)
   {
      LCD = *str;
     Pulse_D(1);
     str++;
   }
} 



void Teclado(void)
{
   if(S1 == 0)
   {
      if(LIGA == 0)
      {
	 LIGA = 1;
	 Pos_Cur(4);
	 Envia_Const("Potencia");
	 Pos_Cur(21);
	 Conv_Bin_Asc(POT,Pot_Asc);
	 Envia_Var(Pot_Asc);
	 Envia_Const("%");
	 INTCONbits.INT0IE = 1;
      }
      else
      {
	 LIGA = 0;
	 INTCONbits.INT0IE = 0;
	 Clear_LCD();
      }
   }
   if(S2 == 0)
   {
      if((S2 == 0)&&(LIGA == 1))
      {
	 if(++POT>99)
	    POT = 99;
	 Pos_Cur(21);
	 Conv_Bin_Asc(POT,Pot_Asc);
	 Envia_Var(Pot_Asc);
      }
   }
      if((S3 == 0)&&(LIGA == 1))
      {
	 if(--POT<1)
	    POT = 1;
	 Pos_Cur(21);
	 Conv_Bin_Asc(POT,Pot_Asc);
	 Envia_Var(Pot_Asc);
      }
   
}

void Config_Ports(void)
{
   OSCCONbits.IRCF2=1;		//000-31KHz, 001-125KHz, 010-250KHz, 011-500KHz
   OSCCONbits.IRCF1=1;		//100-1MHz, 101-2MHz, 110-4MHz, 111-8MHz
   OSCCONbits.IRCF0=0;
   INTCON2bits.RBPU=0;		//1-Pull-Up's desligados, 0-Pull-Up's ligados
   ADCON1bits.PCFG3=0;		//0111-AN8 a AN12 Digitais, AN0 a AN7 Analógicos
   ADCON1bits.PCFG2=1;		//com PBADEN=OFF.
   ADCON1bits.PCFG1=1;		//se PBADEN=ON - AN0 a AN12 Analógicos
   ADCON1bits.PCFG0=1;
   LATA=0x00;					//Todas os bits do PORTA em 0
   TRISA=0b00000000;			//Todas os bits do PORTA como saídas
   LATB=0x00;					//Todas os bits do PORTB em 0
   TRISB=0b01110001;			//Todas os bits do PORTB como saídas
   LATC=0x00;					//Todas os bits do PORTC em 0
   TRISC=0b10000000;			//Todas os bits do PORTC como saídas
   LATD=0x00;					//Todas os bits do PORTD em 0
   TRISD=0b00000000;			//Todas os bits do PORTD como saídas
   LATE=0x00;					//Todas os bits do PORTE em 0
   TRISE=0b0000;				//RE0 a RE2 como saídas e RE3 como entrada
}
/*------------------------------------------------------------------------------------*/
void Config_Int(void)
{
   RCONbits.IPEN=1;				//;Habilita modo de Dupla Prioridade de Interrupção				
   INTCONbits.GIEL=1;				//;Habilita Interrupções Globais - GIE/GIEH
   INTCONbits.GIEH=1;				//;Habilita Interrupção dos Periféricas - PEIE/GIEL
//-------------------------------
//Timer 0
   INTCONbits.TMR0IE=1;			//;Habilita Interrupção do Timer 0
   //INTCONbits.TMR0IF=1;			//;Liga Flag de estouro de contagem do Timer 0
   INTCON2bits.TMR0IP=1;		//;Habilita Interrupção para Baixa Prioridade
   T0CONbits.TMR0ON=0;			//;Desliga Timer
   T0CONbits.T08BIT=0;			//;Habilita Modo de 16 Bit's
   T0CONbits.T0CS=0;			//;Seleciona Clock Interno (Fosc/4). Está via RA4/T0CKI
   T0CONbits.T0SE=1;			//;Seleciona Borda de Subida para o Clock Externo
   T0CONbits.PSA=1;			//;Habilita prescaler do Timer
   T0CONbits.T0PS2=1;			//;Fator de Divisão do Prescaler:
   T0CONbits.T0PS1=1;				//;111-256, 110-128, 101-64, 100-32
   T0CONbits.T0PS0=1;				//;011-16, 010-8, 001-4, 000-2

//-------------------------------
   INTCONbits.INT0IE=0;
   INTCON2bits.INTEDG0=0;
   INTCONbits.INT0IF=0;
//-------------------------------
   INTCON2bits.RBIP=0;
   INTCONbits.RBIE=1;
   INTCONbits.RBIF=0;
//-------------------------------
   IPR1bits.ADIP=1;
   PIE1bits.ADIE=0;
   PIR1bits.ADIF=0;
   ADCON0bits.ADON=1;
   ADCON0bits.GO=1;
   ADCON0bits.CHS3=0;
   ADCON0bits.CHS2=0;
   ADCON0bits.CHS1=0;
   ADCON0bits.CHS0=1;
   ADCON1bits.VCFG1=0;
   ADCON1bits.VCFG0=0;
   ADCON2bits.ADFM=0;
   ADCON2bits.ACQT2=0;
   ADCON2bits.ACQT1=0;
   ADCON2bits.ACQT0=0;
   ADCON2bits.ADCS2=0;
   ADCON2bits.ADCS1=0;
   ADCON2bits.ADCS0=0;
//-------------------------------
//Serial
   IPR1bits.RCIP=1;
   IPR1bits.TXIP=1;
   PIE1bits.RCIE=1;
   PIE1bits.TXIE=0;
   PIR1bits.RCIF=0;
   PIR1bits.TXIF=0;
   
   TXSTAbits.CSRC=0;
   TXSTAbits.TX9=0;
   TXSTAbits.TXEN=1;
   TXSTAbits.SYNC=0;
   TXSTAbits.SENDB=0;
   TXSTAbits.BRGH=1;
   TXSTAbits.TRMT=1;
   TXSTAbits.TX9D=0;
   
   RCSTAbits.SPEN=0;
   RCSTAbits.RX9=0;
   RCSTAbits.SREN=0;
   RCSTAbits.CREN=1;
   RCSTAbits.ADDEN=0;
   RCSTAbits.FERR=0;
   RCSTAbits.OERR=0;
   
   SPBRGH=0x00;
   SPBRG=0x19;
}
#pragma code
/*------------------------------------------------------------------------------------*/
#pragma code SUBROTINAS

/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/
#pragma code
/*------------------------------------------------------------------------------------*/