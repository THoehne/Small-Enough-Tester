#include "set.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>

#include "set_asserts.h"

#ifdef COLORIZED

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#else

#define ANSI_COLOR_RED
#define ANSI_COLOR_GREEN
#define ANSI_COLOR_YELLOW
#define ANSI_COLOR_BLUE
#define ANSI_COLOR_MAGENTA
#define ANSI_COLOR_CYAN
#define ANSI_COLOR_RESET

#endif

// clang-format off
#define BANNER                                                                 \
    ""                                                                         \
    "   _____                 _ _   _____                        _       "     \
    "_____        _            \n"                                             \
    "  /  ___|               | | | |  ___|                      | |     |_   " \
    "_|      | |           \n"                                                 \
    "  \\ `--. _ __ ___   __ _| | | | |__ _ __   ___  _   _  __ _| |__     | " \
    "| ___ ___| |_ ___ _ __ \n"                                                \
    "   `--. | '_ ` _ \\ / _` | | | |  __| '_ \\ / _ \\| | | |/ _` | '_ \\   " \
    " | |/ _ / __| __/ _ | '__|\n"                                             \
    "  /\\__/ | | | | | | (_| | | | | |__| | | | (_) | |_| | (_| | | | |   | " \
    "|  __\\__ | ||  __| |   \n"                                               \
    "  \\____/|_| |_| |_|\\__,_|_|_| \\____|_| |_|\\___/ \\__,_|\\__, |_| "    \
    "|_|   \\_/\\___|___/\\__\\___|_|   \n"                                    \
    "                                                       __/ |            " \
    "                      \n"                                                 \
    "                                                      |___/             " \
    "                      \n"                                                 \
    ""
//clang-format on


static void log_test_summary(struct SETest *test)
{

    double time = test->time.tv_sec * 1000.0 + test->time.tv_usec / 1000.0;
    if (test->passed)
    {
        printf(ANSI_COLOR_GREEN"[ passed ] %s (%.3f ms)" ANSI_COLOR_RESET"\n",
               test->name, time);
    }
    else
    {
        printf( ANSI_COLOR_RED"[ failed ] %s (%.3f ms)\n" 
               "    "ANSI_COLOR_CYAN"Reason: %s\n\n"ANSI_COLOR_RESET,
               test->name, time, test->error_msg);
    }
}

static void log_abnormal_termination(int status)
{
    fprintf(stderr,
            ANSI_COLOR_RED "A test terminated with signal: %d" ANSI_COLOR_RESET,
            WTERMSIG(status));
}

static void dispatch_single_test(struct SETest *test)
{
    pid_t testPID = fork();

    struct timeval stop, start;
    if (testPID == 0)
    {
        gettimeofday(&start, NULL);
        test->function(test);
        gettimeofday(&stop, NULL);
        
        test->time.tv_sec = stop.tv_sec - start.tv_sec;
        test->time.tv_usec = stop.tv_usec - start.tv_usec;

        log_test_summary(test);
        set_free_all();
        exit(0);
    }
}

static void dispatch_tests(struct SETSuit *suit)
{
    printf("\n\n==== Running suit: %s (%d) ====\n\n", suit->name, suit->len);

    if (suit->setup != NULL && !suit->setup()) 
    {
        fprintf(stderr, "Suit not executed: Setup failed.\n");
    }

    struct SETest *test = suit->tests;
    for (int i = 0; i < suit->len; i++)
    {
        dispatch_single_test(&test[i]);
    }

    int status = 0;
    // Wait for all tests.
    while (wait(&status) > 0)
    {
        if (WTERMSIG(status) != 0)
            log_abnormal_termination(status);
    };
    if (shmdt(suit->tests) < 0)
    {
        fprintf(stderr, "%s\n", "Couldn't detach suit space after dispatch.");
        perror("shmat");
        exit(1);
    }
    if (shmctl(suit->shm_key, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "%s\n", "Couldn't remove suit space after dispatch.");
        perror("shctl");
        exit(1);
    }

    if (suit->tear_down != NULL && !suit->tear_down()) 
    {
        fprintf(stderr, "Teardown failed.\n");
    }
}

static void dispatch_test_suits(struct SETSuit **suits, int counter)
{
    for (int i = 0; i < counter; i++)
    {
        dispatch_tests(suits[i]);
        munmap((void*)suits[i], sizeof(struct SETSuit));
    }
}

int main(int argc, char **argv)
{
#ifndef NO_BANNER
    printf("\n\n%s", BANNER);
#endif /* ifndef NO_BANNER */

    if (!set_up())
    {
        fprintf(stderr, "%s\n", "Setup call returned with error. Aborting execution.");
        return 1;
    }

    int counter = 0;

    set_bundle_suits(NULL, &counter, true);

    struct SETSuit *suits[counter];

    counter = 0;

    set_bundle_suits(suits, &counter, false);

    dispatch_test_suits(suits, counter);

    if (!tear_down())
    {
        fprintf(stderr, "%s\n", "Teardown returned with error.");
        return 1;
    }
    return 0;
}
