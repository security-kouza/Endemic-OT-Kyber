# A C++ Library for Endemic OT Using CRYSTALS-Kyber

This library provides mainly an implementation of the Kyber-based endemic OT (or MROT) protocol, which is proposed by Masny and Rindal in their CCS 2019 paper [[eprint](https://eprint.iacr.org/2019/706)]. This work used codes from [LibOTe](https://github.com/osu-crypto/libOTe).

## Prerequisites

- AVX2 compatible CPU(s)
- OpenSSL Library (Tested on version `3.4.0`)
- Basic C/C++ development tools (like compilers, CMake, Git...)

## Usage

You can integrate this library directly using CMake. (eg. `add_subdiretory`, ...)

To manually link the library, run `cmake -B build && cd build/ && make` to build, and link the compiled `libEOTKyber.a`. The header files are under `include/`.


## Test/Benchmark

To build the test program, run `cmake -DENABLE_TEST=ON -B build && cd build/ && make`

The test binary `EOTKyber-test` is built under `build/bin` (or other `<path-to-build>/bin`). It includes two tests:

1. `EndemicOT.UnitTest`: A unit test with a performance benchmark for the basic functionalities of the Kyber-based Endemic OT module. The sender and the receiver functions share the same thread and are executed sequentially.

2. `EndemicOT.BatchTest`: A test with a performance benchmark for the `EndemicOT::batch_send` and `EndemicOT::batch_receive` functions, which integrates with the `Socket` class to batch execute the OT protocol. Each party is given a single thread and benchmarked separately. Messages between the parties are passed via local TCP socket(, which is provided by the `Socket` class).

