/*
 * Temperatura.h
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#ifndef LIB_TEMPERATURA_TEMPERATURA_H_
#define LIB_TEMPERATURA_TEMPERATURA_H_

#include <Arduino.h>

class Temperatura {
	private:
//		int 	modo, interator;
//		float 	configTemperatura [10][2],
//				max,
//				min;
	public:
		short 	modo,
		 		interator;
		short	cursor[2];
		float 	maximo = 2,
				minimo = 2,
				configTemperatura [12][2] ={{0,31.5},
											{1,30.5},
											{4,28.5},
											{6,28},
											{8,27.5},
											{12,27},
											{16,26.7},
											{20,26.3},
											{26,25.7},
											{32,25.3},
											{35,25},
											{38,24.5}};

		String 	menu[3] =   {"Modo:",
					  		 "Ajuste de Temp.",
							 "Definir Max. e Min."},
				modos[3] =  {"Temp Desejada",
							 "Curva de Temp",
							 "Zona Entalpia"},
				title = "  -|TEMPERATURA|-   ";


		Temperatura();
		virtual ~Temperatura();

		const float* getConfTemperatura() const;
		float getMax() const;
		void setMax(float max);
		float getMin() const;
		void setMin(float min);
		int getModo() const;
		void setModo(int modo);
		const String* getModos() const;
};

#endif /* LIB_TEMPERATURA_TEMPERATURA_H_ */
