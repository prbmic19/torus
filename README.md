# Oxell Torus

Torus is a hobby operating system under the Oxell project, created primarily as a learning experience and secondarily because I apparently needed something large enough to sink my free time into.

I intend to build a bootloader, kernel, and userspace from scratch. For now, Torus targets only x86-64, though multi-architecture support is planned.

## Current Status

Very early development.

The current kernel just boots and enters an idle loop.

## Building

A Linux environment is required.

<!-- Worded it this way so there is no need to reword it once Torus actually targets other architectures. -->
The build system requires a cross toolchain for the target architecture with ELF output.

### Prerequisites

You will need the following installed:

- GCC
- GNU Binutils
- GNU Make

Minimum supported tool versions have not yet been determined. Using a reasonably recent toolchain is recommended.

Clone the repo and run `make config`. After you have configured the build system, simply run `make`.

```sh
git clone https://github.com/prbmic19/torus
cd torus
make config
make
```

## Running

You will need QEMU installed.

Running Torus should be as simple as `make run` (it will build first if needed).

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.
