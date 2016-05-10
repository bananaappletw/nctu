# Unix hw3

# There are four files in this project

- main.cpp
- shell.cpp
- shell.h
- Makefile

# Basic 

- [basic] Execute a single command.
- [basic] Properly block or unblock signals.
- [basic] Replace standard input/output of a process using the redirection operators (< and >).
- [basic] Setup foreground process group and background process groups.
- [basic] Create pipeline for commands separated by the pipe operator (|), and put the commands into the same process group.

# Optional

- [optional] Manipulate environment variables: provide commands like export and unset to add and remove environment variables, respectively.
- [optional] Expand of the * and ? operators.
- [optional] Job control: support process suspension using Ctrl-Z, and jobs, fg and bg command.

# Build

```bash
make
```

# Usage

```bash
./hw3
```
# Features

- help
- jobs
- fg
- bg
- fg [ %job_id ]
- bg [ %job_id ]
- export [ name ] [ value ]
- unset [ name ]

