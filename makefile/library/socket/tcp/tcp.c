#include <tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <utils/utils.h>
#include <host/host.h>
#include <sys/select.h>
#include <fcntl.h>

typedef struct private_tcp_t private_tcp_t;
struct private_tcp_t {
    /**
     * @brief public interface
     */
    tcp_t public;

    /**
     * @brief socket handler fd
     */
    int fd;

    /**
     * @brief tcp server accept fd
     */
    int accept_fd;

    /**
     * @brief socket host
     */
    host_t *host;

    /**
     * @brief status of tcp connection
     */
    tcp_status_t status;
};
#define tcp_fd        this->fd
#define tcp_host      this->host
#define tcp_accept_fd this->accept_fd
#define tcp_state     this->status
 
static void make_reusable(int fd)
{
    int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(on));
}

static void make_nonblock(int fd)
{
    int flag = 0;

    flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0) return;
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

static void make_block(int fd)
{
    int flag = 0;

    flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0) return;
    fcntl(fd, F_SETFL, flag & ~O_NONBLOCK);
}

/*
static int get_sock_err(int fd)
{
    int err;
    int len = sizeof(int);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, (socklen_t *)&len);;
    return err;
}
*/

METHOD(tcp_t, listen_, int, private_tcp_t *this, int family, char *ip, int port)
{
    int ret = 0;

    /**
     * create socket
     */
    if (tcp_fd > 0) close(tcp_fd);
    tcp_fd = socket(family, SOCK_STREAM, 0);
    if (tcp_fd < 0) {
        perror("socket()");
        return -1;
    }

    /**
     * create host
     */
    if (tcp_host) tcp_host->destroy(tcp_host);
    tcp_host = host_create_from_string_and_family(ip ? ip : "%any", family, port);
    if (!tcp_host) {
        printf("create host failed\n");
        return -1;
    }

    /**
     * socket bind
     */
    make_reusable(tcp_fd);
    ret = bind(tcp_fd, tcp_host->get_sockaddr(tcp_host), sizeof(struct sockaddr));
    if (ret < 0) {
        perror("bind()");
        return -1;
    }

    /**
     * socket listen 
     */
    ret = listen(tcp_fd, 5);
    if (ret < 0) {
        perror("listen()");
        return -1;
    }

    tcp_state = TCP_LISTENING;
    return tcp_fd;
}

METHOD(tcp_t, connect_, int, private_tcp_t *this, int family, char *ip, int port)
{
    int ret = 0;

    /**
     * create socket
     */
    if (tcp_accept_fd <= 0) tcp_accept_fd = socket(family, SOCK_STREAM, 0);
    if (tcp_accept_fd <= 0) {
        perror("socket()");
        return -1;
    }

    /**
     * create host
     */
    if (tcp_host) {
        if (tcp_host->get_family(tcp_host) != family || tcp_host->get_port(tcp_host) != port || strcmp(tcp_host->get_ip(tcp_host, NULL, 0), ip)) {
            tcp_host->destroy(tcp_host);
            tcp_host = NULL;
    }
    }
    if (!tcp_host) tcp_host = host_create_from_string_and_family(ip ? ip : "%any", family, port);
    if (!tcp_host) {
        printf("create host failed\n");
        return -1;
    }

    /**
     * connect to server
     */
    tcp_state = TCP_CONNECTING;
    ret = connect(tcp_accept_fd, tcp_host->get_sockaddr(tcp_host), sizeof(struct sockaddr));
    if (ret < 0) {
        perror("connect()");
        return -1;
    }

    tcp_state = TCP_CONNECTED;
    return tcp_accept_fd;
}

METHOD(tcp_t, connect_tm_, int, private_tcp_t *this, int family, char *ip, int port, int timeout_ms)
{
    int ret            = 0;
    struct timeval tm  = {0};
    fd_set fds;

    /**
     * create socket
     */
    if (tcp_accept_fd <= 0) tcp_accept_fd = socket(family, SOCK_STREAM, 0);
    if (tcp_accept_fd <= 0) {
        perror("socket()");
        return -1;
    }

    /**
     * create host
     */
    if (tcp_host) {
        if (tcp_host->get_family(tcp_host) != family || tcp_host->get_port(tcp_host) != port || strcmp(tcp_host->get_ip(tcp_host, NULL, 0), ip)) {
            tcp_host->destroy(tcp_host);
            tcp_host = NULL;
    }
    }
    if (!tcp_host) tcp_host = host_create_from_string_and_family(ip ? ip : "%any", family, port);
    if (!tcp_host) {
        printf("create host failed\n");
        return -1;
    }

    /**
     * connect to server
     */
    tcp_state = TCP_CONNECTING;
    if (timeout_ms > 0) {
        make_nonblock(tcp_accept_fd);
        while (timeout_ms > 0) {
            FD_ZERO(&fds);
            FD_SET(tcp_accept_fd, &fds);
            tm.tv_sec  = 0;
            tm.tv_usec = 100 * 1000;
            select(0, NULL, NULL, NULL, &tm);
            ret = connect(tcp_accept_fd, tcp_host->get_sockaddr(tcp_host), sizeof(struct sockaddr));
            if (!ret) break;

            timeout_ms -= 100;
        }
    } else {
        make_block(tcp_accept_fd);
        ret = connect(tcp_accept_fd, tcp_host->get_sockaddr(tcp_host), sizeof(struct sockaddr));
    }

    if (ret < 0) {
        perror("connect() failed");
        return -1;
    }
    make_block(tcp_accept_fd);

    tcp_state = TCP_CONNECTED;
    return tcp_accept_fd;
}

METHOD(tcp_t, accept_, int, private_tcp_t *this)
{
    tcp_accept_fd = accept(tcp_fd, NULL, 0);
    if (tcp_accept_fd > 0) tcp_state = TCP_CONNECTED;
    return tcp_accept_fd;
}

METHOD(tcp_t, send_, int, private_tcp_t *this, void *buf, int size)
{
    return send(tcp_accept_fd, buf, size, 0);   
}

METHOD(tcp_t, recv_, int, private_tcp_t *this, void *buf, int size)
{
    return recv(tcp_accept_fd, buf, size, 0);
}

METHOD(tcp_t, recv_tm_, int, private_tcp_t *this, void *buf, int size, int timeout_ms)
{
    fd_set fds;
    struct timeval tm = {0};
    
    if (timeout_ms <= 0) 
        return recv(tcp_accept_fd, buf, size, 0);

    tm.tv_sec = timeout_ms / 1000;
    tm.tv_usec = timeout_ms % 1000 * 1000;
    FD_ZERO(&fds);
    FD_SET(tcp_accept_fd, &fds);
    switch (select(tcp_accept_fd + 1, &fds, NULL, NULL, &tm)) {
        case 0:
        case -1:
            return -1;
        default:
            if (FD_ISSET(tcp_accept_fd, &fds)) 
                return recv(tcp_accept_fd, buf, size, 0);
            else return -1;
            break;
    }
}

METHOD(tcp_t, close_, int, private_tcp_t *this)
{
    tcp_state = TCP_CLOSED;
    return close(tcp_accept_fd);
}

METHOD(tcp_t, shutdown_, int, private_tcp_t *this, int how)
{
    tcp_state = TCP_CLOSED;
    return shutdown(tcp_accept_fd, how);
}

METHOD(tcp_t, destroy_, void, private_tcp_t *this)
{
    if (tcp_fd) close(tcp_fd);
    if (tcp_accept_fd) close(tcp_accept_fd);
    if (tcp_host) tcp_host->destroy(tcp_host);
    free(this);
}

METHOD(tcp_t, get_state_, tcp_status_t, private_tcp_t *this)
{
    return tcp_state;
}

tcp_t *tcp_create(int family)
{
    private_tcp_t *this;

    INIT(this, 
        .public = {
            .listen     = _listen_,
            .connect    = _connect_,
            .connect_tm = _connect_tm_,
            .accept     = _accept_,
            .send       = _send_,
            .recv       = _recv_,
            .recv_tm    = _recv_tm_,
            .close      = _close_,
            .shutdown   = _shutdown_,
            .destroy    = _destroy_,
            .get_state  = _get_state_,
        },
        .fd     = -1,
        .host   = NULL,
        .status = TCP_CLOSED,
    );

    return &this->public;
}
