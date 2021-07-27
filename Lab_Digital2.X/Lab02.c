// Archivo:  Lab02.c
// Dispositivo:	PIC16F887
// Autor:    Fernando Arribas
// Compilador:	pic-as (v2.31), MPLABX V5.45
// 
// Programa: Coversion ADC y contador con Eusart mostrado en LCD
//           
// Hardware: 1 LCD en PORTB y RC0 y RC1, Pots en RA0 y RA1 y TTL en RC6 y RC7
//           
//
// Creado: 12 jul, 2021
// Ultima modificacion: 19 jul, 2021


#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//******************************************************************************
// Directivas del Compilador
//******************************************************************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#define _XTAL_FREQ 8000000

#include "LCD.h"

//******************************************************************************
// Variables
//****************************************************************************
volatile uint8_t adc1 = 0;
volatile uint8_t adc2 = 0;
volatile uint8_t contador = 0;

char sensor1[10];
char sensor2[10];
char cont1[10];

float conv1 = 0;
float conv2 = 0;
float conv3 = 0;


//******************************************************************************
// Prototipos de funciones
//******************************************************************************
void setup(void);           //Definimos las funciones que vamos a utilizar
void Eusart(void);
void putch(char data);

//******************************************************************************
// Interupción
//******************************************************************************
void __interrupt() isr(void)
{
    //Interupcion del ADC
    if (PIR1bits.ADIF) {
        if  (ADCON0bits.CHS == 0) { //Verificamos el canal que se esta convirtiendo
            adc1 = ADRESH;         //Dependiendo el canal guardamos el resultado
        }
        
        else {
            adc2 = ADRESH;
            PORTD++;
        }
        
        PIR1bits.ADIF = 0;          //Reiniciamos la interupcion
    }
    
}

//******************************************************************************
// Ciclo Principal
//******************************************************************************
void main(void) {
  unsigned int a;
  setup();
  Lcd_Init();
  Lcd_Clear();
  ADCON0bits.GO   = 1;    //Damos inicio a la conversion
  while(1)
  {
      //Primera linea del LDC
      Lcd_Set_Cursor(1, 1);
      Lcd_Write_String("S1:");
      Lcd_Set_Cursor(1, 8);
      Lcd_Write_String("S2:");
      Lcd_Set_Cursor(1, 14);
      Lcd_Write_String("S3:");
      
      Eusart();
      
      //Cambio de canales del ADC
      if (ADCON0bits.GO == 0){        //Cuando termine la conversion
            if (ADCON0bits.CHS == 0) {  //Verificamos cual fue el ultimo canal convertido
                ADCON0bits.CHS = 1;     //Despues cambiamos al siguiente canal
            }
            else {
                ADCON0bits.CHS = 0;
            }
            
            __delay_us(200);            //Esperamos un tiempo para que la conversion
            ADCON0bits.GO = 1;          //termine correctamente
        }                               //Luego le volvemos a indicar que inicie la conversion
      
      //Mostramos en el LCD los valores de los sensores
      Lcd_Set_Cursor(2, 1);             //Elegimos posicion
      Lcd_Write_String(sensor1);        //Escribimos valor del sensor
      Lcd_Set_Cursor(2, 5);             //Nueva posicion
      Lcd_Write_String("V");            //Dimensional del sensor

      Lcd_Set_Cursor(2, 7);
      Lcd_Write_String(sensor2);
      Lcd_Set_Cursor(2, 11);
      Lcd_Write_String("V");
      
      Lcd_Set_Cursor(2, 14);
      Lcd_Write_String(cont1);
      
      //Preparación de los sensores para ser mostrados en el LCD
      conv1 = 0;//se reinicia las cada ves que se inicia el proceso de enviar datos
      conv2 = 0;//tanto para la LCD como por UART.
        
      conv1 = (adc1 / (float) 255)*5; //Se consigue el porcentaje con respecto al valor 
      //maximo que un puerto puede tener, despues se multiplica por 5 para conocer el voltaje actual del puerto                                          
      convert(sensor1, conv1, 2);//se convierte el valor actual a un valor ASCII.
        
      conv2 = (adc2 / (float) 255)*5; //misma logica que conv0
      convert(sensor2, conv2, 2);
      
      convert(cont1, contador, 2);
      
      __delay_ms(500);
  }
    return;
}

void setup(void) {
    //Configuracion de los puertos
    ANSEL   = 0X03;             //Colocamos RA0 y RA1 como entrada analogica
    ANSELH  = 0X00;             //PORTB y el PORTC como salidas
    
    TRISB   = 0X00;             //Colocamos RA0 y RA1 como entradas y el resto del
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    //TRISC   = 0X00;             //PORTB y el PORTC como salidas
    TRISA   = 0X03;
    
    PORTA   = 0X00;
    PORTB   = 0X00;
    PORTC   = 0X00;

    //Configuracion del Oscilador
    OSCCONbits.IRCF2 = 1;       //Reloj interno de 8MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS   = 1;
    
    //Configuracion Interupciones
    INTCONbits.GIE   = 1;       //Activamos la interupcion del ADC
    INTCONbits.PEIE  = 1;
    PIE1bits.ADIE    = 1;       //Interumpcion del ADC
    
    PIR1bits.RCIF    = 0;
    
    //Configuracion ADC
    ADCON1bits.ADFM     = 0;    //Justificado a la izquierda
    ADCON1bits.VCFG0    = 0;    //Colocamos los voltajes de ref como VSS y VDD
    ADCON1bits.VCFG1    = 0;
    
    ADCON0bits.ADCS1    = 1;    //Reloj de conversion como FOSC/32
    ADCON0bits.CHS      = 0;    //Chanel 0
    __delay_us(200);
    ADCON0bits.ADON     = 1;    //Encendemos el ADC
    __delay_us(200);
    
    //Configuracion de TX y RX
    TXSTAbits.SYNC  = 0;    //Modo asincrono
    TXSTAbits.BRGH  = 1;    //Activamos la alta velocidad del Baud rate
    
    BAUDCTLbits.BRG16   = 1;    //Utilizamos los 16 bits del Baud rate
    
    SPBRG   = 207;  //Elegimos el baud rate 9600
    SPBRGH  = 0;
    
    RCSTAbits.SPEN  = 1;    //Activamos los puertos seriales
    RCSTAbits.RX9   = 0;    //No utilizamos los nueve bits
    RCSTAbits.CREN  = 1;    //Activamos la recepción continua
    
    TXSTAbits.TXEN  = 1;    //Activamos la transmición
   
    return;
}

void putch(char data){
    while (TXIF == 0);      //Esperar a que se pueda enviar un nueva caracter
    TXREG = data;           //Transmitir un caracter
    return;
}

void Eusart(void) {         //Mostramos los resultados del ADC y contador en 
   __delay_ms(100);         //El Eusart
   printf("\rSensor 1: \r");
   __delay_ms(100);
   printf(sensor1);
   __delay_ms(100);
   printf("\r---------------\r");
   
   
   __delay_ms(100);
   printf("\rSensor 2: \r");
   __delay_ms(100);
   printf(sensor2);
   __delay_ms(100);
   printf("\r---------------\r");
   
   __delay_ms(100);
   printf("\rContador: \r");
   __delay_ms(100);
   printf(cont1);
   __delay_ms(100);
   printf("\r---------------\r");
   
   if (RCREG == '+') {      //Preguntamos si queremos incrementar o decrementar
       contador++;          //el contador
       RCREG = 0;
   }
   else if (RCREG == '-') {
       contador--;
       RCREG = 0;
   }
   else {
       NULL;
   }
   return;
}

