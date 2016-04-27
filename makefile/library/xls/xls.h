#ifndef __XLS_H__
#define __XLS_H__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct xls_t xls_t;
struct xls_t {
    /**
     * @brief open xls file, if not exist, then create it
     * @param mode O_RDONLY. O_WRONLY or O_RDWR
     */
    int (*open) (xls_t *this, const char *file, mode_t mode);

    /**
     * @brief open work sheet by index
     */
    int (*open_sheet) (xls_t *this, int sheet_index);

    /**
     * @brief  read data from xls
     */
    char *(*read) (xls_t *this, int row, int col);

    /**
     * @brief write data into xls
     */
    int (*write) (xls_t *this, int row, int col, char *data);

    /**
     * @brief save xls file
     */
    int (*save) (xls_t *this, const char *file);

    /**
     * @brief  enumerate xls data
     */
    char *(*enumerate) (xls_t *this);
    void (*reset_enumerate) (xls_t *this);

    /**
     * @brief destroy instance and free memory
     */
    void (*destroy) (xls_t *this);
};

/**
 * @brief create xls instance
 */
xls_t* xls_create();

#endif /* __XLS_H__ */