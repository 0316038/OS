#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>

using namespace std;

struct subarray
{
    int left,right;
} part[16];

////global variables
int *a;
int value;
sem_t sem[16];
sem_t mutex,finish;
pthread_t thread[16];
int thread_id[16];
int count = 0;

////functions
void *func(void *num);
void *partition(void *num);
void *sort(void *num);
void bubblesort(int array[],int num);

int main ()
{
    string input;
    int num = 0;
    cout << "Input file name:" ;
    cin >> input;
    ////open file & read file
    ifstream file_in;
    file_in.open(input.c_str(),ios::in);
    file_in >> num;
    a = new int [num];
    int b[num];
    for(int i=0; i<num; i++){
        file_in >> a[i];
        b[i] = a[i];
    }
    file_in.close();
    ////multi-thread
    sem_init(&mutex,0,1);  //mutex initial value = 1
    sem_init(&finish,0,0);    
    for(int i=0; i<16; i++){
        sem_init(&sem[i],0,0);
        thread_id[i] = i;
    }
    part[1].left = 0;
    part[1].right = num-1;

    struct timeval start,end;
    gettimeofday(&start,0);
    pthread_create(&thread[0],NULL,func,NULL);
    for(int i=1; i<16; i++){
        int *t = thread_id +i;
        if(i<8){
            pthread_create(&thread[i],NULL,partition,(void *)t);
        }
        else{
            pthread_create(&thread[i],NULL,sort,(void *)t);
        }
    }
    sem_wait(&finish);
    sem_destroy(&mutex);
    sem_destroy(&finish);
    for(int i=0; i<16; i++){
        sem_destroy(&sem[i]);
    }

    gettimeofday(&end,0);
    int sec=end.tv_sec-start.tv_sec;
    int usec=end.tv_usec-start.tv_usec;
    cout << "Execution time of MT: " << sec*1000+(usec/1000.0) << "ms" << endl;
    ////output file 1
    ofstream file_out1;
    file_out1.open("output1.txt",ios::out);
    for(int i=0; i<num; i++){
        file_out1 << a[i] << " ";
    }
    file_out1.close();

    ////single thread
    gettimeofday(&start,0);
    bubblesort(b,num);
    gettimeofday(&end,0);
    sec=end.tv_sec-start.tv_sec;
    usec=end.tv_usec-start.tv_usec;
    cout << "Execution time of ST: " << sec*1000+(usec/1000.0) << "ms" << endl;
    ////output file 2
    ofstream file_out2;
    file_out2.open("output2.txt",ios::out);
    for(int i=0; i<num; i++){
        file_out2 << b[i] << " ";
    }
    file_out1.close();
    return 0;
}

void *func(void *num)
{
    sem_post(&sem[1]);
    sem_wait(&sem[0]);
    sem_post(&finish);
    pthread_exit(NULL); 
}

void *partition(void *num)
{
    int thread_n = *(int*)num;
    sem_wait(&sem[thread_n]);
    int index = part[thread_n].left;
    int pivot = a[part[thread_n].left];    //choose the left end as the pivot
    swap(a[part[thread_n].left],a[part[thread_n].right]);  //put the pivot to the right end
    for(int i=part[thread_n].left; i<part[thread_n].right; i++){
        if(a[i]<=pivot){
            swap(a[i],a[index]);
            index++;
        }
    }
    swap(a[index],a[part[thread_n].right]);
    part[thread_n*2].left = part[thread_n].left;
    part[thread_n*2].right = index;
    part[thread_n*2+1].left = index+1;
    part[thread_n*2+1].right= part[thread_n].right;
    sem_post(&sem[thread_n*2]);
    sem_post(&sem[thread_n*2+1]);
}

void *sort(void *num)
{
    int thread_n = *(int*)num;
    sem_wait(&sem[thread_n]);
    //bubble sort
    for(int i=part[thread_n].right; i>part[thread_n].left; i--){
        for(int j=part[thread_n].left; j<i; j++){
            if(a[j]>a[j+1]){
                swap(a[j],a[j+1]);
            }
        }
    }

    sem_wait(&mutex);
    count++;
    sem_post(&mutex);
    if(count == 8){
        sem_post(&sem[0]);  //signals T0
    }
    pthread_exit(NULL);
}

void bubblesort(int array[],int num)
{
    for(int i=num-1; i>0; i--){
        for(int j=0; j<i; j++){
            if(array[j]>array[j+1]){
                swap(array[j],array[j+1]);
            }
        }
    }
}
