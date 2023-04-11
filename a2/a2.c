#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include "a2_helper.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread8(void *arg)
{
    int id = *(int *)arg;
    info(BEGIN, 8, id);
    info(END, 8, id);
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
    int t1_arg = 1, t2_arg = 2, t3_arg = 3, t4_arg = 4;
    pthread_t t1 = -1, t2 = -1, t3 = -1, t4 = -1, tid[47], tid2[5];
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
            pthread_create(&t1, NULL, thread8, &t1_arg);
            pthread_create(&t2, NULL, thread8, &t2_arg);
            pthread_create(&t3, NULL, thread8, &t3_arg);
            pthread_create(&t4, NULL, thread8, &t4_arg);
            pthread_join(t1, NULL);
            pthread_join(t2, NULL);
            pthread_join(t3, NULL);
            pthread_join(t4, NULL);
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
