#include "p3190128-pizza.h"

//Invoking order threads
void *Order(void *threadId) {
	int *tid;
	tid = (int *)threadId;
	unsigned int seedp = seed + (*tid);	// Initialize a seed unique for each order

	int Npizzas;						// Number of pizzas for each order
	int Nmargarita;						// Number of margarita pizzas
	int Npepperoni;						// Number of pepperoni pizzas
	int Nspecial;						// Number of special pizzas
	
	int chargingTime;					// Telephonist's charging time
	int preparingTime;					// Cook's preparing time
	int packingTime;					// Deleverer's packing time
	int readyTime;						// Order's time to be ready for delivery 
	int coldTime;						// Pizza(s)'s cold time
	int deliveryTime;					// Deliverer's delivery time
	int deliveredTime;					// Order's time from start till delivery to customer
	
	struct timespec start_time;			// Time when customer arrives
	struct timespec baked_time; 		// Time when pizzas finished baking
	struct timespec packed_time;		// Time when order is ready to be delivered
	struct timespec del_time;			// Time when customer receives the order



	// -------- Waiting Time Section ----------------------------------------------

	// get the time when the customer arrives
	clock_gettime(CLOCK_REALTIME, &start_time);

	int rc = pthread_mutex_lock(&mutex_availableTels);
	error_check("pthread_mutex_lock", rc);

	// wait until you reach an availble telephonist
	while (availableTels < 1) {
		printf("Order with id %d is waiting for available telephonist.\n", *tid);
		rc = pthread_cond_wait(&cond_availableTels, &mutex_availableTels);
		error_check("pthread_cond_wait", rc);
	}
	
	// when you do, decrease the availble telephonists
	--availableTels;
	
	rc = pthread_mutex_unlock(&mutex_availableTels);
	error_check("pthread_mutex_unlock", rc);



	// -------- Talking on the phone Section --------------------------------------	

	printf("Order with id %d is talking on the phone.\n", *tid);

	// find randomly the number of pizzas for this order
	Npizzas = rand_r(&seedp) % NorderHigh + NorderLow;
	
	// choose pizza types
    Nmargarita = (Npizzas * Pmargarita) / 100;
    Npepperoni = (Npizzas * Ppepperoni) / 100;
    Nspecial = Npizzas - Nmargarita - Npepperoni;

	// find randomly how much time the telephonist needs to charge the customer's credit card
	chargingTime = rand_r(&seedp) % TpaymentHigh + TpaymentLow;

	// charge the customer
	sleep(chargingTime);

	// find randomly if the payment was successful
	int paymentSuccess = rand_r(&seed) % 100 + 0 ; 

	// print appropriate message depending on the payment success
	rc = pthread_mutex_lock(&mutex_lockScreen);
	error_check("pthread_mutex_lock", rc);
	
	if (paymentSuccess > Pfail) {
		printf("Order with id %d was payed and registered successfully.\n", *tid);
	}
	else {
		printf("Order with id %d was canceled after payment failure.\n", *tid);
	}
	
	rc = pthread_mutex_unlock(&mutex_lockScreen);
	error_check("pthread_mutex_unlock", rc);

	// release the telephonist 
	rc = pthread_mutex_lock(&mutex_availableTels);
	error_check("pthread_mutex_lock", rc);
	++availableTels;
	rc = pthread_cond_signal (&cond_availableTels);
	error_check("pthread_cond_signal", rc);
	rc = pthread_mutex_unlock(&mutex_availableTels);
	error_check("pthread_mutex_unlock", rc);


	// ------- update statistics --------------------

	// if payment was successful
	if (paymentSuccess > Pfail){
	
		// update total successful orders
		rc = pthread_mutex_lock(&mutex_successfulOrders);
		error_check("pthread_mutex_lock", rc);
		++successfulOrders;
		rc = pthread_mutex_unlock(&mutex_successfulOrders);
		error_check("pthread_mutex_unlock", rc);

		// update total earnings
		rc = pthread_mutex_lock(&mutex_totalEarnings);
		error_check("pthread_mutex_lock", rc);
		totalEarnings += Nmargarita * Cmargarita + Npepperoni * Cpepperoni + Nspecial * Cspecial;
		rc = pthread_mutex_unlock(&mutex_totalEarnings);
		error_check("pthread_mutex_unlock", rc);
		
		// update total margatita pizzas ordered
		rc = pthread_mutex_lock(&mutex_totalMargaritas);
		error_check("pthread_mutex_lock", rc);
		totalMargaritas += Nmargarita;
		rc = pthread_mutex_unlock(&mutex_totalMargaritas);
		error_check("pthread_mutex_unlock", rc);
		
		// update total pepperoni pizzas ordered
		rc = pthread_mutex_lock(&mutex_totalPepperonis);
		error_check("pthread_mutex_lock", rc);
		totalPepperonis += Npepperoni;
		rc = pthread_mutex_unlock(&mutex_totalPepperonis);
		error_check("pthread_mutex_unlock", rc);
		
		// update total special pizzas ordered
		rc = pthread_mutex_lock(&mutex_totalSpecials);
		error_check("pthread_mutex_lock", rc);
		totalSpecials += Nspecial;
		rc = pthread_mutex_unlock(&mutex_totalSpecials);
		error_check("pthread_mutex_unlock", rc);
	}
	// if payment failed
	else {
	
		// update total failed orders
		rc = pthread_mutex_lock(&mutex_failedOrders);
		error_check("pthread_mutex_lock", rc);
		++failedOrders;
		rc = pthread_mutex_unlock(&mutex_failedOrders);
		error_check("pthread_mutex_unlock", rc);
		
		// order was canceled due to unsuccessful payment so exit thread
		pthread_exit(tid);
	}



	// -------- Preparation Time Section ----------------------------------

	rc = pthread_mutex_lock(&mutex_availableCooks);
	error_check("pthread_mutex_lock", rc);

	// wait until you reach an available Cook
	while (availableCooks < 1) {
		printf("Order with id %d is waiting for available cook.\n", *tid);
		rc = pthread_cond_wait(&cond_availableCooks, &mutex_availableCooks);
		error_check("pthread_cond_wait", rc);
	}
	
	// when you do, decrease the availble cooks
	--availableCooks;
	
	rc = pthread_mutex_unlock(&mutex_availableCooks);
	error_check("pthread_mutex_unlock", rc);

	printf("Order with id %d is being prepared.\n", *tid);

	// prepare Npizzas for Tprep time each
	preparingTime = Tprep * Npizzas;
	sleep(preparingTime);



	// -------- Baking Time Section ----------------------------------------

	rc = pthread_mutex_lock(&mutex_availableOvens);
	error_check("pthread_mutex_lock", rc);

	// wait until you reach available Ovens to bake all pizzas for this order
	while (availableOvens < Npizzas) {
		printf("Order with id %d is waiting for available oven(s).\n", *tid);
		rc = pthread_cond_wait(&cond_availableOvens, &mutex_availableOvens);
		error_check("pthread_cond_wait", rc);
	}
	
	// when you do decrease the available ovens by the amount of this order's pizzas
	availableOvens -= Npizzas;
	
	rc = pthread_mutex_unlock(&mutex_availableOvens);
	error_check("pthread_mutex_unlock", rc);

	printf("Order with id %d baking in the oven(s).\n", *tid);

	// release the cook
	rc = pthread_mutex_lock(&mutex_availableCooks);
	error_check("pthread_mutex_lock", rc);
	++availableCooks;
	rc = pthread_cond_signal (&cond_availableCooks);
	error_check("pthread_cond_signal", rc);
	rc = pthread_mutex_unlock(&mutex_availableCooks);
	error_check("pthread_mutex_unlock", rc);

	// bake pizzas for Tbake time
	sleep(Tbake);

	// get the time when oven(s) stop baking
	clock_gettime(CLOCK_REALTIME, &baked_time);



	// -------- Packing Time Section ---------------------------------------

	rc = pthread_mutex_lock(&mutex_availableDeliverers);
	error_check("pthread_mutex_lock", rc);

	// wait until you reach an available deliverer
	while (availableDeliverers < 1) {
		printf("Order with id %d is waiting for available deliverer.\n",*tid);
		rc = pthread_cond_wait(&cond_availableDeliverers, &mutex_availableDeliverers);
		error_check("pthread_cond_wait", rc);
	}
	
	// when you do decrease the availble deliverers
	--availableDeliverers;

	rc = pthread_cond_signal (&cond_availableDeliverers);
	error_check("pthread_cond_signal", rc);
	
	rc = pthread_mutex_unlock(&mutex_availableDeliverers);
	error_check("pthread_mutex_unlock", rc);
	
	// release the ovens
	rc = pthread_mutex_lock(&mutex_availableOvens);
	error_check("pthread_mutex_lock", rc);
	availableOvens += Npizzas;
	rc = pthread_cond_signal (&cond_availableOvens);
	error_check("pthread_cond_signal", rc);
	rc = pthread_mutex_unlock(&mutex_availableOvens);
	error_check("pthread_mutex_unlock", rc);

	printf("Order with id %d is being packed.\n", *tid);

	// pack pizzas 
	packingTime = Tpack * Npizzas;
	sleep(packingTime);

	// get the time when order is ready to be delivered
	clock_gettime(CLOCK_REALTIME, &packed_time);
	readyTime = (packed_time.tv_sec - start_time.tv_sec);

	// print message 
	rc = pthread_mutex_lock(&mutex_lockScreen);
	error_check("pthread_mutex_lock", rc);
	printf("Order with id %d was ready for delivery in %d minutes.\n", *tid, readyTime);
	rc = pthread_mutex_unlock(&mutex_lockScreen);
	error_check("pthread_mutex_unlock", rc);

	

	// -------- Delivery Time Section ----------------------------------------------

	printf("Order with id %d is being deliverd.\n", *tid);

	// find randomly how much time the deliverer needs to deliver the order 
	deliveryTime = rand_r(&seed) % TdelHigh + TdelLow;
	
	// deliver the pizzas
	sleep(deliveryTime);

	// get the time when the order has been delivered
	clock_gettime(CLOCK_REALTIME, &del_time);
	deliveredTime = (del_time.tv_sec - start_time.tv_sec);
	
	// calculate the cold time 
	coldTime = (del_time.tv_sec - baked_time.tv_sec);

	// print message 
	rc = pthread_mutex_lock(&mutex_lockScreen);
	error_check("pthread_mutex_lock", rc);
	printf("Order with id %d was delivered in %d minutes.\n", *tid, deliveredTime);
	rc = pthread_mutex_unlock(&mutex_lockScreen);
	error_check("pthread_mutex_unlock", rc);

	// get back to the store
	sleep(deliveryTime);

	// release the deliverer
	rc = pthread_mutex_lock(&mutex_availableDeliverers);
	error_check("pthread_mutex_lock", rc);
	++availableDeliverers;
	rc = pthread_cond_signal (&cond_availableDeliverers);
	error_check("pthread_cond_signal", rc);
	rc = pthread_mutex_unlock(&mutex_availableDeliverers);
	error_check("pthread_mutex_unlock", rc);



	// -------- Updating Statistics Section ----------------------------------------

	// update total delivery time
	rc = pthread_mutex_lock(&mutex_totalDeliveryTime);
	error_check("pthread_mutex_lock", rc);
	totalDeliveryTime += deliveredTime;
	rc = pthread_mutex_unlock(&mutex_totalDeliveryTime);
	error_check("pthread_mutex_unlock", rc);

	// update max delivery time
	rc = pthread_mutex_lock(&mutex_maxDeliveryTime);
	error_check("pthread_mutex_lock", rc);
	if (maxDeliveryTime < deliveredTime) {
		maxDeliveryTime = deliveredTime; }
	rc = pthread_mutex_unlock(&mutex_maxDeliveryTime);
	error_check("pthread_mutex_unlock", rc);

	// update total cold time
	rc = pthread_mutex_lock(&mutex_totalColdTime);
	error_check("pthread_mutex_lock", rc);
	totalColdTime += coldTime;
	rc = pthread_mutex_unlock(&mutex_totalColdTime);
	error_check("pthread_mutex_unlock", rc);

	// update max cold time
	rc = pthread_mutex_lock(&mutex_maxColdTime);
	error_check("pthread_mutex_lock", rc);
	if (maxColdTime < coldTime) {
		maxColdTime = coldTime; }
	rc = pthread_mutex_unlock(&mutex_maxColdTime);
	error_check("pthread_mutex_unlock", rc);
	
	// finished with this order so exit thread
	pthread_exit(tid);
}



int main(int argc, char *argv[]) {

	// Check for passed arguments number
    if (argc != 3) {
		printf("ERROR: the program should take 2 arguments, the number of customers to service and the seed for the random number generator!\n");
		exit(-1);
	}
	
	// Xrisi tis atoi gia metatropi char* se int
    Ncust = atoi(argv[1]); 	// number of customers
	seed  = atoi(argv[2]);	// seed for the random number generator
	
	// Initialize variables
    availableTels = Ntel;
	availableCooks = Ncook;
	availableOvens = Noven;
	availableDeliverers = Ndeliverer; 

	totalEarnings = 0;
	successfulOrders = 0;
	failedOrders = 0;
	
	totalMargaritas = 0;
	totalPepperonis = 0;
	totalSpecials = 0;

	totalDeliveryTime = 0;
	maxDeliveryTime = 0;

	totalColdTime = 0;
	maxColdTime = 0;
	
	initMutexesAndConditionals();
		
	// Allocate memory for order threads and their ids
	int *threadIds;
	pthread_t *threads;

	threadIds = malloc(Ncust * sizeof(int)); 
	threads   = malloc(Ncust * sizeof(pthread_t));
	
    if (threadIds == NULL || threads == NULL){
        printf("Error allocating memory.\n");
        exit(-1);
    }
		
	// response code from pthread actions
	int rc;
	    
	// Creating threads
	for (int i = 0; i < Ncust; i++){ 
	
		// Initialize thread's id   
		threadIds[i] = i + 1;	

    	rc = pthread_create(&threads[i], NULL, Order, &threadIds[i]);
        error_check("pthread_create", rc);

		// seed unique for each thread
		unsigned int seedp = seed ^ threadIds[i];
		
		// find randomly in how many minutes the next customer will arrive
		int nextCustTime = rand_r(&seedp) % TorderHigh + TorderLow;
		
		// wait the next customer's arrival for: nextCustTime minutes
		sleep(nextCustTime);
	}
 
	// Join threads
	void *status;
	for (int i = 0; i < Ncust; i++){
		rc = pthread_join(threads[i], &status);
		error_check("pthread_join", rc);		
	}


	// Print statistics
	printf("\n\nTotal earnings: %d", totalEarnings);
	printf("\nSuccessful orders: %d", successfulOrders);
    printf("\nFailed orders: %d", failedOrders);
    
    printf("\n\nTotal margarita pizzas: %d", totalMargaritas);
    printf("\nTotal pepperoni pizzas: %d", totalPepperonis);
    printf("\nTotal special pizzas: %d", totalSpecials);
	
    printf("\n\nAverage service time: %d", totalDeliveryTime/successfulOrders);
	printf("\nMax service time: %d", maxDeliveryTime);
	
    printf("\n\nAverage freezing time: %d", totalColdTime/successfulOrders);
    printf("\nMax freezing time: %d\n\n", maxColdTime);
	

	// Free allocated memory 
	destroyMutexesAndConditionals();
    free(threads);
	free(threadIds);

	return 1;
}

