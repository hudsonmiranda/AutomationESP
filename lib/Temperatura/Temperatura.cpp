/*
 * Temperatura.cpp
 *
 *  Created on: Sep 20, 2020
 *      Author: hudsonm
 */

#include <Arduino.h>
#include "Temperatura.h"

//Descrtuctor
Temperatura::~Temperatura() {}
//Constructor
Temperatura::Temperatura() {
	this->modo = 0;
	// this->max = 2;
	// this->min = 2;
	this->interator = 1;
}

//const float* Temperatura::getConfTemperatura() const {
//	return configTemperatura;
//}

float Temperatura::getMax() const {
	return maximo;
}

void Temperatura::setMax(float max) {
	this->maximo = max;
}

float Temperatura::getMin() const {
	return minimo;
}

void Temperatura::setMin(float min) {
	this->minimo = min;
}

int Temperatura::getModo() const {
	return modo;
}

void Temperatura::setModo(int modo) {
	this->modo = modo;
}

const String* Temperatura::getModos() const {
	return modos;
}
