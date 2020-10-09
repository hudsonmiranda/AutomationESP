/*
 * Temporizador.h
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#ifndef LIB_TEMPORIZADOR_TEMPORIZADOR_H_
#define LIB_TEMPORIZADOR_TEMPORIZADOR_H_

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Naveg.h>

struct Timer
{
	short dia,
		  time[10][4];
};


class Temporizador : public Naveg {
public:
	Timer Idade[6];
	short itI;// Iterador Idade

	Temporizador() {
		this->itI=0;
		this->it=0;
		this->l=2;
		this->c=0;
		this->page=1;
		
		for(short i=0;i<6;i++){
			if(i==0) Idade[i].dia = 0; else Idade[i].dia = Idade[(i-1)].dia+7;
		}
	}

	virtual ~Temporizador();

	void InitLCD();
	void MenuTimer();
	// void MenuTimer(LiquidCrystal_I2C lcd);
	void NavEixoY(short sc);
	void NavEixoX(short sc);
	void Grava(short table[10][4]);
};

#endif /* LIB_TEMPORIZADOR_TEMPORIZADOR_H_ */
