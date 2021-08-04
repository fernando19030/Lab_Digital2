// Archivo:  Lab03_Master.c
// Dispositivo:	PIC16F887
// Autor:    Fernando Arribas
// Compilador:	pic-as (v2.31), MPLABX V5.45
// 
// Programa: Comunicacion SPI y Eusart
//           
// Hardware: Leds en PORTB
//           
//
// Creado: 01 aug, 2021
// Ultima modificacion: 04 aug, 2021
//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
// CONFIG1
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>  // Para usar printf
#include <string.h> // Concatenar
#include <stdlib.h> //Recibir numeros
#include "Librerias.h"

//*****************************************************************************
// Variables
//*****************************************************************************
float conv1 = 0;
float conv2 = 0;

char sensor1[10];
char sensor2[10];

volatile uint8_t adc1 = 0;
volatile uint8_t adc2 = 0;

char pot1, pot2;
int contador;
char hundreds, tens, units, residuo;
char cen, dec, uni;
char var;
char con;
int full;
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
void Eusart(void);
void putch(char data);

//*****************************************************************************
// Código Principal
//*****************************************************************************
void main(void) {
    setup();    //Configuracion
    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    while(1){
        
       PORTCbits.RC2 = 0;       //Slave Select
       __delay_ms(1);
       
       spiWrite(1);             //Comando del master al slave
       adc1 = spiRead();        //Dato del slave
       
       __delay_ms(1);
       PORTCbits.RC2 = 1;       //Slave Deselect 
       
       PORTCbits.RC2 = 0;       //Slave Select
       __delay_ms(1);
       
       spiWrite(2);
       adc2 = spiRead();
       
       __delay_ms(1);
       PORTCbits.RC2 = 1;       //Slave Deselect
       
       //Preparación de los sensores para ser mostrados en el LCD
       conv1 = 0;//se reinicia las cada ves que se inicia el proceso de enviar datos
       conv2 = 0;//tanto para la LCD como por UART.
        
       conv1 = (adc1 / (float) 255)*5; //Se consigue el porcentaje con respecto al valor 
       //maximo que un puerto puede tener, despues se multiplica por 5 para conocer el voltaje actual del puerto                                          
       convert(sensor1, conv1, 2);//se convierte el valor actual a un valor ASCII.
        
       conv2 = (adc2 / (float) 255)*5; //misma logica que conv0
       convert(sensor2, conv2, 2);
       
       Eusart();    //LLamamos la comunicacion del Eusart
       
       PORTB = full;    //Guardamos le contador en el puerto b
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0x00;
    ANSELH = 0x00;
    
    TRISCbits.TRISC2 = 0;
    TRISB = 0x00;
    TRISD = 0x00;
    
    PORTB = 0x00;
    PORTD = 0x00;
    PORTCbits.RC2 = 1;
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    
    //Configuracion del Oscilador
    OSCCONbits.IRCF2 = 1;       //Reloj interno de 8MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS   = 1;
    
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

}

void putch(char data){
    while (TXIF == 0);      //Esperar a que se pueda enviar un nueva caracter
    TXREG = data;           //Transmitir un caracter
    return;
}

void Eusart (void) {
    __delay_ms(100);         
   printf("\rSensor 1: \r"); //Enviamos cadena del caracteres
   __delay_ms(100);
   printf(sensor1);     //Enviamos el valor del ADC convertido
   __delay_ms(100);
   printf("\r---------------\r");
   
   
   __delay_ms(100);
   printf("\rSensor 2: \r");
   __delay_ms(100);
   printf(sensor2);
   __delay_ms(100);
   printf("\r---------------\r");
   
   printf("Ingresar Centena: Rango(0-2)\r");    //Preguntamos la centena del contador
      defensa1:  //Si el valor es mayor a 2 aplicamos una defensa
       while(RCIF == 0);
        cen = RCREG -48;  

       while(RCREG > '2'){ 
           goto defensa1;
       }
    
    printf("Ingresar Decenas: \r"); //Preguntamos la decena
      defensa2: //Si el valor no es valido aplicamos una defensa
        while(RCIF == 0); 
         dec = RCREG -48; 

        if(cen == 2){
           while(RCREG > '5'){
               goto defensa2;
           }
       }

    printf("Ingresar Unidades: \r");    //Preguntamos la unidades
      defensa3: //Si el valor no es valido aplicamos defensa
       while(RCIF == 0); 
        uni = RCREG - 48;

       if(cen == 2 && dec == 5){
           while(RCREG > '5'){
               goto defensa3;
           }
       }
      con = concat(cen, dec);   //Concatenamos los datos de decena y centena
      full = concat(con, uni);  //Concatenamos los daros de con y unidad
      __delay_ms(250);
    printf("El numero elegido es: %d", full);   //mostramos el valor concatenado completo
}
   

int concat(int a, int b)
{
 
    char s1[20];
    char s2[20];
 
    // Convertimos ambos integers a string
    sprintf(s1, "%d", a);
    sprintf(s2, "%d", b);
 
    // Concatenamos ambos strings
    strcat(s1, s2);
 
    // Convertimos los strings concatenados
    // a integers
    int c = atoi(s1);
 
    // regresamos el valor integrer
    return c;
}

