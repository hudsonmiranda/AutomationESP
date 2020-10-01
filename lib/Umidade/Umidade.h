/*
 * Umidade.h
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#ifndef LIB_UMIDADE_UMIDADE_H_
#define LIB_UMIDADE_UMIDADE_H_

#include <Arduino.h>

class Umidade {
public:
	short desejada, maxima, minima, iterator;
	String 	menu[2] =   {"Umidade Desejada",
						 "Definir Max. e Min."},
			title = "    -|UMIDADE|-     ";
	
	Umidade() {
		this->desejada=70;
		this->iterator=2;
		this->maxima=0;
		this->minima=0;
	}
	virtual ~Umidade();

	void menu_umidade(){}
};

#endif /* LIB_UMIDADE_UMIDADE_H_ */
