#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "a2_helper.h"


typedef struct
{
    int id;
} PARAMS;
sem_t sem8_1;
sem_t sem8_2;
sem_t sem6;
sem_t sem6_2;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int count = 1;

void *thread8(void *arg)
{
    int id = *(int *)arg;
    if(id == 2)
    {
        sem_t* sem2 = sem_open("sem2", O_CREAT, 0644,0);
        sem_t* sem5 = sem_open("sem5", O_CREAT, 0644, 0);
        sem_wait(sem2);
        info(BEGIN, 8, id);
        info(END, 8, id);
        sem_post(sem5);
    }
    else if (id == 3)
    {
        info(BEGIN, 8, id);
        sem_post(&sem8_1);
        sem_wait(&sem8_2);
        info(END, 8, id);
    }
    else if (id == 4)
    {
        sem_wait(&sem8_1);
        info(BEGIN, 8, id);
        info(END, 8, id);
        sem_post(&sem8_2);
    }
    else
    {
        info(BEGIN, 8, id);
        info(END, 8, id);
    }
    pthread_exit(NULL);
}

void *thread6(void *arg)
{
    int id = *(int *)arg;
    sem_wait(&sem6);
    pthread_mutex_lock(&mutex);
    count++;
    pthread_mutex_unlock(&mutex);
    info(BEGIN, 6, id);
    if (id == 11 && count == 6)
    {
        sem_wait(&sem6_2);
        info(END, 6, id);
    }
    else
    {
        info(END, 6, id);
        pthread_mutex_lock(&mutex);
        count--;
        pthread_mutex_unlock(&mutex);
    }
    sem_post(&sem6);
    pthread_exit(NULL);
}

void *thread7(void *arg)
{
    int id = *(int *)arg;
    sem_t* sem2 = sem_open("sem2", O_CREAT,0);
    if(id == 1)
    {
        info(BEGIN, 7, id);
        info(END, 7, id);
        sem_post(sem2);
    }else if(id == 5){
        sem_t* sem5 = sem_open("sem5", O_CREAT,0);
        sem_wait(sem5);
        info(BEGIN,7,id);
        info(END,7,id);
    }
    else{
        info(BEGIN, 7, id);
        info(END, 7, id);
    }
    
    pthread_exit(NULL);
}

int main()
{
    pid_t pid2, pid3, pid4, pid5, pid6, pid7, pid8, pid9;
    pthread_t tid[47], tid2[5];
    init();
    info(BEGIN, 1, 0);
    pid2 = fork();
    if (pid2 == 0)
    {
        info(BEGIN, 2, 0);
        pid3 = fork();
        if (pid3 == 0)
        {
            info(BEGIN, 3, 0);
            pid9 = fork();
            if (pid9 == 0)
            {
                info(BEGIN, 9, 0);
                info(END, 9, 0);
                exit(0);
            }
            else
            {
                wait(NULL);
                info(END, 3, 0);
                exit(0);
            }
        }
        pid4 = fork();
        if (pid4 == 0)
        {
            info(BEGIN, 4, 0);
            pid6 = fork();
            if (pid6 == 0)
            {
                info(BEGIN, 6, 0);
                sem_init(&sem6, 0, 6);
                sem_init(&sem6_2, 0, 0);
                int param2[47];
                for (int i = 0; i < 47; i++)
                {
                    param2[i] = i+1;
                    pthread_create(&tid[i], NULL, thread6, &param2[i]);
                }
                for (int i = 0; i < 47; i++)
                {
                    pthread_join(tid[i], NULL);
                }
                pid7 = fork();
                if (pid7 == 0)
                {
                    info(BEGIN, 7, 0);
                    for (int i = 0; i < 5; i++)
                    {
                        int *id = malloc(sizeof(int));
                        *id = i + 1;
                        pthread_create(&tid2[i], NULL, thread7, id);
                    }
                    for (int i = 0; i < 5; i++)
                    {
                        pthread_join(tid2[i], NULL);
                    }
                    info(END, 7, 0);
                    exit(0);
                }
                else
                {
                    waitpid(pid7, NULL, 0);
                    info(END, 6, 0);
                    exit(0);
                }
            }
            else
            {
                waitpid(pid6, NULL, 0);
                info(END, 4, 0);
                exit(0);
            }
        }
        pid5 = fork();
        if (pid5 == 0)
        {
            info(BEGIN, 5, 0);
            info(END, 5, 0);
            exit(0);
        }
        pid8 = fork();
        if (pid8 == 0)
        {
            pthread_t t1[4];
            info(BEGIN, 8, 0);
            int param[4];
            sem_init(&sem8_1, 0, 0);
            sem_init(&sem8_2, 0, 0);
            for (int i = 0; i < 4; i++)
            {
                param[i] = i + 1;
                pthread_create(&t1[i], NULL, thread8, &param[i]);
            }

            for (int i = 0; i < 4; i++)
            {
                pthread_join(t1[i], NULL);
            }
            info(END, 8, 0);
            exit(0);
        }
        waitpid(pid3, NULL, 0);
        waitpid(pid4, NULL, 0);
        waitpid(pid5, NULL, 0);
        waitpid(pid8, NULL, 0);
        info(END, 2, 0);
    }
    else
    {
        waitpid(pid2, NULL, 0);
        info(END, 1, 0);
        exit(0);
    }
    return 0;
}
