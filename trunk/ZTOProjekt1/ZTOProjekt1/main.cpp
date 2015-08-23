#include <iostream>
#include <fstream>
#include <ctime>
#define DEBUG 0
#include "Digraph.h"


using namespace std;


class A {
	
public : 
	A(const A& a){
		throw(Exception("chuj"));
	}
	A(){}

};


int main()
{
	srand((unsigned int)time(NULL));
	//srand(2);
	Digraph<VertexBase> myDigraph;

	// ExcThrower<Exception>::Throw(0.6, "Wyjatek z pawdopodobnienstwem 0.6");
	
	myDigraph.generate(100);

	cout << myDigraph << endl ;
	
	ofstream output("../graf.txt");

	if( output.is_open() )
	{
		while (myDigraph.shortenCriticalPath()){
			cout << myDigraph << endl;						
			try{
				myDigraph.SaveStructure(output);
			}catch(FileWriteException& fwEx)
			{
				cout << fwEx.getMessage() << " numer wyjatku " << fwEx.Count() << endl;			
			}	
		}
		output.close();		
	}

	system("pause");
 return 0;   
}