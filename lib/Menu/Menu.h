/*
 * Menu.h
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#ifndef LIB_MENU_MENU_H_
#define LIB_MENU_MENU_H_

#include <vector>
//#include <iostream>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
//#include "DataHora.h"

class Menu {
	public:
		std::vector<String> menu;
		int		indicator=1,
				menu_enum=0;
		// Menu();
		Menu(String list[]);
		virtual ~Menu();

		void listMenu(LiquidCrystal_I2C lcd, String title);
		void selectMenu(int index);
		void loopMenu(LiquidCrystal_I2C lcd, String title, int eixoX, int eixoY, int switchJoystick);
		void readJoystick();

		int getIndicator() const;
		const String*& getMenu() const;
		int getMenuEnum() const;

		void setIndicator(int indicator);

		void setMenu(const std::vector<String> &menu);

		void setMenuEnum(int menuEnum);
};

#endif /* LIB_MENU_MENU_H_ */
