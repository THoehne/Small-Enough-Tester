#include "set.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set_asserts.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

// clang-format off
#define BANNER \
"" \
"   _____                 _ _   _____                        _       _____        _            \n"\
"  /  ___|               | | | |  ___|                      | |     |_   _|      | |           \n"\
"  \\ `--. _ __ ___   __ _| | | | |__ _ __   ___  _   _  __ _| |__     | | ___ ___| |_ ___ _ __ \n"\
"   `--. | '_ ` _ \\ / _` | | | |  __| '_ \\ / _ \\| | | |/ _` | '_ \\    | |/ _ / __| __/ _ | '__|\n"\
"  /\\__/ | | | | | | (_| | | | | |__| | | | (_) | |_| | (_| | | | |   | |  __\\__ | ||  __| |   \n"\
"  \\____/|_| |_| |_|\\__,_|_|_| \\____|_| |_|\\___/ \\__,_|\\__, |_| |_|   \\_/\\___|___/\\__\\___|_|   \n"\
"                                                       __/ |                                  \n"\
"                                                      |___/                                   \n"\
""
//clang-format on

char* format_string(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t size = vsnprintf(NULL, 0, fmt, args);
	char* out = (char*)malloc(size + 1);

	if (!out) {
		fprintf(stderr, "%s", "Failed to allocate for format string.\n");
	}

	va_end(args);

	va_start(args, fmt);
	vsnprintf(out, size + 1, fmt, args);
	va_end(args);

	return out;
}

static void log_test_start(const char* name) { printf("Running test: %s", name); }

static void log_test_summary(struct SETest* test)
{
	if (test->passed) {
#ifdef COLORIZED
		printf(ANSI_COLOR_GREEN "    - passed.\n" ANSI_COLOR_RESET);
#else
		printf("    - passed.\n");
#endif
	} else {
#ifdef COLORIZED
		printf(ANSI_COLOR_RED "    - failed.\n" ANSI_COLOR_RESET "    Reason: \n    %s\n\n",
			test->error_msg);
#else
		printf("    - failed.\n    Reason: \n    %s\n\n", test->error_msg);
#endif
	}
}

static void dispatch_tests(struct SETSuit* suit)
{
	printf("\n\n==== Running suit: %s ====\n\n", suit->name);
	for (int i = 0; i < suit->len; i++) {
		struct SETest* test = suit->tests[i];
		log_test_start(test->name);
		test->function(test);
		log_test_summary(test);
		fflush(stdout);
	}
}


static void free_suit(struct SETSuit* suit)
{
	for (int i = 0; i < suit->len; i++) {
		struct SETest* test = suit->tests[i];
		free((void*)test->error_msg);
		free((void*)test);
	}

	free(suit->tests);
	free(suit);
}


static void dispatch_test_suits(struct SETSuit** suits, int counter)
{
	for (int i = 0; i < counter; i++)
	{
		dispatch_tests(suits[i]);
		free_suit(suits[i]);
	}
}

int main(int argc, char** argv)
{
	#ifndef NO_BANNER
	
	printf("\n\n%s", BANNER);
	#endif /* ifndef NO_BANNER */
	
	int counter = 0;

	set_bundle_suits(NULL, &counter, true);

	struct SETSuit ** suits = (struct SETSuit**)malloc(counter * sizeof(struct SETSuit*));
	
	counter = 0;

	set_bundle_suits(suits, &counter, false);

	dispatch_test_suits(suits, counter);

	free(suits);
}
