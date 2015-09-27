/**
 * usual head files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "thread.h"
#include "thread_value.h"

/*********************************************************
 **************    Function Declaration    ***************
 *********************************************************/
void* sayhi(void *arg)
{
    sleep(1);
    printf("hi, id: %d\n", thread_current_id());

    return NULL;
}

void* sayhi1(void *arg)
{
    sleep(2);
    printf("hi1\n");

    return NULL;
}

void* sayhi2(void *arg)
{
    printf("hi2\n");

    return NULL;
}

void cleanup(void *arg)
{
    printf("clean up\n");
}

/*********************************************************
 ******************    Main Function    ******************
 *********************************************************/
int main(int agrc, char *agrv[])
{
    int rt = 0; /* return value of function main */

    thread_t *thread = NULL; //*t = NULL, *t1 = NULL;
    threads_init();
    thread_cancelability(true);
    thread = thread_create(sayhi, NULL);

    thread->join(thread);
    threads_deinit();

    return rt;
}