#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>

sem_t resource,mutex;						//Semaphore 'resource' is for locking/unlocking access to the shared resource 									//Semaphore 'mutex' is for mutual exclusion between the reader threads whilst 									//executing their critical section
int readerCount, data;						//Reader Count and shared variable 'data'

void *Reader (void *arg);
void *Writer (void *arg);

int main()
{
	int i=0;
	int NumberOfReaderThread=0,NumberOfWriterThread=0;
	sem_init(&resource,0,1);				//Semaphore Initialization
	sem_init(&mutex,0,1);					//Semaphore Initialization

	pthread_t Readers_Th[10],Writers_Th[10];				//Thread Initialization
	printf("\nEnter number of Reader Threads (Min 2, Max 10)\n");
	scanf("%d",&NumberOfReaderThread);
	printf("\nEnter number of Writer Threads (Max 10)\n");
	scanf("%d",&NumberOfWriterThread);

	for(i=0;i<NumberOfReaderThread;i++){
		pthread_create(&Readers_Th[i],NULL,Reader,(void *)i);		//Thread Creation
	}

	for(i=0;i<NumberOfWriterThread;i++){
		pthread_create(&Writers_Th[i],NULL,Writer,(void *)i);		//Thread Creation
	}

	for(i=0;i<NumberOfWriterThread;i++){
		pthread_join(Writers_Th[i],NULL);				//Wait for Thread termination
	}

	for(i=0;i<NumberOfReaderThread;i++){
		pthread_join(Readers_Th[i],NULL);				//Wait for Thread termination
	}

	sem_destroy(&resource);							//Semaphore deletion
	sem_destroy(&mutex);							//Semaphore deletion
	return 0;
}

void *Writer(void *arg)
{
	sleep(1);
	int temp = ((int)arg);
	printf("Writer %d is about to access the resource to modify the data\n",temp);
	sem_wait(&resource);								//Writer locks the resource from other readers/writers
	printf("Writer %d is writing to the resource\n",temp);				
	data++;										//Writing to shared variable 'data'
	printf("Data is %d after writing by Writer %d\n",data,temp);			
	printf("Writer %d is releasing the resource\n",temp);
	sem_post(&resource);								//Writer releases the resource after it is done
}

void *Reader(void *arg)
{
	sleep(1);
	int temp = ((int)arg);
	printf("Reader %d is about to access the resource to read the data\n",temp);
	sem_wait(&mutex);								//Reader locks the mutex only for other Reader threads 												//to execute its critical section that follows
	readerCount++;									//No. of Reader threads accessing shared resource 												//incremented
	if (readerCount==1) {
	sem_wait(&resource);									//If it is the first reader to access the 													//shared resource, it locks the resource from 													//Writer threads
	printf("Reader %d is the first reader to access the resource, thus locking it\n",temp);
	}
	sem_post(&mutex);								//Reader's critical section over, mutex released
	printf("Data read by Reader %d is %d\n",temp,data);				//Reading
	sem_wait(&mutex);								//Reader locks the mutex only for other Reader threads 												//to execute its critical section that follows
	readerCount--;									//No. of Reader threads accessing shared resource 												//decremented
	printf("Reader %d is about to exit the resource\n",temp);
	if (readerCount==0) {
	sem_post(&resource);									//If it is the last reader to access the 													//shared resource, it releases the resource 													//for other waiting Writer/Reader threads
	printf("Reader %d is the last reader to access the resource, thus releasing it\n",temp);
	}
	sem_post(&mutex);								//Reader's critical section over, mutex released
}	
