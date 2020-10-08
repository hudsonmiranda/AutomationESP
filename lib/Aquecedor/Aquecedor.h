/*
 * Aquecedor.h
 *
 *  Created on: Oct 8, 2020
 *      Author: hudsonm
 */

#ifndef AQUECEDOR_H_
#define AQUECEDOR_H_

#include <LiquidCrystal_I2C.h>
#include <Navegacao.h>

class Aquecedor : public Navegacao {
public:
	float temp[2][2];
	short time[2][2];

	Aquecedor() : Navegacao() {
		this->it=1;
		this->l=2;
		this->c=0;
		this->page=1;
	}
	virtual ~Aquecedor();

	void MenuAquecedor(LiquidCrystal_I2C lcd);
	void NavEixoX(short cr[]);
	
	template <class T>
	void GravaAq(T table[2][2]);
};


#endif /* AQUECEDOR_H_ */
