# Compiling

## What tools do you need?

This project is currently known to compile only on **Linux** systems. If you're on **Windows**, you can use [WSL (Windows Subsystem for Linux)](https://learn.microsoft.com/en-us/windows/wsl/) to run a compatible environment.

You'll need the following tools installed:

* `gcc`
* `mtools`
* `nasm`
* `make`
* `Open Watcom 2.0`

Install them using your system's package manager: 

> [!NOTE]
> For Open Watcom 2.0, you will need to install it from it's [GitHub repository](https://github.com/open-watcom/open-watcom-v2), enable 16-bit compiling and set the host to linux 32-bits.

### Debian/Ubuntu:

```bash
sudo apt install gcc mtools nasm make -y
```

### Arch Linux (or derivatives):

Type this in your terminal:

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
