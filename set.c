#include "set.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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

char *format_string(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t size = vsnprintf(NULL, 0, fmt, args);
    char *out = (char *)malloc(size + 1);

    if (!out)
    {
        fprintf(stderr, "%s", "Failed to allocate for format string.\n");
    }

    va_end(args);

    va_start(args, fmt);
    vsnprintf(out, size + 1, fmt, args);
    va_end(args);

    return out;
}

int create_shared_suit_space(size_t size)
{
    int suit_space_id = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);

    if (suit_space_id == -1)
    {
        fprintf(stdout, "Couldn't create suite space of size: %lu\n", size);
        perror("shmget");
        exit(1);
    }

    return suit_space_id;
}


static void log_test_summary(struct SETest *test)
{
    if (test->passed)
    {
        printf("Test: %s" ANSI_COLOR_GREEN "    - passed." ANSI_COLOR_RESET"\n",
               test->name);
    }
    else
    {
        printf("Test: %s" ANSI_COLOR_RED "    - failed.\n" ANSI_COLOR_RESET
               "    Reason: \n    %s\n\n",
               test->name, test->error_msg);
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

    if (testPID == 0)
    {
        test->function(test);
        log_test_summary(test);
        exit(0);
    }
}

static void dispatch_tests(struct SETSuit *suit)
{
    printf("\n\n==== Running suit: %s (%d) ====\n\n", suit->name, suit->len);
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

    int counter = 0;

    set_bundle_suits(NULL, &counter, true);

    struct SETSuit *suits[counter];

    counter = 0;

    set_bundle_suits(suits, &counter, false);

    dispatch_test_suits(suits, counter);
}
