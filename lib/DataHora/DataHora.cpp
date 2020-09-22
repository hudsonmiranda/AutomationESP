/*
 * Data.cpp
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#include "DataHora.h"

#include "time.h"


DataHora::DataHora() {
	this->data.tm_mday = 9;
	this->data.tm_mon = 9-1;
	this->data.tm_year = 2020-1900;
	data_t=mktime(&data);
}
DataHora::DataHora(int day, int month, int year) {
	this->data.tm_mday = day;
	this->data.tm_mon = month-1;
	this->data.tm_year = year-1900;
	data_t=mktime(&data);

}

DataHora::~DataHora() {}

int DataHora::getAno() const {
	return data.tm_year;
}

void DataHora::setAno(int ano) {
	this->data.tm_year = ano;
	data_t=mktime(&data);
}

int DataHora::getDia() const{
	return data.tm_mday;
}

void DataHora::setDia(int dia) {
	this->data.tm_mday = dia;
}

int DataHora::getMes() const {
	return data.tm_mon;
}
void DataHora::setMes(int mes) {
	this->data.tm_mon = mes;
}

struct tm& DataHora::getData() {
	return data; // @suppress("Invalid arguments")
}

timer_t DataHora::getDataT() const {
	return data_t;
}

void DataHora::setDataT(struct tm &data) {
	data_t = mktime(&data);
}
