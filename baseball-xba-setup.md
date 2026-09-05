# baseball-xba — Project Setup Guide

A C++ program that computes a player's "expected" batting stats from the physics
of their batted balls (exit velocity + launch angle), then displays those stats
alongside league percentile rankings — similar in spirit to Baseball Savant.

Environment: Linux (EndeavourOS), VS Code

---

## 1. Install core build tools

```bash
sudo pacman -S gcc cmake make git python python-pip
```

**What this does:**
- `gcc` — includes `g++`, the C++ compiler that turns your `.cpp` files into a runnable program.
- `cmake` — a build system generator; lets you describe your project once and have it generate the right build files for your platform.
- `make` — the tool that actually runs the compilation steps CMake generates.
- `git` — version control, for tracking changes and building a real commit history.
- `python` / `python-pip` — needed for the data-pulling script (pybaseball is a Python library).

**Verify it worked:**
```bash
g++ --version
cmake --version
```

---

## 2. Install VS Code extensions

- **C/C++** (Microsoft) — IntelliSense, syntax highlighting, debugging support for C++.
- **CMake Tools** (Microsoft) — lets VS Code build/run the project via CMake directly from the editor (adds Build/Run buttons to the status bar).
- *(Optional)* **CodeLLDB** — alternative debugger; the C/C++ extension's built-in GDB support is enough to start.

---

## 3. Set up a Python virtual environment for pybaseball

```bash
python -m venv venv
source venv/bin/activate
pip install pybaseball pandas
```

**What this does, step by step:**

- `python -m venv venv` — creates a self-contained folder (`venv/`) inside the project holding its own private copy of Python and its own package space, completely separate from the system-wide Python install.
- `source venv/bin/activate` — "turns on" that isolated environment for the current terminal session. Once active, your prompt shows `(venv)` and any `pip install` goes into the venv folder, not system-wide.
- `pip install pybaseball pandas` — installs the two packages needed:
  - `pybaseball` — pulls Statcast data (exit velocity, launch angle, outcomes, etc.) from MLB.
  - `pandas` — data-handling library pybaseball depends on to return clean, table-shaped data you can export to CSV.

**Why bother with a venv instead of installing globally?**
- Arch/EndeavourOS often marks system Python as "externally managed" (PEP 668), which blocks plain `pip install` outside a venv entirely — so this is often required, not just best practice.
- Keeps dependencies reproducible: anyone (including future you) can recreate the exact environment without guessing what's installed system-wide.
- Avoids version conflicts with any other Python project on the machine.

**Important:** you must re-run `source venv/bin/activate` every time you open a new terminal to work on the Python data-pulling script. It does not stay active permanently.

---

## 4. Project folder structure

```
baseball-xba/
├── src/          # C++ source files (.cpp)
├── include/      # header files (.hpp/.h), including third-party single-header libs
├── data/         # CSVs pulled from pybaseball
├── scripts/      # Python data-pulling script(s)
├── venv/         # Python virtual environment (git-ignored)
├── build/        # compiled output (git-ignored)
└── CMakeLists.txt
```

This keeps data, Python glue code, and C++ source cleanly separated — and mirrors how a real C++ project is typically organized.

---

## 5. Get the rapidcsv header library

Download `rapidcsv.hpp` from its GitHub repo and place it directly in `include/`.
No package manager needed — just `#include "rapidcsv.hpp"` in C++ code once it's there.

This avoids setting up a full package manager (vcpkg/Conan) for the very first dependency, keeping early setup friction low.

---

## 6. Verify the build works (hello-world check)

Before writing any real logic, confirm the toolchain works end-to-end:
- A minimal `CMakeLists.txt` that compiles a single `main.cpp`
- `main.cpp` just prints `"Hello, baseball"` and confirms `rapidcsv.hpp` can be found/included

Build via VS Code's CMake Tools (Build/Run buttons in the status bar) or manually:
```bash
mkdir build && cd build
cmake ..
make
```

Getting this trivial build working first avoids debugging build issues and data logic at the same time later.

---

## 7. Initialize git

```bash
git init
```

Add a `.gitignore` with at least:
```
build/
venv/
data/*.csv
```

Make an initial commit. Starting version control from commit #1 matters — a real, visible commit history is part of what makes this project credible on a resume/GitHub profile.

---

## Project name

**Folder / repo name:** `baseball-xba`

Chosen for clarity — unambiguous to anyone skimming a repo list, and won't need renaming if the project later expands beyond batting average to other expected stats.
