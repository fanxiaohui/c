/**
 * usual head files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread.h>

/*********************************************************
 **************    Function Declaration    ***************
 *********************************************************/
void* sayhello(void *arg)
{
    printf("hello\n");

    return NULL;
}

void* sayhi(void *arg)
{
    printf("hi\n");

    return NULL;
}

void* waitsleep(void *arg)
{
    sleep(5);
    printf("sleep 5\n");
    return NULL;
}

void* waitsleep1(void *arg)
{
    sleep(2);
    printf("sleep 2\n");

    return NULL;
}

void clean(void *arg)
{
    printf("clean up\n");
}

void *pthread(void *arg)
{
    printf("pthrad\n");
    struct thread *pthread = (struct thread *)arg;
    printf("d: %d, r: %d\n", pthread->delete, pthread->run);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_cleanup_push(clean, NULL);
    pthread_testcancel();
    sleep(5);
    pthread_testcancel();

    printf("pthread be canceled\n");
    pthread_cleanup_pop(0);
    return NULL;
}

/*********************************************************
 ******************    Main Function    ******************
 *********************************************************/
int main(int agrc, char *agrv[])
{
    int rt = 0; /* return value of function main */
    
    pthread_t pid;
    struct thread t = {0};
    t.run = 1;
    t.done = 1;

    //pid = pstart(pthread, &t);
    pthread_create(&pid, NULL, pthread, &t);
    sleep(2);
    pthread_cancel(pid);
    sleep(5);
   
    /*
    pid = pstart(sayhi, &t);
    pid = pstart(sayhi, &t);
    pid = pstart(sayhi, &t);
    pid = pstart(sayhi, &t);
    sleep(3);

    return 0;
    */

    /*
    struct thread_cfg cfg = {0};

    thread_create("sayhello", sayhello, NULL, 1, 0);
    thread_create("waitsleep", waitsleep, NULL, 1, 0);
    thread_create("waitsleep1", waitsleep1, NULL, 1, 0);
    thread_create("sayhi", sayhi, NULL, 1, 0);

    cfg.run = 1;
    cfg.name = "pthread";
    cfg.worker.handler = pthread;
    cfg.worker.arg = NULL;
    rt = pthread_start(&cfg);
    pthread_hold(rt);
    pthread_info();

    sleep(1);
    thread_create("sayhi", sayhi, NULL, 1, 0);
    cfg.name = "sayhello";
    cfg.worker.handler = sayhello;
    cfg.worker.arg = NULL;

    rt = pthread_start(&cfg);
    pthread_info();
    */

    pthread_pool_init(5, 3, 2, 4);
    
    pthread_pool_add(sayhi, NULL);
    pthread_pool_add(sayhi, NULL);
    pthread_pool_add(sayhi, NULL);
    pthread_pool_add(sayhi, NULL);
    pthread_pool_add(sayhi, NULL);
    pthread_pool_add(sayhi, NULL);
    pthread_pool_add(waitsleep, NULL);
    pthread_pool_add(waitsleep, NULL);
    while (rt++ < 5) pthread_pool_add(sayhello, NULL);

    sleep(8);
            
    return rt;
}
