#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

//Storing constants
const int Ntel = 2; 		//number of telephonists
const int Ncook = 2; 		//number of cooks
const int Noven = 10; 		//number of ovens
const int Ndeliverer = 10; 	//number of deliverers

const int TorderLow = 1; 	//range of possible time for a new order arival
const int TorderHigh = 5;

const int NorderLow = 1; 	//range of possible number of pizzas in each order
const int NorderHigh = 5; 

const int Pmargarita = 35; 	//percentage for ordering chance for every pizza type
const int Ppepperoni = 25; 	
const int Pspecial = 40; 	

const int TpaymentLow = 1; 	//range of possible time for a telephonist 
const int TpaymentHigh = 3; //to charge the customer's credit card

const int Pfail = 5;		//percentage for charge failure and order cancellation

const int Cmargarita = 10; 	//price for a margarita pizza
const int Cpepperoni = 11; 	//price for a pepperoni pizza
const int Cspecial = 12; 	//price for a special pizza

const int Tprep = 1; 		//preparation time for each pizza
const int Tbake = 10;		//baking time for each pizza/order
const int Tpack = 1;		//packing time for each order

const int TdelLow = 5;		//range of possible time for an order delivery
const int TdelHigh = 15;

//Declaration of variables
unsigned int Ncust;
unsigned int seed;

int availableTels;
int availableCooks;
int availableOvens;
int availableDeliverers;

int totalEarnings;
int successfulOrders;
int failedOrders;

int totalMargaritas;
int totalPepperonis;
int totalSpecials;

int totalDeliveryTime;
int maxDeliveryTime;

int totalColdTime;
int maxColdTime;


//Declaration of mutexes
pthread_mutex_t mutex_availableTels;
pthread_mutex_t mutex_availableCooks;
pthread_mutex_t mutex_availableOvens;
pthread_mutex_t mutex_availableDeliverers;

pthread_mutex_t mutex_totalEarnings;
pthread_mutex_t mutex_successfulOrders;
pthread_mutex_t mutex_failedOrders;

pthread_mutex_t mutex_totalMargaritas;
pthread_mutex_t mutex_totalPepperonis;
pthread_mutex_t mutex_totalSpecials;

pthread_mutex_t mutex_totalDeliveryTime;
pthread_mutex_t mutex_maxDeliveryTime;

pthread_mutex_t mutex_totalColdTime;
pthread_mutex_t mutex_maxColdTime;

pthread_mutex_t mutex_lockScreen;

//Declaration of conditions
pthread_cond_t cond_availableTels;
pthread_cond_t cond_availableCooks;
pthread_cond_t cond_availableOvens;
pthread_cond_t cond_availableDeliverers;


void error_check(char err[],int rc){ 
	if (rc!=0) {
		printf("ERROR: return code from %s is %d\n", err, rc);
		exit(-1);
	}
}

void initMutexesAndConditionals() { 
	int rc = pthread_mutex_init(&mutex_availableTels, NULL);
	error_check("pthread_mutex_init", rc);
	rc = pthread_mutex_init(&mutex_availableCooks, NULL);
	error_check("pthread_mutex_init", rc);
	rc = pthread_mutex_init(&mutex_availableOvens, NULL);
	error_check("pthread_mutex_init", rc);
	rc = pthread_mutex_init(&mutex_availableDeliverers, NULL);
	error_check("pthread_mutex_init", rc);
	
	rc = pthread_mutex_init(&mutex_totalEarnings, NULL);
	error_check("pthread_mutex_init", rc);
	rc = pthread_mutex_init(&mutex_successfulOrders, NULL);
	error_check("pthread_mutex_init", rc);
	rc = pthread_mutex_init(&mutex_failedOrders, NULL);
	error_check("pthread_mutex_init", rc);
	
	rc = pthread_mutex_init(&mutex_totalMargaritas, NULL);
	error_check("pthread_mutex_init", rc);
	rc = pthread_mutex_init(&mutex_totalPepperonis, NULL);
	error_check("pthread_mutex_init", rc);
	rc = pthread_mutex_init(&mutex_totalSpecials, NULL);
	error_check("pthread_mutex_init", rc);
	
	rc = pthread_mutex_init(&mutex_totalDeliveryTime, NULL);
	error_check("pthread_mutex_init", rc);
	rc = pthread_mutex_init(&mutex_maxDeliveryTime, NULL);
	error_check("pthread_mutex_init", rc);
	
	rc = pthread_mutex_init(&mutex_totalColdTime, NULL);
	error_check("pthread_mutex_init", rc);
	rc = pthread_mutex_init(&mutex_maxColdTime, NULL);
	error_check("pthread_mutex_init", rc);
	
	rc = pthread_mutex_init(&mutex_lockScreen, NULL);
	error_check("pthread_mutex_init", rc);
	
	rc = pthread_cond_init(&cond_availableTels, NULL); 
	error_check("pthread_cond_init", rc);
	rc = pthread_cond_init(&cond_availableTels, NULL); 
	error_check("pthread_cond_init", rc);
	rc = pthread_cond_init(&cond_availableCooks, NULL); 
	error_check("pthread_cond_init", rc);
	rc = pthread_cond_init(&cond_availableOvens, NULL); 
	error_check("pthread_cond_init", rc);
	rc = pthread_cond_init(&cond_availableDeliverers, NULL); 
	error_check("pthread_cond_init", rc);
}

void destroyMutexesAndConditionals() { 
	int rc = pthread_mutex_destroy(&mutex_availableTels);
	error_check("pthread_mutex_destroy", rc);
	rc = pthread_mutex_destroy(&mutex_availableCooks);
	error_check("pthread_mutex_destroy", rc);
	rc = pthread_mutex_destroy(&mutex_availableOvens);
	error_check("pthread_mutex_destroy", rc);
	rc = pthread_mutex_destroy(&mutex_availableDeliverers);
	error_check("pthread_mutex_destroy", rc);
	
	rc = pthread_mutex_destroy(&mutex_totalEarnings);
	error_check("pthread_mutex_destroy", rc);
	rc = pthread_mutex_destroy(&mutex_successfulOrders);
	error_check("pthread_mutex_destroy", rc);
	rc = pthread_mutex_destroy(&mutex_failedOrders);
	error_check("pthread_mutex_destroy", rc);
	
	rc = pthread_mutex_destroy(&mutex_totalMargaritas);
	error_check("pthread_mutex_destroy", rc);
	rc = pthread_mutex_destroy(&mutex_totalPepperonis);
	error_check("pthread_mutex_destroy", rc);
	rc = pthread_mutex_destroy(&mutex_totalSpecials);
	error_check("pthread_mutex_destroy", rc);
	
	rc = pthread_mutex_destroy(&mutex_totalDeliveryTime);
	error_check("pthread_mutex_destroy", rc);
	rc = pthread_mutex_destroy(&mutex_maxDeliveryTime);
	error_check("pthread_mutex_destroy", rc);
	
	rc = pthread_mutex_destroy(&mutex_totalColdTime);
	error_check("pthread_mutex_destroy", rc);
	rc = pthread_mutex_destroy(&mutex_maxColdTime);
	error_check("pthread_mutex_destroy", rc);
	
	rc = pthread_mutex_destroy(&mutex_lockScreen);
	error_check("pthread_mutex_destroy", rc);
	
	rc = pthread_cond_destroy(&cond_availableTels); 
	error_check("pthread_cond_destroy", rc);
	rc = pthread_cond_destroy(&cond_availableTels); 
	error_check("pthread_cond_destroy", rc);
	rc = pthread_cond_destroy(&cond_availableCooks); 
	error_check("pthread_cond_destroy", rc);
	rc = pthread_cond_destroy(&cond_availableOvens); 
	error_check("pthread_cond_destroy", rc);
	rc = pthread_cond_destroy(&cond_availableDeliverers); 
	error_check("pthread_cond_destroy", rc);
}
