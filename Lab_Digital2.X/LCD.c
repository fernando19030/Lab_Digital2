/* 
 * File: LCD.c  
 * Se utilizó y se adaptaron las librerías de Ligo George 
 * de la página www.electrosome.com
 * Enlace: https://electrosome.com/lcd-pic-mplab-xc8/
 * Revision history: 
 */

//Librerias del LCD por Electrosome
#define _XTAL_FREQ 4000000
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "LCD.h"

void Lcd_Port(char a) { //Puerto del LCD
    PORTB = a;
}

void Lcd_Cmd(char a) {  //Comandos de preparacion del LCD
    RS = 0; // => RS = 0 // Dato en el puerto lo va interpretar como comando
    Lcd_Port(a);
    EN = 1; // => E = 1
    __delay_ms(4);
    EN = 0; // => E = 0
}

void Lcd_Clear(void) {  //Limpieza del LCD
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b) {   //Posicion horizontal del LCD
    char temp;
    if (a == 1) {
        temp = 0x80 + b - 1;
        Lcd_Cmd(temp);
    } else if (a == 2) {
        temp = 0xC0 + b - 1;
        Lcd_Cmd(temp);
    }
}

void Lcd_Init(void) {   //Preparacion del LCD
    Lcd_Port(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x30);
    __delay_ms(5);
    Lcd_Cmd(0x30);
    __delay_ms(11);
    Lcd_Cmd(0x30);
    /////////////////////////////////////////////////////
    Lcd_Cmd(0x38);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x6);
}

void Lcd_Write_Char(char a) {   //Escribir un caracter en el LCD
    RS = 1; // => RS = 1
    Lcd_Port(a); //Data transfer
    EN = 1;
    __delay_us(10);
    EN = 0;
}

void Lcd_Write_String(char *a) {    //Escribir cadena de caracteres
    int i;
    for (i = 0; a[i] != '\0'; i++)
        Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right(void) {    //Corrido a la derecha
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x0C);
}

void Lcd_Shift_Left(void) { //Corrido a la izquierda
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x08);
}

//Librerias del ADC 
// Conversion del ADC para mostrarlo en el LCD por embeded life
void convert(char *data,float a, int place) //definition
{
     int temp=a;
     float x=0.0;
     int digits=0;
     int i=0,mu=1;
     int j=0;
     if(a<0)
     {
            a=a*-1;
            data[i]='-';
            i++;
      }
     //exponent component
     while(temp!=0)
     {
         temp=temp/10;
         digits++;          
     }
     while(digits!=0)
     {
         if(digits==1)mu=1;
         else  for(j=2;j<=digits;j++)mu=mu*10;
         
         x=a/mu;
         a=a-((int)x*mu);
         data[i]=0x30+((int)x);
         i++;
         digits--;
         mu=1;
     }
     //mantissa component
     data[i]='.';
     i++;
     digits=0;
     for(j=1;j<=place;j++)mu=mu*10;
     x=(a-(int)a)*mu; //shift places
     a=x;
     temp=a;
     x=0.0;
     mu=1;
     digits=place;
     while(digits!=0)
     {
         if(digits==1)mu=1;
         else  for(j=2;j<=digits;j++)mu=mu*10;
         
         x=a/mu;
         a=a-((int)x*mu);
         data[i]=0x30+((int)x);
         i++;
         digits--;
         mu=1;
     }   
     
    data[i]='\n';
}

//Librerias comunicacion EUSART
