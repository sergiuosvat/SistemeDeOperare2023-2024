#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include "a2_helper.h"

typedef struct{
    int id;
}PARAMS;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int var = 0;

void *thread8(void *arg)
{
    PARAMS *p = (PARAMS *)arg;
    if(p->id != 3 && p->id!=4)
    {
        info(BEGIN, 8, p->id);
    }
    if(p->id == 3)
    {
        info(BEGIN, 8, p->id);
        pthread_mutex_lock(&lock);
        while(var == 0)
        {
            pthread_cond_wait(&cond, &lock);
        }
        pthread_mutex_unlock(&lock);
        info(END, 8, p->id);
        pthread_exit(NULL);
    }
    if(p->id == 4)
    {
        info(BEGIN, 8, p->id);
        pthread_mutex_lock(&lock);
        var = 1;
        info(END, 8, p->id);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
    }
    info(END, 8, p->id);
    pthread_exit(NULL);
}

void *thread6(void *arg)
{
    int id = *(int *)arg;
    info(BEGIN, 6, id);
    info(END, 6, id);
    pthread_exit(NULL);
}

void *thread7(void *arg)
{
    int id = *(int *)arg;
    info(BEGIN, 7, id);
    info(END, 7, id);
    pthread_exit(NULL);
}

int main()
{
    pid_t pid2, pid3, pid4, pid5, pid6, pid7, pid8, pid9;
    pthread_t t1[4] = {-1}, tid[47] = {-1}, tid2[5] = {-1};
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
                for (int i = 0; i < 47; i++)
                {
                    int *id = malloc(sizeof(int));
                    *id = i + 1;
                    pthread_create(&tid[i], NULL, thread6, id);
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
            info(BEGIN, 8, 0);
            int param[4] = {1,2,3,4};
            // for(int i = 0;i<4;i++)
            // {
            //     param[i].id = i+1;
            //     pthread_create(&t1[i], NULL, thread8, &param[i]);
            // }
            pthread_create(&t1[2], NULL, thread8, &param[2]);
            pthread_create(&t1[0], NULL, thread8, &param[0]);
            pthread_create(&t1[1], NULL, thread8, &param[1]);
            pthread_create(&t1[3], NULL, thread8, &param[3]);
            for(int i =0;i<4;i++)
            {
                pthread_join(t1[i],NULL);
            }
            pthread_mutex_destroy(&lock);
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
