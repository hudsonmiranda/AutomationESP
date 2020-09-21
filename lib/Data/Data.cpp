/*
 * Data.cpp
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#include "Data.h"

Data::Data() :dia(9), mes(12), ano(1991) {}
Data::Data(int day, int month, int year) :dia(day), mes(month), ano(year) {}

Data::~Data() {}

int getAno() const {return ano;}
void setAno(int ano) {this->ano = ano;}

int getDia() const {return dia;}
void setDia(int dia) {this->dia = dia;}

int getMes() const {return mes;}
void setMes(int mes) {this->mes = mes;}

