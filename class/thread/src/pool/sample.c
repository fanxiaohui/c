#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <thread.h>
#include <pool.h>

void sayhi(void *arg)
{
    printf("------ hi %d------\n", *(int *)arg);
}

int main(int argc, char **argv)
{
    int i = 0;
    int cnt = 100;
    int num[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    pool_t *pool = create_pool(5, 20, 1);
    if (!pool) {
        printf("create_pool failed\n");
        return -1;
    }

    while (cnt-- > 0) {
        for (i = 0; i < 10; i++) {
            pool->addjob(pool, sayhi, &num[i]);
        }
        usleep(200000);
        printf("\n-----------------%d------------------\n\n", cnt);
    }

    pool->destroy(pool);

    return 0;
}
