# spn_sw

## Intro

This repository is linked to [spn fpga](https://github.com/gennartan/Arithmetic_circuit_Posit_FPGA). It contains the software to simulate and compute the expected output of a given SPN.

In addition to this, it computes an error bound  for the SPN given a representation of numbers (Posit or Float of parametrisable size)

## Usage

```
cd build
make
./exe [in_filename.spn] [out_filename.txt]
```

**in_filename** should be on the form of spn format. This file format can be generated from PSDD using a python script available in **psdd2spn** folder. Without input filename, an input example is loaded.

**out_filename** is not mandatory. Without output filename, output is redirected to stdout.
