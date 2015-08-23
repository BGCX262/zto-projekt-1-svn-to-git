#pragma once

#include <string>

#include "Countable.h"
#include "Utils.h"
using namespace std;


template<class Exc>
class ExcThrower {
public:
	static void Throw(float probabilty, string message){
		if( TRUE_PROB(probabilty) )
			throw Exc(message); 
	}
};


class Exception : public Countable<Exception>{
protected :
	string message;
public :
	Exception(string mes = ""):message(mes){
		
	}

	string getMessage(){
		return message;
	}

};

class FileWriteException : public Exception, public  Countable<FileWriteException>
{	
public:
	using Countable<FileWriteException>::Count;

	FileWriteException(string message = ""):Exception(message){}
};

class DatabaseException : public Exception, public Countable<DatabaseException>{
	
public :
	using Countable<DatabaseException>::Count;
	DatabaseException(string message = "") : Exception(message){}

};


class CopyException : public Exception, public Countable<CopyException>{
	
public :
	using Countable<CopyException>::Count;
	CopyException(string message = "") : Exception(message){}

};
