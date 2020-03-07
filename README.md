
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

