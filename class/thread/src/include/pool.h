#ifndef __POOL_H__
#define __POOL_H__

typedef struct pool_t pool_t;
struct pool_t {
    /**
     * @brief add task to thread pool
     * @param work  task 
     * @param arg   parameter of task
     */
    int (*addjob) (pool_t *this, void (*job) (void *), void *arg);

    /**
     * @brief destroy instance and free memory 
     */
    void (*destroy) (pool_t *this);
};

/**
 * @brief create pool instance 
 * @param min_size   pool thread min size
 * @param max_size   pool thread max size
 */
pool_t *create_pool(int min_size, int max_size);

#endif /* __POOL_H__ */
