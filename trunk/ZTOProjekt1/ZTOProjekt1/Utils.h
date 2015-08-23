#pragma once



#define RAND( i ) (int)( rand()/(RAND_MAX+1.) * i )

#define TRUE_PROB( i ) (rand()/(RAND_MAX*1.0) < (i) )? true : false


#define PROBABILTY_Q 0.1
#define PROBABILTY_P 0.5
