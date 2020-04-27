# spn_sw

## Intro 

This repository is linked to [spn fpga](https://github.com/gennartan/Arithmetic_circuit_Posit_FPGA). It contains the software to simulate and compute the expected output of a given SPN.

In addition to this, it computes an error bound  for the SPN given a representation of numbers (Posit or Float of parametrisable size)

## Usage

```
cd build
make
./exe <in_filename.spn> [out_filename.txt]
```
