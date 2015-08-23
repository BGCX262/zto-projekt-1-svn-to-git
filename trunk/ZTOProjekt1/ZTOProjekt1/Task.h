#pragma once

#include <iostream>
#include "Utils.h"

using namespace std;

class Task {

public:
	// liczba osob realizujaca zadanie 
	int persons;

	// czas w którym podana liczba osob realizuje zadanie 
	float hours;

	int id;

	Task():persons(-1), hours(-1), id(-1) {}

	static Task merge(const Task& a, const Task& b){
		Task tmp;
		//TODO trzeba by zmienic zeby nowo tworzony id mogl byc rozpoznawalny
		tmp.id = RAND(1000) + 1000;
		tmp.persons = a.persons + b.persons;
		tmp.hours = (a.hours*a.persons  + b.hours*b.persons)/tmp.persons;
		return tmp;
	}

	friend ostream& operator<<(ostream&,Task&);
};

ostream& operator<<(ostream& os, Task& t)
{
	return os << "[(" << t.id << ") " << t.hours << " " << t.persons <<"]";
}