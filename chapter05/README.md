## How to Use This Set of Programs

March 21, 2013

This directory contains demo programs that demonstrate the ideas in
Chapter 5 of the UNIX System Programming lecture notes. This README explains
the order in which you should study these programs.

The programs should be studied in the following order:

|                  |                  |
|:-----------      |:---------------  |
|`simplevi_2.c`        | -- a simple vi editor |
|`term_demo1.c`       | -- adds to `canon_mode_test1.c` arguments for controlling blocking/non-blocking, in conjunction|
|`signal_demo1.c`      | -- demonstrates simple signal handling with `signal()`|
|`signal_demo2.c`      | -- demonstrates ignoring signals with `signal()`|
|`restart_demo.c`      | -- shows the effect of the `SA_RESTART` flag in a simple way|
|`sigaction_demo1.c`   | -- shows how to use sigaction's `sig_info_t` struct|
|`sigaction_demo2.c`   | -- shows how to use sigaction and control ways to handle pending signals|
|`canon_mode_test2.c` | -- adds signal handling to `canon_mode_test1.c`|
|`term_demo2.c`       | -- adds signal handling to `term_demo1.c`|
|`procmaskdemo.c`      | -- shows how to mask signals with `procmask()`|


To compile and build, you need to do the following:
1. In the `utilities` directory
