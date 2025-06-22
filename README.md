# Small-Enough-Tester

Tiny C testing framework.

## Build

Building is quite simple. To build a test `test_fac.c` just pick your favourite compiler and run.

    gcc Small-Enough-Tester/set.c test_fac.c -I Small-Enough-Tester/ -D COLORIZED -lm -o test_fac

Running a test is equally simple just execute the compiled binary.

    ./test_fac

## LICENSE

This software is licensed under GPL v3.0 . For more information see [LICENSE](LICENSE).
