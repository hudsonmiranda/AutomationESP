/*
 * Data.h
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#ifndef LIB_DATA_DATA_H_
#define LIB_DATA_DATA_H_

class Data {
	private:
		int dia,
			mes,
			ano;

	public:
		Data();
		Data(int day, int month, int year);
		virtual ~Data();

		int getAno();
		void setAno(int ano);

		int getDia();
		void setDia(int dia);

		int getMes();
		void setMes(int mes);
};

#endif /* LIB_DATA_DATA_H_ */
