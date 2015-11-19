/**
 * usual head files
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <proc.h>
#include <fileio.h>
#include <signal.h>
#include <thread/mutex.h>
#include <thread/thread.h>
#include <utils/utils.h>
#include <utils/linked_list.h>
#include <socket/event.h>

event_t *evt = NULL;
#define FIFO_PATH "./"
#define CONFIG_FILE_PATH "./cmd_agent.cfg"
typedef struct conf_t conf_t;
struct conf_t {
    /**
     * @brief config path
     */
    const char *path;

    /**
     * @brief max count of task queue
     */
    int task_queue_cnt;

    /**
     * @brief time of select waiting
     */
    int timeout;
} conf = {
    .path           = CONFIG_FILE_PATH,
    .task_queue_cnt = -1,
    .timeout       = -1
};

typedef struct task_t task_t;
struct task_t {
    /**
     * @brief id of cmd execute process
     */
    pid_t pid;

    /**
     * @brief task command
     */
    char *cmd;

    /**
     * @brief create time of task
     */
    long ctime;

    /**
     * @brief execute time of task
     */
    long etime;

    /**
     * @brief finish time of task
     */
    long ftime;
};

typedef struct task_queue_t task_queue_t;
struct task_queue_t {
    /**
     * @brief task queue id
     */
    int id;

    /**
     * @brief stop queue
     */
    int stop;

    /**
     * @brief task number in this queue
     */
    int task_num;

    /**
     * @brief fifo fd
     */
    int fd;

    /**
     * @brief timeout of command executing
     */
    int timeout;

    /**
     * @brief fifo
     */
    ipc_t *fifo;

    /**
     * @brief handler task in queue 
     */
    thread_t *handler;

    /**
     * @brief rwlock
     */
    mutex_t *lock;

    /**
     * @brief task queue
     */
    linked_list_t *task;
} *task_queue = NULL;

/*********************************************************
 **************    Function Declaration    ***************
 *********************************************************/
/**
 * @brief read config 
 */
int read_config()
{
    ini_t *ini = create_ini(conf.path);
    if (!ini) return -1;

    conf.task_queue_cnt = atoi(ini->get_value(ini,"queue", "max_cnt"));
    conf.timeout = atoi(ini->get_value(ini,"cmd", "timeout"));
    ini->destroy(ini);

    if (conf.timeout < 0 || conf.task_queue_cnt < 0) return -1;
    return 0;
}

/**
 * @brief task_handler 
 */
static void *task_handler(task_queue_t *taskq)
{
    task_t *task = NULL;
    while (!taskq->stop) {
        if (taskq->task->get_count(taskq->task) == NOT_FOUND) continue;

        if (taskq->task->get_first(taskq->task, (void **)&task) == NOT_FOUND)
            continue;

        task->etime = read_systime();
        cmd_exec(task->cmd, &task->pid);

        taskq->lock->lock(taskq->lock);
        taskq->task->remove_first(taskq->task, (void **)&task);
        taskq->lock->unlock(taskq->lock);
        free(task);
    }
    return NULL;
}

/**
 * @brief task_queue_init 
 */
int task_queue_init()
{
    ini_t *ini = NULL;
    int  i      = 0;
    char *str   = NULL;
    char *token = NULL;
    char *save  = NULL;
    char fifo_path[128] = {0};
    task_queue_t *queue = NULL;
    char *task_queue_timeout = NULL;

    /**
     * ask memory for task queue 
     */
    task_queue = (task_queue_t *)malloc(conf.task_queue_cnt * sizeof(struct task_queue_t));
    if (!task_queue) return -1;
    queue = task_queue;

    /**
     * read task queue timeout
     */
    ini = create_ini(conf.path);
    task_queue_timeout = ini->get_value(ini, "queue", "timeout");
    
    /**
     * initial task queue 
     * 1. create fifo 
     * 2. create task queue
     */
    threads_init();
    for (str = task_queue_timeout, i = 0; i < conf.task_queue_cnt; str = NULL, i++) {
        /**
         * get task queue timeout
         */
        token = strtok_r(str, ":,", &save);
        if (!token) break;
        queue->timeout = atoi(token);

        /**
         * create fifo 
         */
        sprintf(fifo_path, "%s%d", FIFO_PATH, i);
        queue->fifo = create_ipc();
        if (!queue->fifo) return -1;
        queue->fd = queue->fifo->mkfifo(queue->fifo, fifo_path, O_RDONLY | O_NONBLOCK);

        /**
         * create task queue
         */
        queue->task = linked_list_create();
        if (!queue->task) {
            queue->fifo->destroy(queue->fifo);
            break;
        }

        /**
         * create rwlock
         */
        queue->lock = mutex_create();
        if (!queue->lock) {
            queue->fifo->destroy(queue->fifo);
            queue->task->destroy(queue->task);
            break;
        }

        /**
         * create thread handler
         */
        queue->handler = thread_create((void *)task_handler, queue);
        if (!queue->handler) {
            queue->fifo->destroy(queue->fifo);
            queue->task->destroy(queue->task);
            queue->lock->destroy(queue->lock);
            break;
        }

        queue->stop = 0;
        queue->id = i;
        queue++;
    }

    ini->destroy(ini);
    if (i < 1) return -1;
    else if (i < conf.task_queue_cnt) {
        conf.task_queue_cnt = i;
    }

    return 0;
}

/**
 * @brief deinit task queue
 */
int task_queue_deinit()
{
    int i = 0;
    task_t *task = NULL;
    task_queue_t *queue = task_queue;

    if (!task_queue) return -1;

    for (queue = task_queue, i = 0; i < conf.task_queue_cnt; queue++, i++) {
        if (queue->task != NULL) {
            while (queue->task->remove_first(queue->task, (void **)&task) != NOT_FOUND) {
                free(task->cmd);
                free(task);
            }
        }

        if (queue->fifo) queue->fifo->destroy(queue->fifo);
        if (queue->task) queue->task->destroy(queue->task);
        if (queue->handler) queue->handler->destroy(queue->handler);
        if (queue->lock) queue->lock->destroy(queue->lock);
    }

    return 0;
}

/**
 * @brief free memory 
 */
static void free_memory()
{
    /**
     * finish task and free memory
     */
    if (evt != NULL) evt->destroy(evt);
    task_queue_deinit();
    if (task_queue != NULL) free(task_queue);
}

/**
 * @brief error handler 
 */
static void error_handler(int sig,siginfo_t *info, void *text)
{
    switch (sig) {
        case SIGINT:
        case SIGTERM:
            free_memory();
            exit(1);
            break;
    }
}

/**
 * @brief read fifo handler
 */
void* fifo_listen_handler(int fd, task_queue_t *taskq)
{
    char buf[128] = {0};
    char *str = NULL;
    char *token = NULL, *save = NULL;
    task_t *newtask = NULL;

    if (fd != taskq->fd) return NULL;
    taskq->fifo->read(taskq->fifo, buf, sizeof(buf));
    for (str = buf; ; str = NULL) {
        token = strtok_r(str, "\n:;,", &save);
        if (!token) break;

        newtask = (task_t *)malloc(sizeof(task_t));
        if (!newtask) continue;
        newtask->cmd = strdup(token);
        newtask->ctime = read_systime();

        taskq->lock->lock(taskq->lock);
        taskq->task->insert_last(taskq->task, newtask);
        taskq->lock->unlock(taskq->lock);
        printf("cmd: %s\n", token);
    }
    return NULL;
}

/**
 * @brief handle command timeout
 */
static void timeout_handle(task_queue_t *taskq)
{
    task_t *task = NULL;
    int i = 0;

    for (i = 0; i < conf.task_queue_cnt; i++) {
        if (taskq->task->get_first(taskq->task, (void **)&task) == NOT_FOUND) return;
        if ((read_systime() - task->etime) >= taskq->timeout) {
            kill(-task->pid, SIGTERM);
        }
        taskq++;
    }
}

/*********************************************************
 ******************    Main Function    ******************
 *********************************************************/
int main(int agrc, char *agrv[])
{
    int rt = -1; /* return value of function main */
    int i = 0;
    struct sigaction act;
    task_queue_t *queue = NULL;

    /**
     * read config
     */
    if (read_config() < 0) goto deinit;

    /**
     * @brief initial task queue
     */
    if (task_queue_init() < 0) goto deinit;

    /**
     * create fifo fd event listening
     */
    evt = create_event(EVENT_MODE_SELECT, conf.timeout * 1000);
    if (!evt) goto deinit;
    evt->exception_handle(evt, EXCEPTION_TIMEOUT, (void *)timeout_handle, task_queue);
    for (i = 0, queue = task_queue; i < conf.task_queue_cnt; i++, queue++) {
        evt->add(evt, queue->fd, EVENT_ON_RECV, (void *)fifo_listen_handler, queue);
    }

    /**
     * act error handler
     */
    act.sa_sigaction = (void *)error_handler;
    act.sa_flags =  SA_SIGINFO;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    sleep(20);    
    rt = 0;
deinit:
    /**
     * finish task and free memory
     */
    free_memory();
    return rt;
}
