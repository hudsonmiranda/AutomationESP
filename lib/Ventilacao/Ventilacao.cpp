/*
 * Ventilacao.cpp
 *
 *  Created on: Oct 6, 2020
 *      Author: hudsonm
 */

#include "Ventilacao.h"
#include <LiquidCrystal_I2C.h>
#include <Navegacao.h>

Ventilacao::~Ventilacao() {} // Destrutor

// void Ventilacao::MenuVentilation(LiquidCrystal_I2C lcd, short eixoY, short switchJoystick) {
void Ventilacao::MenuVentilation(LiquidCrystal_I2C lcd){
	lcd.clear();
	lcd.setCursor(0,it); lcd.print(">");//lcd.write(0);
	do{
	int i=0;
		lcd.setCursor(0,0); lcd.print("|-   VENTILACAO   -|");
		lcd.setCursor(1,1); lcd.print("Ventilacao Minima");
		lcd.setCursor(1,2); lcd.print("Dif. Liga/Desliga");
		lcd.setCursor(1,3); lcd.print("Ajuste de Grupos");
	} while (analogRead(eixoY)>1000 && 
			 analogRead(eixoY)<3000 && 
			 digitalRead(switchJoystick)!=0);

	//  -===TOP===-
	if(analogRead(eixoY)<1000){
		if(it>1){
			it--;
			if(it==0) it=1;
			Serial.printf("it: %u\n",it);
			MenuVentilation(lcd);
		}
		else{
			Serial.printf("it: %u\n",it);
			MenuVentilation(lcd);
		}
	}
	//  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
		if(it<3){
			it++;
			if(it==4) it=3;
			Serial.printf("it: %u\n",it);
			MenuVentilation(lcd);
		}
		else{
			Serial.printf("it: %u\n",it);
			MenuVentilation(lcd);
		}
	}
	// -=== Selector ===-
	if(digitalRead(switchJoystick)==0){
		Serial.printf("Swtich Intetator: %u\n",it);
		switch (it){
			case 1: // Ventilação Mínima
				it=1;
				VMinima(lcd);
				break;
			case 2: // Diferença: Liga / Desliga
				DifOnOff(lcd);
				break;
			case 3: // Ajuste de Grupos de Ventilação
				it=1;
				ConfGrupos(lcd);
				break;
			default:
				break;
		}
	}
}

// void Ventilacao::VMinima(LiquidCrystal_I2C lcd, short page,short l,short c){
void Ventilacao::VMinima(LiquidCrystal_I2C lcd){
	short cr[] = {2,3,7,8,9,10,15,16,17,18};
	lcd.clear();
	lcd.setCursor(0,0); lcd.printf("|- VEN MINIMA -|%u/3|",page);
	lcd.setCursor(0,1); lcd.print("IDADE|  LIG  |  DES");
	switch (page){
	case 1:
		lcd.setCursor(0,2); lcd.printf("%u %02u | %04us | %04us",(page-1),vm[page-1][0],vm[page-1][1],vm[page-1][2]);
    	lcd.setCursor(0,3); lcd.printf("%u %02u | %04us | %04us",(page),vm[page][0],vm[page][1],vm[page][2]);
		break;
	case 2:
		lcd.setCursor(0,2); lcd.printf("%u %02u | %04us | %04us",(page),vm[page][0],vm[page][1],vm[page][2]);
    	lcd.setCursor(0,3); lcd.printf("%u %02u | %04us | %04us",(page+1),vm[page+1][0],vm[page+1][1],vm[page+1][2]);
		break;
	case 3:
		lcd.setCursor(0,2); lcd.printf("%u %02u | %04us | %04us",(page+1),vm[page+1][0],vm[page+1][1],vm[page+1][2]);
    	lcd.setCursor(0,3); lcd.printf("%u %02u | %04us | %04us",(page+2),vm[page+2][0],vm[page+2][1],vm[page+2][2]);
		break;
	default:
		break;
	}
	do{
		lcd.setCursor(cr[c],l); lcd.blink();
		key = keypad.getKey();
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
					GravaVM();	
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
			VMinima(lcd);
		}
		else{
			Serial.printf("l: %u c: %u it: %u page: %u \n",l,c,it,page);
			VMinima(lcd);
		}
	}
	//  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
		if(page<3 || l<3){
			l++;
			it++;
			if(l==4){l=2;page++;}
			Serial.printf("l: %u c: %u it: %u page: %u \n",l,c,it,page);
			VMinima(lcd);
		}
		else{
			Serial.printf("l: %u c: %u it: %u page: %u \n",l,c,it,page);
			VMinima(lcd);
		}
  	}
	//  -===RIGTH===-
	if(analogRead(eixoX)>3000){
		if(cr[c]<18){
			c++;
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
			VMinima(lcd);
		}
		else{
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
			VMinima(lcd);
		}
	}
	//  -===LEFT===-
	if(analogRead(eixoX)<1000){
		if(cr[c]>2){
			c--;
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
			VMinima(lcd);
		}
		else{
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
			VMinima(lcd);
		}
	}
}

// void Ventilacao::DifOnOff(LiquidCrystal_I2C lcd, short l, short c){
void Ventilacao::DifOnOff(LiquidCrystal_I2C lcd){
	short cr[] = {7,8,10,15,16,18};
	lcd.clear();
	lcd.setCursor(0,0); lcd.printf("|-  DIF. LIG/DES  -|");
	lcd.setCursor(8,1); lcd.print("LIG  |  DES");
	lcd.setCursor(0,2); lcd.printf("Dif.   %04.1fC | %04.1fC",dif[0],dif[1]);
	do{
		key = keypad.getKey();
		lcd.setCursor(cr[c],l); lcd.blink();
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
					char aux[4];
					short a;
					if(c>=0 && c<=2){ // LIGa
						if(c==2) a=c+1; else a=c;
						sprintf(aux,"%04.1f",dif[0]);
						Serial.printf("aux (String): %s\n",aux);
						aux[a]=key;
						Serial.printf("aux (%ux%u): %s\n",c,l,aux);
						dif[0] = atof(aux);
						Serial.printf("=> Dif[0] (float): {%f}\n",dif[0]);
					}else if(c>=3 && c<=5){ //DESLIGA
						if(c==5) a=c-2; else a=c-3;
						sprintf(aux,"%04.1f",dif[1]);
						Serial.printf("aux (String): %s\n",aux);
						aux[a]=key;
						Serial.printf("aux (%ux%u): %s\n",c,l,aux);
						dif[1] = atof(aux);
						Serial.printf("=> Dif[1] (float): {%f}\n",dif[1]);
					}
					lcd.print(key);
					break;
				case '*':
					lcd.noCursor(); //blink_off();
					lcd.clear();
					l=2;
					c=0;
					page=1;
          			return;
			}
    	}
	} while (analogRead(eixoX)>1000 && 
			 analogRead(eixoX)<3000);
	

	//  -===RIGTH===-
	if(analogRead(eixoX)>3000){
		if(cr[c]<18){
			c++;
			Serial.printf("l: %u c: %u\n",l,c);
			DifOnOff(lcd);
		}
		else{
			Serial.printf("l: %u c: %u\n",l,c);
			DifOnOff(lcd);
		}
	}
	//  -===LEFT===-
	if(analogRead(eixoX)<1000){
		if(cr[c]>7){
			c--;
			Serial.printf("l: %u c: %u\n",l,c);
			DifOnOff(lcd);
		}
		else{
			Serial.printf("l: %u c: %u\n",l,c);
			DifOnOff(lcd);
		}
	}
}

// void Ventilacao::ConfGrupos(LiquidCrystal_I2C lcd, short page, short l, short c){
void Ventilacao::ConfGrupos(LiquidCrystal_I2C lcd){
	short cr[] = {7,8,10,15,16,18};
	lcd.clear();
	lcd.setCursor(0,0); lcd.printf("|- AJ. GRUPOS -|%u/2|",page);
	lcd.setCursor(0,1); lcd.print("GRUPO|  LIG  |  DES");
	switch (page){
		case 1:
			lcd.setCursor(0,2); lcd.printf(" G%02u | %04.1fC | %04.1fC",(page-1),grupos[page-1][0],grupos[page-1][1]);
			lcd.setCursor(0,3); lcd.printf(" G%02u | %04.1fC | %04.1fC",(page),grupos[page][0],grupos[page][1]);
			break;
		case 2:
			lcd.setCursor(0,2); lcd.printf(" G%02u | %04.1fC | %04.1fC",(page),grupos[page][0],grupos[page][1]);
			lcd.setCursor(0,3); lcd.printf(" G%02u | %04.1fC | %04.1fC",(page+1),grupos[page+1][0],grupos[page+1][1]);
			break;
	
	// Para exaustores, mais cases devem ser considerados
	// case 3:
	// 	lcd.setCursor(0,2); lcd.printf("%u %02u | %04us | %04us",(page+1),vm[page+1][0],vm[page+1][1],vm[page+1][2]);
    // 	lcd.setCursor(0,3); lcd.printf("%u %02u | %04us | %04us",(page+2),vm[page+2][0],vm[page+2][1],vm[page+2][2]);
	// 	break;
	}
	do{
		key = keypad.getKey();
		lcd.setCursor(cr[c],l); lcd.blink();
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
					GravaTemp();
					lcd.print(key);
					break;
				case '*':
					lcd.noCursor(); //blink_off();
					lcd.clear();
					it=3;
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
			ConfGrupos(lcd);
		}
		else{
			Serial.printf("l: %u c: %u it: %u page: %u \n",l,c,it,page);
			ConfGrupos(lcd);
		}
	}
	//  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
		if(page<2 || l<3){
			l++;
			it++;
			if(l==4){l=2;page++;}
			Serial.printf("l: %u c: %u it: %u page: %u \n",l,c,it,page);
			ConfGrupos(lcd);
		}
		else{
			Serial.printf("l: %u c: %u it: %u page: %u \n",l,c,it,page);
			ConfGrupos(lcd);
		}
  	}
	//  -===RIGTH===-
	if(analogRead(eixoX)>3000){
		if(cr[c]<18){
			c++;
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
			ConfGrupos(lcd);
		}
		else{
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
			ConfGrupos(lcd);
		}
	}
	//  -===LEFT===-
	if(analogRead(eixoX)<1000){
		if(cr[c]>7){
			c--;
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
			ConfGrupos(lcd);
		}
		else{
			Serial.printf("l: %u c: %u page: %u \n",l,c,page);
			ConfGrupos(lcd);
		}
	}
}

void Ventilacao::GravaTemp(){
	char aux[5];
	short a;
	if(l==2){ 
		if(c>=0 && c<=2){ //LIGA
			if(c==2) a=c+1; else a=c;
			sprintf(aux,"%04.1f",grupos[(it-1)][0]);
			Serial.printf("aux (String): %s\n",aux);
			aux[a]=key;
			Serial.printf("aux (%ux%u): %s\n",c,l,aux);
			grupos[(it-1)][0] = atof(aux);
			Serial.printf("=> grupos[%u][0] (float): {%f}\n",(it-1),grupos[(it-1)][0]);
		}else if(c>=3 && c<=5){ //DESLIGA
			if(c==5) a=c-2; else a=c-3;
			sprintf(aux,"%04.1f",grupos[(it-1)][1]);
			Serial.printf("aux (String): %s\n",aux);
			aux[a]=key;
			Serial.printf("aux (%ux%u): %s\n",c,l,aux);
			grupos[(it-1)][1] = atof(aux);
			Serial.printf("=> grupos[%u][1] (float): {%f}\n",(it-1),grupos[(it-1)][1]);
		}
	} else{
		if(c>=0 && c<=2){ //LIGA
			if(c==2) a=c+1; else a=c;
			sprintf(aux,"%04.1f",grupos[(it-1)][0]);
			Serial.printf("aux (String): %s, a:%u\n",aux,a);
			aux[a]=key;
			Serial.printf("aux (%ux%u): %s\n",c,l,aux);
			grupos[(it-1)][0] = atof(aux);
			Serial.printf("=> grupos[%u][0] (float): {%f}\n",(it-1),grupos[(it-1)][0]);
		}else if(c>=3 && c<=5){ //DESLIGA
			if(c==5) a=c-2; else a=c-3;
			sprintf(aux,"%04.1f",grupos[(it-1)][1]);
			Serial.printf("aux (String): %s\n",aux);
			aux[a]=key;
			Serial.printf("aux (%ux%u): %s\n",c,l,aux);
			grupos[(it-1)][1] = atof(aux);
			Serial.printf("=> grupos[%u][1] (float): {%f}\n",(it-1),grupos[(it)][1]);
		}
	}
}

void Ventilacao::GravaVM(){
	char aux[5];
	short a;
	if(l==2){ 
		if(c==0 || c==1){ //IDADE
			sprintf(aux,"%02u",vm[(it-1)][0]);
			Serial.printf("aux (String): %s\n",aux);
			aux[c]=key;
			Serial.printf("aux (%ux%u): %s\n",c,l,aux);
			vm[(it-1)][0] = atoi(aux);
			Serial.printf("=> vm[%u][0] (float): {%u}\n",(it-1),vm[(it-1)][0]);
		}else if(c>=2 && c<=5){ //LIGA
			a=c-2;
			sprintf(aux,"%04u",vm[(it-1)][1]);
			Serial.printf("aux (String): %s\n",aux);
			aux[a]=key;
			Serial.printf("aux (%ux%u): %s\n",c,l,aux);
			vm[(it-1)][1] = atoi(aux);
			Serial.printf("=> vm[%u][1] (float): {%u}\n",(it-1),vm[(it-1)][1]);
		} else if(c>=6 && c<=9){ // DESLIGA
			a=c-6;
			sprintf(aux,"%04u",vm[(it-1)][2]);
			Serial.printf("aux (String): %s\n",aux);
			aux[a]=key;
			Serial.printf("aux (%ux%u): %s\n",c,l,aux);
			vm[(it-1)][2] = atoi(aux);
			Serial.printf("=> vm[%u][2] (float): {%u}\n",(it-1),vm[(it-1)][2]);
		}
	}

	else{
		if(c==0 || c==1){ //IDADE
			sprintf(aux,"%02u",vm[(it-1)][0]);
			Serial.printf("aux (String): %s\n",aux);
			aux[c]=key;
			Serial.printf("aux (%ux%u): %s\n",c,l,aux);
			vm[(it-1)][0] = atoi(aux);
			Serial.printf("=> vm[%u][0] (float): {%u}\n",(it-1),vm[(it-1)][0]);
		}else if(c>=2 && c<=5){ //LIGA
			a=c-2;
			sprintf(aux,"%04u",vm[(it-1)][1]);
			Serial.printf("aux (String): %s\n",aux);
			aux[a]=key;
			Serial.printf("aux (%ux%u): %s\n",c,l,aux);
			vm[(it-1)][1] = atoi(aux);
			Serial.printf("=> vm[%u][1] (float): {%u}\n",(it-1),vm[(it-1)][1]);
		}else if(c>=6 && c<=9){ // DESLIGA
			a=c-6;
			sprintf(aux,"%04u",vm[(it-1)][2]);
			Serial.printf("aux (String): %s\n",aux);
			aux[a]=key;
			Serial.printf("aux (%ux%u): %s\n",c,l,aux);
			vm[(it-1)][2] = atoi(aux);
			Serial.printf("=> vm[%u][2] (float): {%u}\n",(it-1),vm[(it-1)][2]);
		}
	}
}