# Compiling

## What tools do you need?

This project is currently known to compile only on **Linux** systems. If you're on **Windows**, you can use [WSL (Windows Subsystem for Linux)](https://learn.microsoft.com/en-us/windows/wsl/) to run a compatible environment.

You'll need the following tools installed:

* `gcc`
* `mtools`
* `nasm`
* `make`
* `dosfstools`
* `Open Watcom 2.0`

Install them using your package manager of choice: 

> [!NOTE]
> For Open Watcom 2.0, you will need to install it from it's [GitHub repository](https://github.com/open-watcom/open-watcom-v2), enable 16-bit compiling and set the host to linux 32-bits.

### Debian/Ubuntu:

```
sudo apt install gcc mtools nasm make dosfstools -y
```

### Arch Linux (or derivatives):

Type this in your terminal:

```
sudo pacman -S base-devel gcc mtools nasm make dosfstools
```

## Compiling the OS

To compile the operating system, simply run:

```
make
```

After a successful build, you'll find the output file here:

```
build/main_floppy.img
```

## Testing

If you want to run the OS, you can install `qemu-system` from your package manager of choice, then run: 

```
make run
```
