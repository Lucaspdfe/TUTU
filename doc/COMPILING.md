# Compiling

## What tools do you need?

This project is currently known to compile only on **Linux** systems. If you're on **Windows**, you can use [WSL (Windows Subsystem for Linux)](https://learn.microsoft.com/en-us/windows/wsl/) to run a compatible environment.

You'll need the following tools installed:

* `gcc`
* `mtools`
* `nasm`
* `make`

Install them using your system's package manager:

### Debian/Ubuntu:

```bash
sudo apt install gcc mtools nasm make -y
```

### Arch Linux (or derivatives):

Type this in your command prompt:

```bash
sudo pacman -S base-devel gcc mtools nasm make
```

## Compiling the OS

To compile the operating system, simply run:

```bash
make
```

After a successful build, you'll find the output file here:

```
build/main_floppy.img
```
