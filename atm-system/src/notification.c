#include "header.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define FIFO_PREFIX "/tmp/atm_notify_"

static pid_t listener_pid = -1;
static char g_fifo_path[256] = {0};

static void getFifoPath(char *path, const char *username)
{
    snprintf(path, 256, "%s%s", FIFO_PREFIX, username);
}

static void cleanup_on_exit(void)
{
    if (listener_pid > 0)
    {
        kill(listener_pid, SIGTERM);
        waitpid(listener_pid, NULL, 0);
    }
    if (g_fifo_path[0])
        unlink(g_fifo_path);
}

void startNotificationListener(struct User u)
{
    getFifoPath(g_fifo_path, u.name);

    // Create the FIFO; EEXIST is fine (leftover from a previous session)
    if (mkfifo(g_fifo_path, 0666) == -1 && errno != EEXIST)
        return;

    atexit(cleanup_on_exit);

    pid_t pid = fork();
    if (pid < 0)
        return;

    if (pid == 0)
    {
        // ── Child process: notification listener ──────────────────────────

        // Open read end non-blocking first so we don't hang waiting for a writer
        int fd_read = open(g_fifo_path, O_RDONLY | O_NONBLOCK);
        if (fd_read == -1)
            _exit(1);

        // Keep a write end open in this process so read() never gets a
        // premature EOF when an external writer closes its end
        int fd_dummy = open(g_fifo_path, O_WRONLY | O_NONBLOCK);

        // Switch read end to blocking so read() sleeps until data arrives
        fcntl(fd_read, F_SETFL, 0);

        char buf[512];
        ssize_t n;
        while ((n = read(fd_read, buf, sizeof(buf) - 1)) > 0)
        {
            buf[n] = '\0';
            printf("\n\n\t========================================\n");
            printf("\t          *** NOTIFICATION ***\n");
            printf("\t  %s\n", buf);
            printf("\t========================================\n\n");
            fflush(stdout);
        }

        close(fd_read);
        if (fd_dummy != -1)
            close(fd_dummy);

        // Use _exit to skip atexit handlers — cleanup belongs to the parent
        _exit(0);
    }

    // ── Parent: save child PID for later cleanup ──────────────────────────
    listener_pid = pid;
}

void sendNotification(const char *toUser, const char *fromUser, const char *accountNbr)
{
    char fifo_path[256];
    getFifoPath(fifo_path, toUser);

    // O_NONBLOCK: return immediately if the recipient isn't logged in
    int fd = open(fifo_path, O_WRONLY | O_NONBLOCK);
    if (fd == -1)
        return; // recipient not online — silently skip

    char msg[512];
    snprintf(msg, sizeof(msg),
             "'%s' transferred account #%s to you!", fromUser, accountNbr);

    write(fd, msg, strlen(msg));
    close(fd);
}
