#pragma once
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Task.h"
#include "Exception.h"


class VertexBase {

protected:

	

public :
	Task task;

	// gwarancja silna
	VertexBase(){}

	// gwarancja nie zwracania wyjatk�w
	~VertexBase(){}

	// konstruktor kopiuj�cy, gwarancja silna rzucaj�ca wyjatkami 
	// z prawdopodobie�stwem p(zasymulowac)
	VertexBase(const VertexBase & other ){ 
		this->task = other.task;
		ExcThrower<CopyException>::Throw( PROBABILTY_Q, "Blad w konstrukorze kopiujacym" );		
	}

	// operator przypisania, gwarancja silna rzucaj�ca wyjatkami 
	// z prawdopodobie�stwem p(zasymulowac)
	VertexBase & operator= (const VertexBase & v){ 		
		this->task = v.task; 
		ExcThrower<CopyException>::Throw( PROBABILTY_Q, "Blad w operatorze przypisania" );
		return *this;
	}

	// metoda pobiera zadanie o podanym id
	// gwarancja silna 
	// rzucanie wyjatk�w z prawdopodobienstwem q(symulacja przez pliki, 
	// sens:  b��d komunikacji z baza danych )u
	void PobierzDane(int task_id) throw (DatabaseException) {
		ostringstream stask;
		stask << "Blad komunikacji z bazu przy pobieraniu zadania o numerze " << task_id;
		string message = stask.str();
		ExcThrower<DatabaseException>::Throw( PROBABILTY_Q, message );
		task.id = task_id;
		task.hours = (float)(rand()%10) + 1;
		task.persons = (rand()%10) + 1;
	}

	friend ostream& operator<<(ostream&, VertexBase&);	

};

ostream& operator<<(ostream& os, VertexBase& vb){	
	return os << vb.task;
}