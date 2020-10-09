/*
 * Temporizador.cpp
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#include "Temporizador.h"
#include <LiquidCrystal_I2C.h>

Temporizador::~Temporizador() {}

void Temporizador::InitLCD() {
	lcd.init();   // initializing the LCD
	lcd.backlight(); // Enable or Turn On the backlight
}

void Temporizador::MenuTimer() {
// void Temporizador::MenuTimer(LiquidCrystal_I2C lcd) {
	short crtempo[] = {1,9,10,12,13,15,16,18,19};
	short s = sizeof(crtempo)/sizeof(crtempo[0])-1;
	Serial.printf("s: %u\n",s);
	lcd.clear();
	lcd.setCursor(0,0); lcd.printf("|-TEMPORIZADOR-|%u/5|",page);
	lcd.setCursor(0,1); lcd.printf("IDADE|    LIG | DES");	
	switch (page){
	case 1:
		lcd.setCursor(2,2);
		lcd.printf("%02u |P%u|%02u:%02u|%02u:%02u",Idade[itI].dia,(page-1),Idade[itI].time[(page-1)][0],Idade[itI].time[(page-1)][1],Idade[itI].time[(page-1)][2],Idade[itI].time[(page-1)][3]);
		lcd.setCursor(5,3);
		lcd.printf("|P%u|%02u:%02u|%02u:%02u",(page),Idade[itI].time[(page)][0],Idade[itI].time[(page)][1],Idade[itI].time[(page)][2],Idade[itI].time[(page)][3]);
		break;
	case 2:
		lcd.setCursor(2,2);
		lcd.printf("%02u |P%u|%02u:%02u|%02u:%02u",Idade[itI].dia,(page),Idade[itI].time[(page)][0],Idade[itI].time[(page)][1],Idade[itI].time[(page)][2],Idade[itI].time[(page)][3]);
		lcd.setCursor(5,3);
		lcd.printf("|P%u|%02u:%02u|%02u:%02u",(page+1),Idade[itI].time[(page+1)][0],Idade[itI].time[(page+1)][1],Idade[itI].time[(page+1)][2],Idade[itI].time[(page+1)][3]);
		break;
	
	case 3:
		lcd.setCursor(2,2);
		lcd.printf("%02u |P%u|%02u:%02u|%02u:%02u",Idade[itI].dia,(page+1),Idade[itI].time[(page+1)][0],Idade[itI].time[(page+1)][1],Idade[itI].time[(page+1)][2],Idade[itI].time[(page+1)][3]);
		lcd.setCursor(5,3);
		lcd.printf("|P%u|%02u:%02u|%02u:%02u",(page+2),Idade[itI].time[(page+2)][0],Idade[itI].time[(page+2)][1],Idade[itI].time[(page+2)][2],Idade[itI].time[(page+2)][3]);
		break;
	
	case 4:
		lcd.setCursor(2,2);
		lcd.printf("%02u |P%u|%02u:%02u|%02u:%02u",Idade[itI].dia,(page+2),Idade[itI].time[(page+2)][0],Idade[itI].time[(page+2)][1],Idade[itI].time[(page+2)][2],Idade[itI].time[(page+2)][3]);
		lcd.setCursor(5,3);
		lcd.printf("|P%u|%02u:%02u|%02u:%02u",(page+3),Idade[itI].time[(page+3)][0],Idade[itI].time[(page+3)][1],Idade[itI].time[(page+3)][2],Idade[itI].time[(page+3)][3]);
		break;
	
	case 5:
		lcd.setCursor(2,2);
		lcd.printf("%02u |P%u|%02u:%02u|%02u:%02u",Idade[itI].dia,(page+3),Idade[itI].time[(page+3)][0],Idade[itI].time[(page+3)][1],Idade[itI].time[(page+3)][2],Idade[itI].time[(page+3)][3]);
		lcd.setCursor(5,3);
		lcd.printf("|P%u|%02u:%02u|%02u:%02u",(page+4),Idade[itI].time[(page+4)][0],Idade[itI].time[(page+4)][1],Idade[itI].time[(page+4)][2],Idade[itI].time[(page+4)][3]);
		break;
	}
	lcd.setCursor(1,3); lcd.printf("%u/%u ",(itI+1),(sizeof(Idade)/sizeof(Idade[0])));
	if (c==0){lcd.setCursor(1,2); lcd.write(B01111110);} else {lcd.setCursor(1,2);lcd.write(B10100101);}
	do{
		key = keypad.getKey();
		if (c==0){
			lcd.blink_off();
			if(key=='*') Escape();
		} 
		else {
			
			lcd.setCursor(crtempo[c],l); lcd.blink();
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
						Grava(Idade[itI].time);
						lcd.print(key);
						break;
					case '*':
						Escape();
				}
			}
		}
	} while (analogRead(eixoX)>1000 &&
			 analogRead(eixoX)<3000 &&
			 analogRead(eixoY)>1000 &&
			 analogRead(eixoY)<3000);

	//  -===TOP / BOTTOM===-
	if(analogRead(eixoY)>3000 || analogRead(eixoY)<1000){
		NavEixoY(5);// 5 nº Max de Pages
		MenuTimer();
	}
	//  -===RIGTH / LFET===-
	if(analogRead(eixoX)>3000 || analogRead(eixoX)<1000){
		NavEixoX(s);
		MenuTimer();
	}
	
}

void Temporizador::NavEixoY(short sc) {
	if(analogRead(eixoY)<1000){
		if (c==0){
			if (itI>0){
				itI--;
				Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
				MenuTimer();
			}
		} else {
			if(page>1 || l>2){
				l--;
				it--;
				if(l==1) {l=3;page--;}
				Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
				MenuTimer();
			}
			else{
				Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
				MenuTimer();
			}
		}
	}
	//  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
		if (c==0){
			if (itI<(sizeof(Idade)/sizeof(Idade[0])-1)){
				itI++;
				Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
				MenuTimer();
			}
		} else {
			if(page<sc || l<3){
				l++;
				it++;
				if(l==4){l=2;page++;}
				Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
				MenuTimer();
			}
			else{
				Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
				MenuTimer();
			}
		}
  	}
}

void Temporizador::NavEixoX(short sc) {
	//  -===RIGTH===-
	if(analogRead(eixoX)>3000){
		if(c<sc){
			c++;
			Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
		}
		else{
			Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
		}
	}
	//  -===LEFT===-
	if(analogRead(eixoX)<1000){
		if(c>0){
			c--;
			Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
		}
		else{
			Serial.printf("itI: %u l: %u c: %u it: %u page: %u \n",itI,l,c,it,page);
		}
	}
}

void Temporizador::Grava(short table[10][4]) {
	char aux[3]; 
	short a,sc,ac;//a -> iterador de navegação em aux[]; sc -> size crusor; ac -> Aux. Coluna
	
	if(c<5){
		sc=2;
		a=(c<=sc)?(c-1):(c-3);	
		ac=(c<=sc)?0:1;
	} else {
		sc=6;
		a=(c<=sc)?(c-5):(c-7);
		ac=(c<=sc)?2:3;
	}
	
	sprintf(aux,"%02u",table[it][ac]); //sc:6 a:1 ac:3 [1][3]
	Serial.printf("aux[%u]:'%c'|| aux (String): %s\n",a,aux[a],aux);
	Serial.printf("BEF => aux (a: %u - c:%u - l:%u - sc:%u): %s\n",a,c,l,sc,aux);
	aux[a]=key;
	Serial.printf("AFT => aux (a: %u - c:%u - l:%u - sc:%u): %s\n",a,c,l,sc,aux);
	table[it][ac] = atoi(aux);
	Serial.printf("=> table[%u][%u]: {%u}\n",it,ac,table[it][ac]);
}
