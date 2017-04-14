#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<stdio.h>
#include<stdlib.h>

unsigned int *A,*B,*C;  //save matrix value
void process(int dimension);

int main()
{
    int dimension = 0;
    int shmid_A,shmid_B,shmid_C;
    int size;

    printf("Input the matrix dimension:");
    scanf("%d",&dimension);

    //create a block of shared memory
    size = sizeof(int)*dimension*dimension;
    shmid_A = shmget(IPC_PRIVATE , size ,IPC_CREAT|0600); //new allocate,read only shm
    shmid_B = shmget(IPC_PRIVATE , size ,IPC_CREAT|0600);
    shmid_C = shmget(IPC_PRIVATE , size ,IPC_CREAT|0600);

    if(shmid_A == -1 || shmid_B == -1 || shmid_C == -1){
        printf("shared memory error\n");
    }

    //attach shared memory to the current process address space
    A = shmat(shmid_A, NULL, 0); //shmflg:0 for read/write
    B = shmat(shmid_B, NULL, 0);
    C = shmat(shmid_C, NULL, 0);

    //set matrix value
    int i=0,num=0;
    for(i=0; i< dimension*dimension; i++){
        A[i] = num;
        B[i] = num;
        C[i] = 0;
        num++;
    }

    process(dimension);

    shmdt(A);
    shmdt(B);
    shmdt(C);
    shmctl(shmid_A, IPC_RMID, NULL);
    shmctl(shmid_B, IPC_RMID, NULL);
    shmctl(shmid_C, IPC_RMID, NULL);
}

void process(int dimension)
{
    int i=0,j=0;
    int r=0,m=0,n=0;
    pid_t pid;
    for(i=1;i<=16;i++){
        struct timeval start, end;
        gettimeofday(&start, 0);
        for(j=0; j<i; j++){
            pid = fork();
            if(pid == -1){
                printf("Fork failed\n");
                exit(-1);
            }
            else if(pid == 0){
                for(r=dimension/i*j; r<dimension/i*(j+1); r++){
                    for(m=0; m<dimension; m++){
                        C[dimension*r+m] = 0;
                        for(n=0; n<dimension; n++){
                            C[dimension*r+m] += A[dimension*r+n]*B[dimension*n+m];
                        }
                    }
                }
                exit(0);
            }
            else if(pid > 0){

            }
        }
        int waiting=i;
        while(waiting > 0){
            wait(NULL);
            waiting--;
        }
        gettimeofday(&end, 0);
        unsigned int checksum = 0;
        for(j=0; j<dimension*dimension; j++){
            checksum+=C[j];
        }
        int sec=end.tv_sec-start.tv_sec;
        int usec=end.tv_usec-start.tv_usec;
        printf("Multiplying matrices using %d processes\n",i);
        printf("Elapsed time:%f sec, Checksum:%u\n", sec+(usec/1000000.0), checksum);
    }
}

