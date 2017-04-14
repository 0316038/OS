#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <queue>
#include <string.h>
#include <sstream>

using namespace std;

//global variables
int array[1000001];
sem_t finish;
sem_t mutex;
sem_t job_num;

struct job_info{
	int begin;
	int end;
	int kind;
};

class Queue{
    public:
	int first;
	int last;
	struct job_info data[100];
} ;
Queue job_queue;
//functions
void bubble_sort(int *array, int num);
int partition(int low, int high);
void *thread_pool(void *arg);
void push_job(int kind, int p, int r);

struct job_info pop_job()
{
	sem_wait(&job_num);
	sem_wait(&mutex);

	struct job_info job;
	job.begin = job_queue.data[job_queue.first].begin;
	job.end = job_queue.data[job_queue.first].end;
	job.kind = job_queue.data[job_queue.first].kind;
	job_queue.first++;

	sem_post(&mutex);
	return job;
}


int main()
{
	int n=1;
	while(n<=8){
		///input file
		int num = 0;
        ifstream file_in;
        file_in.open("input.txt",ios::in);
        file_in >> num;
        for(int i=0; i<num; i++){
            file_in >> array[i];
        }
        file_in.close();

		struct timeval start, end;
		gettimeofday(&start, 0);

		job_queue.first = 0;
		job_queue.last = 0;
		pthread_t t[n];
		sem_init(&finish, 0, 0);
		sem_init(&job_num, 0, 0);
		sem_init(&mutex, 0, 1);
		for(int i=0; i<n; i++){
			pthread_create(&t[i], NULL, thread_pool, NULL);
		}
		push_job(1, 0, num-1);
		for(int wait=0; wait<8; wait++){	//wait for sorting
			sem_wait(&finish);
		}

		gettimeofday(&end, 0);
		int sec = end.tv_sec - start.tv_sec;
		int usec = end.tv_usec - start.tv_usec;
		cout << "Execution time of thread pool with " << n << " threads: "<< sec*1000+(usec/1000.0) << "sec" << endl;

		///output file
		stringstream ss;
        ss << n;
        string output_filename="output_" + ss.str() + ".txt";
        ofstream file_out1;
        file_out1.open(output_filename.c_str(),ios::out);
        for(int i=0; i<num; i++){
            file_out1 << array[i] << " ";
        }
        file_out1.close();
		n++;
	}
}

void *thread_pool(void *arg){
	while(1)
	{
		struct job_info job = pop_job();
		int middle=0;
		switch (job.kind){
		    case 0:
				bubble_sort(array+job.begin, job.end-job.begin+1);
				sem_post(&finish);
				break;
			case 1:
				middle = partition(job.begin, job.end);
				push_job(2, job.begin, middle -1);
				push_job(2, middle + 1, job.end);
				break;
			case 2:
				middle = partition(job.begin, job.end);
				push_job(3, job.begin, middle -1);
				push_job(3, middle + 1, job.end);
				break;
			case 3:
				middle = partition(job.begin, job.end);
				push_job(0, job.begin, middle -1);
				push_job(0, middle + 1, job.end);

		}
	}
}


int partition(int low, int high){
	int pivot=array[high];
	int i=low;
    if(low<high){
        for(int j=low;j<high;j++){
            if(array[j]<=pivot){
                swap(array[i],array[j]);
                i++;
            }
        }
        swap(array[i],array[high]);
    }
	return i;
}

void bubble_sort(int *array, int num){
	for(int i=num-1; i>0; i--){
        for(int j=0; j<i; j++){
            if(array[j]>array[j+1]){
                swap(array[j],array[j+1]);
            }
        }
    }
}

void push_job(int kind, int p, int r)
{
	sem_wait(&mutex);

	job_queue.data[job_queue.last].begin = p;
	job_queue.data[job_queue.last].end = r;
	job_queue.data[job_queue.last].kind = kind;
	job_queue.last++;

	sem_post(&mutex);
	sem_post(&job_num);
}
