// Archivo:  Lab05.c
// Dispositivo:	PIC16F887
// Autor:    Fernando Arribas
// Compilador:	pic-as (v2.31), MPLABX V5.45
// 
// Programa: Comunicacion Eusart y contador
//           
// Hardware: Leds en PORTD y push butons en PORTB
//           
//
// Creado: 14 aug, 2021
// Ultima modificacion: 14 aug, 2021
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

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <pic16f887.h>
#include <xc.h>
#include <stdint.h>
#include <stdio.h>  // Para usar printf
#include <string.h> // Concatenar
#include <stdlib.h> //Recibir numeros

//*****************************************************************************
// Variables
//*****************************************************************************
uint8_t contador, ada;
char cen, dec, uni;
char var;
char con;
int full;
//*****************************************************************************
// Definiciones
//*****************************************************************************
#define _XTAL_FREQ 8000000

//*****************************************************************************
// Prototipos de Funciones
//*****************************************************************************
void setup(void);
void Eusart(void);
void putch(char data);
//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void){
   //Interupcion on change del puerto B
    if (INTCONbits.RBIF == 1){                 //Contador
        if (PORTBbits.RB0 == 1){              //Verificamos cual boton se presiona
            contador++;                //Si es RB0 incrementamos
        }
        
        if (PORTBbits.RB1 == 1){              //Si es RB1 decrementamos
            contador--;
        }
        
        INTCONbits.RBIF = 0;        //Reiniciamos la interupción
    }
    
    //Interupcion de recepcion Eusart
//    if (PIR1bits.RCIF == 1) {
//        ada = RCREG; 
//    }

}

//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    while(1){
       PORTD = full; 
       Eusart();
    }
    return;
}
//*****************************************************************************
// Funciones
//*****************************************************************************
void setup(void){
    ANSEL = 0x00;
    ANSELH = 0x00;
    
    TRISA = 0x00;
    TRISB = 0x03;
    TRISD = 0x00;
    
    PORTA = 0x00;
    PORTB = 0x00;
    PORTD = 0x00;   
    
    //Configuracion del Oscilador
    OSCCONbits.IRCF2 = 1;       //Reloj interno de 8MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS   = 1;
    
    //Configuracion Interupciones
    INTCONbits.GIE   = 1;       //Activamos la interupcion on change del PORTB
    INTCONbits.PEIE  = 1;
    INTCONbits.RBIE  = 1;
    
    INTCONbits.RBIF  = 0;
    
    //Configuracion Puerto B
    IOCBbits.IOCB0   = 1;       //Activamos en RB0 y RB1 la interupcion on change
    IOCBbits.IOCB1   = 1;
    
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
    printf("\r------------------------\r");
    printf("Valor del Contador: ");
    __delay_ms(1000);
    printf("%d",contador);     //Enviamos el valor del Contador
    printf("\r------------------------\r");
   
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
