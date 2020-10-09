/*
 * Menu.h
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#ifndef LIB_MENU_MENU_H_
#define LIB_MENU_MENU_H_

// #include <vector>
#include <Navegacao.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
//#include "DataHora.h"

class Menu : public Navegacao {
	public:
		// short l, itMenu;
		String menulist[9] ={"1. Data e Hora     "
							,"2. Data Alojamento "
							,"3. Temperatura     "
							,"4. Umidade         "
							,"5. Ventilacao      "
							,"6. Nebulizacao     "
							,"7. Aquecedor       "
							,"8. Temporizador    "
							,"9. Relatorio       "};
		
		Menu();
		virtual ~Menu();

		// void listMenu(LiquidCrystal_I2C lcd);
		// void selectMenu(int index);
		void loopMenu(LiquidCrystal_I2C lcd);
		// void readJoystick();

		// int getIndicator() const;
		// const String*& getMenu() const;
		// int getMenuEnum() const;

		// void setIndicator(int indicator);
		// void setMenu(const std::vector<String> &menu);
		// void setMenuEnum(int menuEnum);
		void NavEixoY(short sc);
};

#endif /* LIB_MENU_MENU_H_ */
