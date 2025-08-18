# Building the OS

## Prerequisites

- A computer running Linux[^1].
- Package manager of your choice.

To build the OS, you'll need to install these packages:

- `make`
- `nasm`
- `mtools`
- `qemu-system-x86_64`
- `watcom`[^2]

[^1]: Windows nor macOS have been tested, feel free to try them out.
[^2]: The Watcom compiler is available for download from the [github repository](https://github.com/open-watcom/open-watcom-v2). Make sure to install it with 16 bit support and the host as 32-bits.

## How to Build

To build the OS, execute the following command: `make`

## Running the OS

To run the OS, run the following command: `make run`
