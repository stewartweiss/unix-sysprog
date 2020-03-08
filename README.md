# UNIX System Programming "Demo" Programs

## About This Repository
I taught a course in UNIX System Programming at Hunter College for many years,
using in part an excellent book by Bruce Molay, and in part, my own research,
to develop the course.

With each chapter, I created programs that were designed to demonstrate the
concepts. The intention was that a student interested in this subject could
tinker with the code to see how modifying parameters and inputs would affect
program behavior. I call these "demo" programs for obvious reasons.

The programs in this repository are organized by chapter. At present only
Chapter 5 is included, simply because I am using it in an operating systems course
to show how signals and asynchronous I/O work.

I would welcome suggestions, corrections, discovery of bugs, and other improvements. At present there is no CONTRIBUTING file, because the projectis small and
just getting underway.  If you see something that needs improvement,
create an issue with as much detail as possible. 
Please ensure your description is clear and has sufficient instructions to be able to reproduce the issue.

## How To Use This Repository

Each chapter is a self-contained collection of programs with its own separate
`README` file.  You should read that file before doing anything in that chapter.

All program code depends on the files in the `utilities` directory. To build
the programs in any chapter, you must set up your environment as follows:

1. After cloning the repository, `cd` into the utilities directory and run `make`:

   ```bash
      $ cd utilities
      $ make
   ```

1. Then run `make install` in that directory:

   ```bash
      $ make install
   ```
   This copies the header file created by `make` into the `include` directory in
   this repository, and the static library `libutils.a` into the `lib`
   directory.

1. Now `cd` into the chapter you'd like to build and run `make` there:

   ```bash
     $ cd ../chapter05
     $ make
   ```

