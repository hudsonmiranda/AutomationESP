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

Menu::Menu(String list[]) {
	for (int var = 0; var < list->length(); var++) {
		menu.push_back(list[var]);
	}
}

void Menu::loopMenu(LiquidCrystal_I2C lcd, String title, int eixoX, int eixoY, int switchJoystick){
	lcd.clear();
	lcd.setCursor(0,indicator); lcd.print(">");//lcd.write(0);
	Serial.printf("Send Up Menu, menu_num: %d, indicator: %d, size: %d\n",menu_enum,indicator,menu.size());
	for (int var = 0; var < menu.size(); var++) {
		Serial.printf(menu[var]);
	}

	do{
		listMenu(lcd, title);
	} while(analogRead(eixoX)>1000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000 && digitalRead(switchJoystick)!=0);
	// } while(analogRead(eixoX)>1000 && analogRead(eixoX)<3000 && analogRead(eixoY)>1000 && analogRead(eixoY)<3000 && digitalRead(switchJoystick)!=0);

//	readJoystick();
	int size = menu.size();//sizeof(menu)/sizeof(menu[0])-1;

		// Navigation Top
		if(analogRead(eixoY)<1000){
//			Serial.printf("\n- - - - - - - - - - - - - -\n||  TOP ||\n----------\n");
//			Serial.printf("analogRead eixoY 33: %d\n",analogRead(eixoY));
//			Serial.printf("Sizeof Menu: %d\n",size);
//			Serial.printf("menu_num: %d\n- - - - - - - - - - - - - -|\n",menu_enum);
//			Serial.printf("Menu option: %s\n",menulist[menu_num]);
			if(menu_enum>0){
			  Serial.printf("menu_num is: %d, and indicator is: %d\n",menu_enum, indicator);
			  menu_enum--;
			  indicator--;

			  if(indicator==-1) indicator=3;

			  Serial.printf("Send Up Menu, menu_num: %d, indicator: %d\n",menu_enum,indicator);
			  loopMenu(lcd, title, eixoX, eixoY, switchJoystick);
			}
			else{
			  Serial.printf("Begin Menu!");
			  loopMenu(lcd, title, eixoX, eixoY, switchJoystick);
			}
		}

		// Navigation Bottom
		if(analogRead(eixoY)>3000){
//			Serial.printf("\n- - - - - - - - - - - - - -\n||BOTTOM||\n----------\n");
//			Serial.printf("analogRead eixoY 33: %d\n",analogRead(eixoY));
//			Serial.printf("Sizeof Menu: %d\n",size);
//			Serial.printf("menu_num: %d\n- - - - - - - - -\n",menu_num);
//			Serial.printf("Menu option: %s\n",menulist[menu_num]);
			if(menu_enum<size){
//			  Serial.printf("menu_num is: %d, and indicator is: %d\n",menu_enum, indicator);
//			  setMenuEnum(menu_enum++);
//			  setIndicator(indicator++);
				indicator++;
				menu_enum++;

			  if(indicator==4) indicator=0;

//			  Serial.printf("Send Down Menu, menu_num: %d, indicator: %d\n- - - - - - - - - - - - - -\n",menu_enum, indicator);
			  loopMenu(lcd, title, eixoX, eixoY, switchJoystick);
			}
			else{
			  Serial.printf("End Menu!");
			  loopMenu(lcd, title, eixoX, eixoY, switchJoystick);
			}
		}

		// Escape User
		if(analogRead(eixoX)<1000){
//			Serial.printf("\n General Left → Break\n- - - - - - - - - - - - - -\n");
//			Serial.printf("analogRead eixoX 32: %d\n",analogRead(eixoX));
			menu_enum=0;
			indicator=0;
			lcd.clear();
			return;
		}
		// Select menu option
		// if(analogRead(eixoX)>3000){
		//   Serial.printf("Right → Select option: %d\n- - - - - - - - - - - - - -\n",menu_num);
		//   Serial.printf("analogRead eixoX 32: %d\n",analogRead(eixoX));
		//   select_menu(menu_num);
		// }
		if(digitalRead(switchJoystick)==0){
//			Serial.printf("Select option: %d\n- - - - - - - - - - - - - -\n",menu_enum);
//			Serial.printf("digitalRead switchJoystick 14: %d\n",digitalRead(switchJoystick));
			selectMenu(menu_enum);
		}

		//End read Joystick


	lcd.clear();
}

void Menu::listMenu(LiquidCrystal_I2C lcd, String title){
	int i=0;
//	if(menu_num<=3) i=0;
	if(menu_enum>3 && menu_enum<=7) i+=4;
	else if(menu_enum>7) i+=8;

	lcd.setCursor(0,0); lcd.print(title);
	lcd.setCursor(1,1); lcd.print(menu[i]);
	lcd.setCursor(1,2); lcd.print(menu[i+1]);
	lcd.setCursor(1,3); lcd.print(menu[i+2]);
//	lcd.setCursor(1,3); lcd.print(menu[i+3]);
}

void Menu::selectMenu(int index){
	switch (index) {
		case 0:

			break;
		case 1:

			break;
		case 2:

			break;
		case 3:

			break;
		case 4:

			break;
		case 5:

			break;
		case 6:

			break;
		case 7:

			break;
		case 8:

			break;
		case 9:

			break;
		default:
			break;
	}
}



void Menu::readJoystick(){

}

int Menu::getIndicator() const {
	return indicator;
}

void Menu::setIndicator(int indicator) {
	this->indicator = indicator;
}

void Menu::setMenuEnum(int menuEnum) {
	this->menu_enum = menuEnum;
}

int Menu::getMenuEnum() const {
	return menu_enum;
}

void Menu::setMenu(const std::vector<String> &menu) {
		this->menu = menu;
	}
