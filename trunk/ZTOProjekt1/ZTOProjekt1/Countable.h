

template <typename T> 
class Countable{

	static int counter;

public:
	
	Countable(){counter++;}
	virtual int Count() const {
		return counter;
	}
};

template<typename T>
int Countable<T>::counter = 0;