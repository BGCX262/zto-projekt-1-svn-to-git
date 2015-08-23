#pragma once
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include "VertexBase.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <deque>
#include <list>

#include "Exception.h"
#include "Utils.h"
//# define DEBUG 0
using namespace std;


// return true ew
bool rand(int percent){
	return true;
}

template <class VertexType>
class Digraph {
	
private:
	
	

	// dodajemy id do klasy zadanej parametrem szablonu
	class DigraphVertex : public VertexType {
	public:
		int ID;
		double critical_val;
		DigraphVertex* critical_child;
		DigraphVertex* critical_parent;

		// lista dzieci 
		set< DigraphVertex* > childs;
	
		// lista ojcow
		set< DigraphVertex* > parents;

		DigraphVertex(int id = -1):ID(id){
			dynamic_cast<const VertexBase*>(this);
			bool success;
			do{
				try {
					this->PobierzDane(id);
					success = true;
				}catch(DatabaseException& de)
				{
					cout << de.getMessage() << " numer wyjatku " << de.Count() << endl;
					success = false;
				}
			}while(! success);
			reset_critical_values();
		}		
		bool operator==(DigraphVertex& d){
			return ID == d.ID;
		}
		bool operator!=(DigraphVertex& d){
			return ID != d.ID;
		}
		
		void reset_critical_values(){
		
			critical_child = NULL;
			critical_parent = NULL;
			critical_val = this->task.hours;
		
		}
	};

	
	typedef DigraphVertex DV;

	// wierzcho³ki
	map<int, DigraphVertex > vertexs;

	typedef map<int,DV> vcon;


	DV& operator[](int i){
		return vertexs[i];
	}

	void addEdge(DV& from, DV& to){
		// TODO dodac sprawdzanie zakresów i powtorzen		
		do {
			try {
				from.childs.insert(&to);
				break;
			}catch(CopyException& de){				
				cout << de.getMessage() << " numer wyjatku " << de.Count() << endl;
			}
		}while( true );
		do {
			try{
				to.parents.insert(&from);
				break;
			}catch(CopyException& de)			
			{
				cout << de.getMessage() << " numer wyjatku " << de.Count() << endl;
			}
		}while(true);
		
	}

	void addEdge(int from, int to){
		do {
			try{
				addEdge(vertexs[from],vertexs[to]);
				break;
			}catch(CopyException& de)
			{
				cout << de.getMessage() << " numer wyjatku " << de.Count() << endl;
			}
		}while(true);
	}

	
	bool merge(int parent, int child, double criticalValue){
		if( ! canMerge(parent, child) )
			return false;

		DV* newVertex = NULL;
		do {
			try{
				DV nV(++lastID);
				nV.task = Task::merge(vertexs[parent].task, vertexs[child].task);

				vertexs[nV.ID] = nV;
				newVertex = &vertexs[nV.ID];
				break;
			}catch(CopyException& ce)
			{				
				cout << ce.getMessage() << " numer wyjatku " << ce.Count() << endl;
				vertexs.erase(lastID);
				--lastID;
			}			
		}while(true);

		newVertex->reset_critical_values();

		copyChilds(vertexs[parent],*newVertex);
		copyChilds(vertexs[child],*newVertex);
		
		copyParents(vertexs[parent],*newVertex);
		copyParents(vertexs[child],*newVertex);

		newVertex->childs.erase(&vertexs[child]);
		newVertex->parents.erase(&vertexs[parent]);

		checkEdges(*newVertex);	
		
		// szukamy nowej sciezki krytycznej
		double newCriticalValue;
		reset_critical_values();

		if( source == parent ){
			dfs_funcBefore<func_set_critical_path>(newVertex->ID, escape);
			newCriticalValue = vertexs[escape].critical_val;
		}
		else { 
			if( escape == child ){
				dfs_funcBefore<func_set_critical_path>(source, newVertex->ID);
				newCriticalValue = newVertex->critical_val;
			}
			else {
				dfs_funcBefore<func_set_critical_path>(source, escape);
				newCriticalValue = vertexs[escape].critical_val;
			}
		}

		// czy sie poprawilo

		if( newCriticalValue > criticalValue ){ // jest gorzej
			erase(newVertex->ID);
			return false;
		} else {
			erase(parent);
			erase(child);
			
			if( source == parent )
				source = newVertex->ID;
			
			if( escape == child )
				escape = newVertex->ID;

			return true;
		}
	}

	void checkEdges(DV& a){
		for(set<DV*>::iterator it = a.parents.begin();
			it != a.parents.end();
			++it)
			(**it).childs.insert(&a);

		for(set<DV*>::iterator it = a.childs.begin();
				it != a.childs.end();
				++it)
				(**it).parents.insert(&a);
	
	}
	
	void erase(int id){
		DV* actual = &vertexs[id];
		for( set<DV*>::iterator it = actual->parents.begin();
			it != actual->parents.end();
			++it){
				(**it).childs.erase(actual);
		}

		for( set<DV*>::iterator it = actual->childs.begin();
			it != actual->childs.end();
			++it){
			(**it).parents.erase(actual);
		}

		vertexs.erase(id);
	}

	void copyChilds(DV& from, DV& to){
		for(set<DV*>::iterator it = from.childs.begin();
			it != from.childs.end();
			++it)
			to.childs.insert(*it);

	}
	void copyParents(DV& from, DV& to){
		for(set<DV*>::iterator it = from.parents.begin();
			it != from.parents.end();
			++it)
			to.parents.insert(*it);
	}



	vector<int> visited;

	// mozna polaczyc wierzcholki jesli jest miedzy nimi tylko jedna sciezka
	bool canMerge(int from, int to ){
		visited.clear();
		visited.resize(lastID+1);
		dfs(from,to);
		return visited[to] == 1;
	}

	int source;
	int escape;

	int lastID;

	// graf jest skierowany bez cykli
	void dfs(int start, int end){
		++visited[start];
		if( start == end)
			return;		
		for(set<DV*>::iterator it = vertexs[start].childs.begin();
			it != vertexs[start].childs.end();
			++it){
				if( visited[(**it).ID]<=1 )
					dfs( (**it).ID, end );
			
		}
	}
	// szablon funkcji dfs, func zostanie wywolana przed wejsciem do dfs
	// jej parametry sa typu DV&	
	// perwszy to aktualny wierzcholek drugi to aktualne dziecko
	template<typename func>
	void dfs_funcBefore(int start, int end){
		++counter;
		if(start == end )
			return;
		for(set<DV*>::iterator it = vertexs[start].childs.begin();
			it!=vertexs[start].childs.end();
			++it){
			if(func()(vertexs[start],**it))
				dfs_funcBefore<func>( (*it)->ID, end);
		}
	}
	
	struct func_set_critical_path {
		bool operator()(DV& a,DV& b){
			return set_critical(a,b);
		}
	};

	static bool set_critical(DigraphVertex& parent_vertex, DigraphVertex& child_vertex){

			//  nowa potencjalna wartoœæ krytyczna, pochodz¹ca od parent 
			double new_critical_val = parent_vertex.critical_val + child_vertex.task.hours;

			if (child_vertex.critical_val < new_critical_val) {		// je¿eli nowa krytyczna jest wieksza od starej

				child_vertex.critical_val = new_critical_val;
				child_vertex.critical_parent = &parent_vertex;
				parent_vertex.critical_child = &child_vertex;
				return true;
			}
			return false;

	}

	bool addVertex(int id){
		
		DV v(id);
		//TODO trzeba dodac sprawdzanie czy nie ma tego ID juz
		vertexs[id] = v;
		return true;
	}

	// czysci graf i dodaje nowe wierzcholki w liczbie size ideksowane od 0
	void createVertexs(int size){
		vertexs.clear();				
		for(int i=0;i<size;++i){
			do{
				try{				
					vertexs[i] = DV(i);			
					break;
				}catch(CopyException& de)
				{
					cout << de.getMessage() << " numer wyjatku " << de.Count() << endl;				
					vertexs.erase(i);
				}
			}while(true);
		}
		lastID = vertexs.size() - 1;
	}

	void printCriticalPath(list<DV*>& result){
		cout << "critical path value: " << (*result.back()).critical_val << endl;
		for(list<DV*>::iterator it = result.begin();
			it != result.end();
			++it)
			cout << (VertexType*)(*it) << " ";
		cout << endl;
	}
public:
	static int counter;
	Digraph():lastID(0){
	}
	
	void SaveStructure(ostream& os)
	{
		ExcThrower<FileWriteException>::Throw( PROBABILTY_P, "B³ad zpapisu do pliku");
		os << *this;
	}

	
	bool shortenCriticalPath(){

		list<DV*> rlist = criticalPath(source, escape);

		double criticalValue = rlist.back()->critical_val;

		if( rlist.size() > 1 ){

			for(list<DV*>::iterator it = rlist.begin();
				it != rlist.end();
				++it){
					list<DV*>::iterator sec(it);
					++sec;
					if( sec != rlist.end() )
						if(merge((**it).ID, (**sec).ID, criticalValue))
							return true;
			}

		}
		return false;
	}
/*
	double actualCriticalPathValue() const {
		return escape->critical_val;
	}
*/
	void reset_critical_values(){
		for(vcon::iterator it = vertexs.begin(); 
			it != vertexs.end();
			++it){
			it->second.reset_critical_values();
		}	
	}
	list<DV*> criticalPath(int start, int end){
		reset_critical_values();
		queue<DV*> que;
		que.push( &vertexs[start] );
		while( que.size() > 0 ){
			++counter;
			DV* actual = que.front();
			que.pop();
			for(set<DV*>::iterator it = actual->childs.begin();
				it != actual->childs.end();
				++it){					
					if( set_critical(*actual, **it) && (*it)->ID != end )
						que.push( *it );	
			}
		}
		list<DV*> result;
		result.push_front(&vertexs[end]);
		while( result.front()->ID != start ){
			DV* actual = result.front();
			result.push_front( actual->critical_parent );
		}
		return result;
	}

	void showCriticalPath(){
		counter = 0;
		list<DV*> result = criticalPath(source, escape);
		printCriticalPath(result);
	}	
	
	double set_critical_path (){
		counter = 0;		
		dfs_funcBefore<func_set_critical_path>(source->ID,escape->ID);		
		return actualCriticalPathValue();		
	}

	int N() const {
		return vertexs.size();
	}


	void generate(int size){
		createVertexs(size);
		int i= 1;
		while(i < size - 1) {							// 0 nie bedzie jako 'from'
			
			int how_many;

			if( !(how_many=rand()%(size - i)))			// losowa liczba z tylu ile zosta³o 
				how_many = 1;


			for( int to = i+1; to < i + how_many;++to){
				// TODO dodac losowoœæ
				if( rand()%100 < 80 )
					addEdge(i,to);
			}
		i++;	
		}
	
		// sprawdz kto nie ma ojca
		for( int v = 1; v < size; ++v){
			do {
				try{
					if( vertexs[v].parents.size() == 0 ){
						addEdge(0,v);					// zmienilem z 1 na 0, bo jak 1 nie ma ojca, to zrobi pare 1 1 
					}
					break;
				}catch(CopyException& de){
					cout << de.getMessage() << " numer wyjatku " << de.Count() << endl;
				}
			}while(true);
		}

		// sprawdz kto nie ma syna
		for( int v = 0;v < size -  1;++v){
			do {
				try{
					if( vertexs[v].childs.size() == 0 )
						addEdge(v,size - 1);
					break;
				}catch(CopyException& de){
					cout << de.getMessage() << " numer wyjatku " << de.Count() << endl;
				}
			}while(true);
				
		}
		source = vertexs[0].ID;
		escape = vertexs[N()-1].ID;
	}



	bool testGenerate(int testsNumber, int graphSize){
		
		for (int i = 0; i < testsNumber; i++){
			visited.clear();
			visited.resize(graphSize);
			generate(graphSize);

			dfs_funcBefore<func_set_critical_path>(vertexs[0].ID,vertexs[graphSize-1].ID);
			for (unsigned i = 0; i < visited.size();i++){
				if(visited[i] == 0)							
					return false;
			}

		}
		return true;
	}

	
	template<class T>
	friend ostream& operator<<(ostream&,Digraph<T>&);

private : 

	
};
template<class D>
int Digraph<D>::counter = 0;

template<class T>
ostream& operator<<(ostream& os, Digraph<T>& d){
	os << d.vertexs.size() << endl;
	for(map<int,Digraph<T>::DV>::iterator it = d.vertexs.begin();
		it != d.vertexs.end();
		++it){
			os << *(T*)(&(it->second)) <<  " -- ";
			for(set<Digraph<T>::DV*>::iterator it2 = it->second.childs.begin();
				it2 != it->second.childs.end();
				++it2){			
			    os << "  " << *(T*)(*it2);
			}
			os << endl;
	}
	return os;
}