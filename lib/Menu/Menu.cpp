/*
 * Menu.cpp
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */


//#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Menu.h"
#include "DataHora.h"

//Destructor
Menu::~Menu() {}
//Constructor
Menu::Menu() {
	this->l=1;
	this->it=0;
	this->page=1;
}

void Menu::loopMenu(LiquidCrystal_I2C lcd){
	short i=0;
	lcd.clear();
	Serial.printf(" => Menu, it: %d, l: %d, size: %d\n",it,l,(sizeof(menulist)/sizeof(menulist[0])-1));
	if(it>2 && it<=5) i+=3;
	else if(it>5) i+=6;
	lcd.setCursor(0,0); lcd.printf("|-    MENU    -|%u/3|",page);
	lcd.setCursor(1,1); lcd.print(menulist[i]);
	lcd.setCursor(1,2); lcd.print(menulist[i+1]);
	lcd.setCursor(1,3); lcd.print(menulist[i+2]);
	do{
		lcd.setCursor(0,l); lcd.write(B01111110);
		key = keypad.getKey();
		if(key=='*') {lcd.clear();return;}
	} while(analogRead(eixoY)>1000 && analogRead(eixoY)<3000 && digitalRead(switchJoystick)!=0);
	
	//  -===TOP / BOTTOM===-
	if(analogRead(eixoY)>3000 || analogRead(eixoY)<1000){
		NavEixoY(3);// 5 nº Max de Pages
		loopMenu(lcd);
	}
	// -=== SELECT ==-
	if(digitalRead(switchJoystick)==0){
		lcd.clear();
		// selectMenu(it);
	}
}

// void Menu::selectMenu(int index){
// 	switch (index) {
// 		case 0:

// 			break;
// 		case 1:

// 			break;
// 		case 2:

// 			break;
// 		case 3:

// 			break;
// 		case 4:

// 			break;
// 		case 5:

// 			break;
// 		case 6:

// 			break;
// 		case 7:

// 			break;
// 		case 8:

// 			break;
// 		case 9:

// 			break;
// 		default:
// 			break;
// 	}
// }


void Menu::NavEixoY(short sc) {
	//  -===TOP===-
	if(analogRead(eixoY)<1000){
		if(page>1 || l>1){
			l--;
			it--;
			if(l==0) {l=3;page--;}
			Serial.printf("l: %u it: %u page: %u \n",l,it,page);
		}
		else{
			Serial.printf("l: %u it: %u page: %u \n",l,it,page);
		}
	}
	//  -===BOTTOM===-
	if(analogRead(eixoY)>3000){
		if(page<sc || l<3){
			l++;
			it++;
			if(l==4){l=1;page++;}
			Serial.printf("l: %u it: %u page: %u \n",l,it,page);
		}
		else{
			Serial.printf("l: %u it: %u page: %u \n",l,it,page);
		}
  	}
}
