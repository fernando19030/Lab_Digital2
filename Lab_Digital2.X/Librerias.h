/* 
 * File:   Librerias.h
 * Author: Fernando Arribas
 *
 * Created on 16 de julio de 2021, 09:11 PM
 */

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#ifndef __LIBRERIAS_H_
#define	__LIBRERIAS_H_

void config_osc(uint8_t frec);
void config_tmr0(uint8_t prescaler);
void config_ADC(uint8_t adcFrec);
void transfer_ADC (uint8_t dato);
void nibbles (uint8_t dato);
void segmentos (uint8_t dato);

#endif	/* LIBRERIAS_H */

