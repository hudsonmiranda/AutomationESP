/*
 * Navegacao.h
 *
 *  Created on: Oct 7, 2020
 *      Author: hudsonm
 */

#ifndef NAVEGACAO_H_
#define NAVEGACAO_H_

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

class Navegacao {

public:
	char key,
		 keys[4][3] =  {{'1','2','3'},
				    	{'4','5','6'},
				    	{'7','8','9'},
				    	{'*','0','#'}};

	// For keypads
	byte rowPins[4] = {23, 3, 19, 18};
	byte colPins[3] = {5, 17, 16};
	Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 3);

//	LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F,20,4);

	short switchJoystick,
		  eixoX,
		  eixoY,
		  it,
		  l,
		  c,
		  page;

	Navegacao() {
		this->switchJoystick = 14;
		this->eixoX = 33;
		this->eixoY = 32;
//		this->lcd.init();   // initializing the LCD
//		this->lcd.backlight(); // Enable or Turn On the backlight
	}
	virtual ~Navegacao();
	void NavEixoY(short sc);
	void Escape();
};

#endif /* NAVEGACAO_H_ */
