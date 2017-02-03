/* Note: In this program the scheduling policy of LINUX OS is changed to 'SCHED_FIFO' to demonstrate threads with different priorities in contention of resources.
		 This may require the executable to be run as a super user using the 'sudo' command.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>

#define handle_error_en(en, msg) \											// Error-handling
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
							 									
pthread_mutex_t resource, reader_mutex;
//pthread_attr_t disp_tattr;
struct sched_param displayParam;
int displayPolicy;
int readerCount, data;													//Reader Count and shared variable 'data'

void *Reader (void *arg);
void *Writer (void *arg);

int main()
{
	pthread_mutexattr_t prioAttr;
	
	pthread_mutexattr_init(&prioAttr);
	pthread_mutexattr_setprotocol(&prioAttr, PTHREAD_PRIO_INHERIT);			// Mutex attribute with priority inheritance protocol set
	pthread_mutex_init(&resource, &prioAttr);								//Mutex 'resource' is for locking/unlocking access to the shared resource
	pthread_mutex_init(&reader_mutex, &prioAttr);							//Mutex 'reader_mutex' is for mutual exclusion between the reader threads whilst executing their critical section
	pthread_mutexattr_destroy(&prioAttr);

	int i=0;

	pthread_attr_t tattr;
	pthread_t r0;
	int ret;
	int newprio = 99;
	int policy = SCHED_FIFO;
	struct sched_param param;

	pthread_attr_init (&tattr);											/* initialized with default attributes */
	pthread_attr_getschedparam (&tattr, &param);							/* safe to get existing scheduling param */
	pthread_attr_setinheritsched(&tattr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&tattr, policy);
	param.sched_priority = newprio;												/* set the priority; others are unchanged */
	pthread_attr_setschedparam (&tattr, &param);							/* setting the new scheduling param */
	ret = pthread_create(&r0,&tattr,Reader,(void *)i);							//Thread Creation - r0 with priority specified
	if (ret != 0)
        handle_error_en(ret, "pthread_create");
	// for debug
	//pthread_getschedparam (pthread_self(), &displayPolicy, &displayParam);
	//printf("Reader %d with priority %d is about to access the resource to read the data\n",i, displayParam.sched_priority);

	i = 1;
	newprio = 96;
	pthread_t r1;

	pthread_attr_setinheritsched(&tattr, PTHREAD_EXPLICIT_SCHED);
	param.sched_priority = newprio;
	pthread_attr_setschedparam (&tattr, &param);
	pthread_create(&r1,&tattr,Reader,(void *)i);							//Thread Creation - r1 with priority specified

	i = 2;
	newprio = 95;
	pthread_t r2;

	pthread_attr_setinheritsched(&tattr, PTHREAD_EXPLICIT_SCHED);
	param.sched_priority = newprio;
	pthread_attr_setschedparam (&tattr, &param);
	pthread_create(&r2,&tattr,Reader,(void *)i);							//Thread Creation - r2 with priority specified

	i = 0;
	newprio = 98;
	pthread_t w0;

	pthread_attr_setinheritsched(&tattr, PTHREAD_EXPLICIT_SCHED);
	param.sched_priority = newprio;
	pthread_attr_setschedparam (&tattr, &param);
	pthread_create(&w0,&tattr,Writer,(void *)i);							//Thread Creation - w0 with priority specified

	i = 1;
	newprio = 97;
	pthread_t w1;

	pthread_attr_setinheritsched(&tattr, PTHREAD_EXPLICIT_SCHED);
	param.sched_priority = newprio;
	pthread_attr_setschedparam (&tattr, &param);
	pthread_create(&w1,&tattr,Writer,(void *)i);							//Thread Creation - w1 with priority specified

	pthread_attr_destroy(&tattr);

	pthread_join(r0,NULL);														//Wait for Thread termination
	pthread_join(r1,NULL);
	pthread_join(r2,NULL);
	pthread_join(w0,NULL);
	pthread_join(w1,NULL);

	pthread_mutex_destroy(&resource);											//Mutex deletion
	pthread_mutex_destroy(&reader_mutex);										//Mutex deletion
	return 0;
}

void *Writer(void *arg)
{
	sleep(1);
	int temp = ((int)arg);
	//pthread_attr_init (&disp_tattr);
	pthread_getschedparam (pthread_self(), &displayPolicy, &displayParam);
	printf("Writer %d with priority %d is about to access the resource to modify the data\n",temp, displayParam.sched_priority);
	pthread_mutex_lock(&resource);													//Writer locks the resource from other readers/writers
	printf("Writer %d is writing to the resource\n",temp);				
	data++;																			//Writing to shared variable 'data'
	printf("Data is %d after writing by Writer %d\n",data,temp);			
	printf("Writer %d is releasing the resource\n",temp);
	pthread_mutex_unlock(&resource);												//Writer releases the resource after it is done
}

void *Reader(void *arg)
{
	sleep(1);
	int temp = ((int)arg);
	//pthread_attr_init (&disp_tattr);
	pthread_getschedparam (pthread_self(), &displayPolicy, &displayParam);
	printf("Reader %d with priority %d is about to access the resource to read the data\n",temp, displayParam.sched_priority);
	pthread_mutex_lock(&reader_mutex);												//Reader locks the mutex only for other Reader threads to execute its critical section that follows
	readerCount++;																	//No. of Reader threads accessing shared resource incremented
	if (readerCount==1) {
		pthread_mutex_lock(&resource);													//If it is the first reader to access the shared resource, it locks the resource from Writer threads
		printf("Reader %d is the first reader to access the resource, thus locking it\n",temp);
	}
	pthread_mutex_unlock(&reader_mutex);											//Reader's critical section over, mutex released
	printf("Data read by Reader %d is %d\n",temp,data);								//Reading
	pthread_mutex_lock(&reader_mutex);												//Reader locks the mutex only for other Reader threads to execute its critical section that follows
	readerCount--;																	//No. of Reader threads accessing shared resource decremented
	printf("Reader %d is about to exit the resource\n",temp);
	if (readerCount==0) {
		pthread_mutex_unlock(&resource);												//If it is the last reader to access the shared resource, it releases the resource for other waiting Writer/Reader threads
		printf("Reader %d is the last reader to access the resource, thus releasing it\n",temp);
	}
	pthread_mutex_unlock(&reader_mutex);											//Reader's critical section over, mutex released
}	
