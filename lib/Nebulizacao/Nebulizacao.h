/*
 * Nebulizacao.h
 *
 *  Created on: Oct 5, 2020
 *      Author: hudsonm
 */

#ifndef LIB_NEBULIZACAO_NEBULIZACAO_H_
#define LIB_NEBULIZACAO_NEBULIZACAO_H_

#include <Arduino.h>

class Nebulizacao {
public:
	String title = "|-   NEBULIZACAO  -|";
	float temp[2][2] = {{0,0},{0,0}};
	short g_on[2] = {0,0},
		  umid[2][2] = {{0,0},{0,0}},
		  time[2][2] = {{0,0},{0,0}},
		  cursorTemp[6] = {9,10,12,15,16,18},
		  cursorUmid[4] =   {10,11,16,17},
		  cursorTime[8] = {9,10,11,12,15,16,17,18},
		  cursorGon[4]  =   {11,12,17,18};
		  
	Nebulizacao() {}
	virtual ~Nebulizacao();
	Nebulizacao(const Nebulizacao &other);
	Nebulizacao& operator=(const Nebulizacao &other);
};

#endif /* LIB_NEBULIZACAO_NEBULIZACAO_H_ */
