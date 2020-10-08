/*
 * Aquecedor.cpp
 *
 *  Created on: Oct 8, 2020
 *      Author: hudsonm
 */

#include "Aquecedor.h"
#include <LiquidCrystal_I2C.h>

Aquecedor::~Aquecedor() {
	// TODO Auto-generated destructor stub
}

void Aquecedor::MenuAquecedor(LiquidCrystal_I2C lcd) {
	short crtemp[] = {7,8,10,15,16,18},
		  crtime[] = {7,8,9,10,15,16,17,18};
	lcd.clear();
	lcd.setCursor(0,0); lcd.printf("|- AQUECEDOR  -|%u/2|",page);
	lcd.setCursor(0,1); lcd.print("     |  LIG  |  DES");
	switch (page){
		case 1:
			lcd.setCursor(0,2); lcd.printf(" A1%u | %04.1fC | %04.1fC",(page),temp[0][0],temp[0][1]);
			lcd.setCursor(0,3); lcd.printf(" A1%u | %04.1fC | %04.1fC",(page+1),temp[1][0],temp[1][1]);
			break;
		case 2:
			lcd.setCursor(0,2); lcd.printf(" A2%u | %04us | %04us",(page-1),time[0][0],time[0][1]);
			lcd.setCursor(0,3); lcd.printf(" A2%u | %04us | %04us",(page),time[1][0],time[1][1]);
			break;
	}
	do {
		key = keypad.getKey();
		if(page==1) {
			lcd.setCursor(crtemp[c],l);
			lcd.blink();
		} else{
			lcd.setCursor(crtime[c],l);
			lcd.blink();
		}
		if (key) {
			switch (key) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					// if(page==1) GravaAq(temp);
					// else GravaAq(time);
					(page==1)?GravaAq(temp):GravaAq(time);
					lcd.print(key);
					break;
				case '*':
					lcd.noCursor(); //blink_off();
					lcd.clear();
					it=1;
					l=2;
					c=0;
					page=1;
          			return;
			}
    	}
	} while (analogRead(eixoX)>1000 &&
			 analogRead(eixoX)<3000 &&
			 analogRead(eixoY)>1000 &&
			 analogRead(eixoY)<3000);
	
	//  -===TOP===-
	if(analogRead(eixoY)<1000){
		if(page>1 || l>2){
			l--;
			it--;
			if(l==1) {l=3;page--;}
			Serial.printf("l: %u c: %u it: %u page: %u \n",l,c,it,page);
			MenuAquecedor(lcd);
		}
		else{
			Serial.printf("l: %u c: %u it: %u page: %u \n",l,c,it,page);
			MenuAquecedor(lcd);
		}
	}
	//  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
		if(page<2 || l<3){
			l++;
			it++;
			if(l==4){l=2;page++;}
			Serial.printf("l: %u c: %u it: %u page: %u \n",l,c,it,page);
			MenuAquecedor(lcd);
		}
		else{
			Serial.printf("l: %u c: %u it: %u page: %u \n",l,c,it,page);
			MenuAquecedor(lcd);
		}
  	}
	//  -===RIGTH / LFET===-
	if(analogRead(eixoX)>3000 || analogRead(eixoX)<1000){
		if (page==1){
			NavEixoX(crtemp);
			MenuAquecedor(lcd);
		} else {
			NavEixoX(crtime);
			MenuAquecedor(lcd);
		}		
	}
}

void Aquecedor::NavEixoX(short cr[]) {
	//  -===RIGTH===-
	if(analogRead(eixoX)>3000){
		if(cr[c]<18){
			c++;
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
		}
		else{
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
		}
	}
	//  -===LEFT===-
	if(analogRead(eixoX)<1000){
		if(cr[c]>7){
			c--;
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
		}
		else{
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
		}
	}
}


template <class T>
void Aquecedor::GravaAq(T table[2][2]) {
	char aux[5], exp[7];
	short a,sc,ac;// sc -> size crusor; ac -> Aux. Coluna
	
	if(page==1){
		sc=2;
		strcpy(exp,"%04.1f");
		a=(c<=sc)?c:(c-3);
		Serial.printf("Exp: %s\n",exp);
	} else{
		sc=3;
		a=(c<=sc)?c:(c-4);
		strcpy(exp,"%04u");
		Serial.printf("Exp: %s\n",exp);
	}
	ac=(c<=sc)?0:1;
	
	sprintf(aux,exp,table[(l-2)][ac]);
	Serial.printf("aux[%u]:'%c'|| aux (String): %s\n",a,aux[a],aux);
	Serial.printf("BEF => aux (a: %u - c:%u - l:%u - sc:%u): %s\n",a,c,l,sc,aux);
	if(aux[a]=='.') a++;
	aux[a]=key;
	Serial.printf("AFT => aux (a: %u - c:%u - l:%u - sc:%u): %s\n",a,c,l,sc,aux);
	table[(l-2)][ac] = atof(aux);
	Serial.printf("=> table[%u][%u]: {%f}\n",(l-2),ac,table[(l-2)][ac]);
}