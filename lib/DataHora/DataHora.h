/*
 * Data.h
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#ifndef LIB_DATAHORA_DATAHORA_H_
#define LIB_DATAHORA_DATAHORA_H_

#include "time.h"

class DataHora {
	private:
		struct tm data;
		timer_t data_t;

	public:
		DataHora();
		DataHora(int day, int month, int year);
		virtual ~DataHora();

		int getAno() const;
		void setAno(int ano);

		int getDia() const;
		void setDia(int dia);

		int getMes() const;
		void setMes(int mes);

		timer_t getDataT() const;

		void validacaoData();

		struct tm& getData();

		void setDataT(struct tm &data);
};

#endif /* LIB_DATA_DATA_H_ */
