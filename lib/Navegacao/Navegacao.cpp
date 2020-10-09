/*
 * Navegacao.cpp
 *
 *  Created on: Oct 7, 2020
 *      Author: hudsonm
 */

#include "Navegacao.h"

Navegacao::~Navegacao() {}

void Navegacao::Escape() {
	// lcd.noCursor(); //blink_off();
	// lcd.clear();
	it=0;
	l=2;
	c=0;
	page=1;
	return;
}

// void Navegacao::NavEixoY(short sc) {
// 	//  -===TOP===-
// 	if(analogRead(eixoY)<1000){
// 		if(page>1 || l>2){
// 			l--;
// 			it--;
// 			if(l==1) {l=3;page--;}
// 			Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
// 			MenuTimer();
// 		}
// 		else{
// 			Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
// 			MenuTimer();
// 		}
// 	}
// 	//  -===BOTTOM===-
// 	if(analogRead(eixoY)>3000){
// 		if (c==0){
// 			if (itI<(sizeof(Idade)/sizeof(Idade[0])-1)){
// 				itI++;
// 				Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
// 				MenuTimer();
// 			}
// 		} else {
// 			if(page<sc || l<3){
// 				l++;
// 				it++;
// 				if(l==4){l=2;page++;}
// 				Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
// 				MenuTimer();
// 			}
// 			else{
// 				Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
// 				MenuTimer();
// 			}
// 		}
//   	}
// }
