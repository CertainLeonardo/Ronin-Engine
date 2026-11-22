# Ronin Engine Makefile Guide

This document explains how the Ronin Engine Makefile builds the project,
what each section does, and which commands you can run from the project
directory.

## 1. What the Makefile does

The Makefile automates the build process. Instead of typing compiler
commands by hand, you run a short command such as:

``` bash
make
```

GNU Make reads the rules in the Makefile, checks which files need to be
compiled, builds object files, and links those object files with the
required libraries to create:

``` text
bin/RoninEngine.exe
```

The Makefile currently supports both C++ (`.cpp`) and C (`.c`) source
files.

## 2. Project layout

The Makefile expects this general structure:

``` text
Ronin-Engine/
├── src/                    # C and C++ source files
│   ├── main.cpp
│   └── example.c
├── include/                # Project header files
├── libs/
│   └── glfw-3.5.1.bin.WIN64/
│       ├── include/        # GLFW headers
│       └── lib-static-ucrt/ # Static GLFW library
├── build/                  # Generated object files (.o)
├── bin/                    # Final executable
├── Makefile
└── docs/                   # Project documentation
```

`build/` and `bin/` are generated output directories. The Makefile
creates them when needed. Your actual source files stay in `src/`, and
your project headers stay in `include/`.

## 3. Compilers

``` make
CXX = clang++
CC = clang
```

These variables select the compilers:

-   `CXX` is the C++ compiler. It compiles `.cpp` files and performs the
    final link.
-   `CC` is the C compiler. It compiles `.c` files.

The final link uses `clang++`. This is important because the project
contains C++ code and needs the C++ runtime.

## 4. Compiler flags

``` make
CXXFLAGS = -g -std=c++23 -O2 -Wall -Wextra
CFLAGS = -g -O2 -Wall -Wextra
```

These options control how the compilers build your source files.

  Flag           Meaning
  -------------- --------------------------------------------------------
  `-g`           Includes debugging information in the compiled output.
  `-std=c++23`   Uses the C++23 language standard for C++ files.
  `-O2`          Enables compiler optimizations.
  `-Wall`        Enables a broad set of common warnings.
  `-Wextra`      Enables additional warnings beyond `-Wall`.

You currently use both `-g` and `-O2`. That gives you debugging
information while also optimizing the program. Optimization can make
some debugging behavior less straightforward, so you may eventually want
separate Debug and Release configurations.

## 5. Directory variables

``` make
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include
```

These variables store directory names in one place.

-   `SRC_DIR` tells Make where to find source files.
-   `BUILD_DIR` tells Make where to put object files.
-   `BIN_DIR` tells Make where to put the executable.
-   `INCLUDE_DIR` tells the compiler where to find your project headers.

If you change a directory name later, you can update its variable
instead of editing every rule that uses it.

## 6. GLFW and OpenGL libraries

``` make
GLFW_DIR = libs/glfw-3.5.1.bin.WIN64

LDLIBS = -L"$(GLFW_DIR)/lib-static-ucrt" -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32

INCLUDES = -I$(INCLUDE_DIR) -I$(GLFW_DIR)/include
```

These settings tell the compiler and linker where GLFW and the required
headers and libraries are located.

### Include directories

The `-I` options add directories where the compiler searches for header
files.

-   `-I$(INCLUDE_DIR)` points to your project's `include/` directory.
-   `-I$(GLFW_DIR)/include` points to GLFW's headers.

For example, your source code can include GLFW with:

``` cpp
#include <GLFW/glfw3.h>
```

### Linker libraries

The `LDLIBS` variable contains libraries used when the linker creates
the executable.

  -----------------------------------------------------------------------
  Option                              Purpose
  ----------------------------------- -----------------------------------
  `-L...`                             Adds a directory where the linker
                                      searches for libraries.

  `-lglfw3`                           Links the static GLFW library.

  `-lopengl32`                        Links the Windows OpenGL library.

  `-lgdi32`                           Links Windows Graphics Device
                                      Interface functions.

  `-luser32`                          Links Windows user interface and
                                      window-management functions.

  `-lshell32`                         Links Windows Shell API functions.
  -----------------------------------------------------------------------

The GLFW library path in this Makefile points to the `lib-static-ucrt`
folder. This assumes your compiler environment and the GLFW library use
a compatible runtime.

## 7. Finding source files

``` make
CPP_SRC = $(wildcard $(SRC_DIR)/*.cpp)
C_SRC = $(wildcard $(SRC_DIR)/*.c)
```

`wildcard` asks Make to find files matching a pattern.

-   `$(SRC_DIR)/*.cpp` finds C++ source files directly inside `src/`.
-   `$(SRC_DIR)/*.c` finds C source files directly inside `src/`.

This means you can add another `.cpp` or `.c` file to `src/` without
manually listing it in the Makefile.

This pattern only searches the top level of `src/`. It does not
automatically search nested folders such as `src/graphics/`.

## 8. Object files

``` make
CPP_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SRC))
C_OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRC))

OBJ = $(CPP_OBJ) $(C_OBJ)
```

Before the linker creates the executable, the compiler turns each source
file into an object file.

An object file contains compiled machine-code sections, but it is not
the final standalone program.

For example:

``` text
src/main.cpp  -> build/main.o
src/example.c -> build/example.o
```

`patsubst` transforms each source path into its matching object-file
path. `OBJ` combines the C and C++ object-file lists so the link rule
can use them together.

## 9. The executable target

``` make
TARGET = $(BIN_DIR)/RoninEngine.exe
```

This variable defines the final output file:

``` text
bin/RoninEngine.exe
```

The main build rule is:

``` make
all: $(TARGET)

$(TARGET): $(OBJ) | $(BIN_DIR)
    $(CXX) $(OBJ) -o $@ $(LDLIBS)
```

Here is what happens:

1.  `all` depends on `$(TARGET)`, so running `make` asks Make to build
    the executable.
2.  The executable depends on all object files in `$(OBJ)`.
3.  `| $(BIN_DIR)` is an order-only prerequisite. It makes sure the
    `bin/` directory exists before the link command runs. Changes to the
    directory itself do not make the executable out of date.
4.  The recipe runs `clang++` to link the object files and libraries.
5.  `-o $@` tells the linker where to write the output. In this rule,
    `$@` means the target filename, `bin/RoninEngine.exe`.

The linker combines the compiled object files and the required libraries
into the final executable.

## 10. Compilation rules

### C++ files

``` make
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
    $(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
```

This is a pattern rule. It tells Make how to create an object file from
a C++ source file.

For example, to create `build/main.o`, Make uses `src/main.cpp`.

Automatic variables used here:

-   `$@` means the target being created, such as `build/main.o`.
-   `$<` means the first prerequisite, such as `src/main.cpp`.
-   `-c` tells the compiler to compile the source file without linking
    it yet.
-   `-o $@` tells the compiler where to write the object file.

`| $(BUILD_DIR)` ensures the `build/` directory exists before
compilation.

### C files

``` make
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
```

This rule does the same job for C source files, using `clang` and
`CFLAGS`.

## 11. Creating output directories

``` make
$(BUILD_DIR):
    mkdir -p $(BUILD_DIR)

$(BIN_DIR):
    mkdir -p $(BIN_DIR)
```

These rules create the output directories when they do not exist.

The `-p` option tells `mkdir` to create missing parent directories and
avoid an error if the directory already exists.

These commands use Unix-style syntax. They work in environments such as
MSYS2 or Git Bash on Windows. If you run Make from a different Windows
shell or toolchain, the directory commands may need to change.

## 12. Commands you can run

Run these commands from the Ronin Engine project root, where the
Makefile is located.

  -----------------------------------------------------------------------
  Command                             What it does
  ----------------------------------- -----------------------------------
  `make`                              Builds the executable.

  `make all`                          Runs the default build target.

  `make run`                          Builds the executable if needed,
                                      then launches it.

  `make clean`                        Deletes the executable in `bin/`.

  `make clean-objects`                Deletes the object files listed by
                                      the current source-file patterns.
  -----------------------------------------------------------------------

### Build the project

``` bash
make
```

Make checks the target and its prerequisites. If an object file is
missing or older than its source file, Make rebuilds it. It then links
the object files if the executable needs updating.

### Run the project

``` bash
make run
```

The `run` target depends on the executable. Make builds it first if
necessary, then runs:

``` bash
./bin/RoninEngine.exe
```

### Remove the executable

``` bash
make clean
```

This removes `bin/RoninEngine.exe`. It does not remove the object files
in `build/`.

### Remove object files

``` bash
make clean-objects
```

This removes the object files currently listed in `OBJ`. The next build
will compile those source files again.

## 13. Important Makefile details to improve later

The current Makefile is a useful starting point. Keep these limitations
in mind as Ronin Engine grows.

### Header-file dependencies

The object-file rules currently depend on the `.cpp` or `.c` source
file, but they do not explicitly depend on included headers.

For example, changing `include/Renderer.hpp` may not cause `main.cpp` to
recompile automatically.

A common improvement is to ask Clang to generate dependency files with
`-MMD -MP`, then include those generated `.d` files in the Makefile.
This lets Make track header dependencies.

### Nested source directories

The `wildcard` patterns only find files directly inside `src/`. If you
later organize code into folders such as `src/renderer/` or
`src/platform/`, you will need to expand the source-file search.

### Clean target behavior

`make clean` currently removes only the executable. It leaves object
files behind.

That is intentional based on the current recipe. You can choose to make
`clean` remove both the executable and the contents of `build/` later.

### Declare every command target phony

The Makefile currently declares:

``` make
.PHONY: all run clean
```

`clean-objects` is also a command target, so it should be added:

``` make
.PHONY: all run clean clean-objects
```

A phony target is a command name, not a real file that Make should
compare timestamps against.

### Debug and Release configurations

The current flags combine debugging information (`-g`) and optimization
(`-O2`). As the engine grows, you may want separate configurations so
you can choose between easier debugging and optimized builds.

## 14. The complete build flow

When you run `make`, the process looks like this:

``` text
                 make
                   |
                   v
              Build target
                   |
                   v
          Find .cpp and .c files
                   |
                   v
          Compile source files
                   |
                   v
       Create .o files in build/
                   |
                   v
        Link object files and libraries
                   |
                   v
       Create bin/RoninEngine.exe
```

The key idea is that compilation and linking are separate stages.

1.  **Compilation:** Clang translates each source file into an object
    file.
2.  **Linking:** Clang++ combines the object files and libraries into
    the final executable.

Make manages the order of these steps and skips work when the existing
outputs are already up to date.

## 15. Quick reference

``` bash
make                 # Build Ronin Engine
make run             # Build and run Ronin Engine
make clean           # Delete the executable
make clean-objects   # Delete generated object files
```

The Makefile keeps the workflow simple: source code lives in `src/`,
headers live in `include/`, intermediate object files go in `build/`,
and the finished executable goes in `bin/`.
