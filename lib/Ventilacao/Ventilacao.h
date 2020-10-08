/*
 * Ventilacao.h
 *
 *  Created on: Oct 6, 2020
 *      Author: hudsonm
 */

#ifndef LIB_VENTILACAO_H_
#define LIB_VENTILACAO_H_

#include <LiquidCrystal_I2C.h>
#include <Navegacao.h>

class Ventilacao : public Navegacao
{
public:
	float grupos[4][2], //Exaustor 10x2, Ventilador 4x2 //Conf. Liga/Desliga por Grupo de Ventilação -> |Grupo|Liga|Desliga|
		  dif[2]; //Diferença em ºC para Ligar/Desligar -> |Liga|Desliga|
	short vm[6][3]; //Ventilaçao mínima -> |Idade|Liga|Desliga|
	// 	  it,l,c,page;
	// char  key;

	Ventilacao(): Navegacao() {
		this->it=1;
		this->l=2;
		this->c=0;
		this->page=1;
	};
	virtual ~Ventilacao();

	// void MenuVentilation(LiquidCrystal_I2C lcd, short eixoY, short switchJoystick);
	void MenuVentilation(LiquidCrystal_I2C lcd);
	void VMinima(LiquidCrystal_I2C lcd);
	void DifOnOff(LiquidCrystal_I2C lcd);
	void ConfGrupos(LiquidCrystal_I2C lcd);
	void GravaTemp();
	void GravaVM();
};

#endif /* LIB_VENTILACAO_H_ */
