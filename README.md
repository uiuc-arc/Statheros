# Statheros

Statheros is a Probabilistic programming language with fixed-point inference by Jacob Laurel, Rem Yang, Atharva Sehgal, Shubham Ugare, Sasa Misailovic at UIUC. This work was published at the 58th Design Automation Conference [paper](https://ieeexplore.ieee.org/document/9586276). 

`@inproceedings{statheros,
  title={Statheros: Compiler for Efficient Low-Precision Probabilistic Programming},
  author={Laurel, Jacob and Yang, Rem and Sehgal, Atharva and Ugare, Shubham and Misailovic, Sasa},
  booktitle={2021 58th ACM/IEEE Design Automation Conference (DAC)},
  pages={787--792},
  year={2021},
  organization={IEEE}
}`


The codebase extends from the first stable version of [AutoPPL](https://github.com/JamesYang007/autoppl) by James Yang et al. and utilizes a modified version of the [fpm](https://github.com/MikeLankamp/fpm) fixed-point library by Mike Lankamp.

## Requirements

In addition to the two aforementioned libraries which our codebase extends, to run Statheros we require the following libraries:

- `Boost` (version 1.63+)


## Code Organization

The language itself is in the `autoppl` folder, which needs the libraries in the `include` folder to run. The `experiments` folder consists of the benchmarks we implemented (in the `Models` subfolder), the experimental driver to run these models (`driver_desktop.py`), and a notebook to plot the results (`Plot_Results.ipynb`). To see how to write a probabilistic program, see the benchmarks in `experiments/Models`. To compile one simple program, see below:  

- `g++ -std=c++17 -O3 -w -I./ -I./include test.cpp` for inference on floating-point numbers (make sure to include `mh.hpp`)
- `g++ -std=c++17 -O3 -w -I./ -I./include test.cpp -D FP -D PREC=? -D DIST_VALUE_PREC=?` for inference on fixed-point numbers (make sure to include `mh_fixed.hpp`). The `PREC` and `DIST_VALUE_PREC` macros are optional; the default for both is 16. To manually define either, replace ? with a number in the range \[1, 31\]. There are also two additional macros that may be defined at compile-time: `BIG_DATA` allows the language to use a more approximate version of log to speed up computation, and `PRINT_ERROR_CHECK` prints an indication on whether the inferred values were erroneous.


## Raspberry Pi and PocketBeagle

To run Statheros on a Raspberry Pi or PocketBeagle, gcc/g++-7 is sufficient.

## Arduino

To run Statheros on an Arduino Due, one must download a more recent version of the gcc-arm compiler from https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm. We used 'arm-none-eabi-gcc-9.3.1'. Hence, instead of using the standard gcc/g++ compiler, the command to compile will be

- `arm-none-eabi-gcc-9.3.1 -std=c++17 -O3 -w -I./ -I./include test.cpp -D FP -D PREC=? -D DIST_VALUE_PREC=?`

Inside the Arduino IDE, one can configure the settings to automatically use the downloaded arm-gcc compiler instead of the default version when compiling code for the Due. One does this by going into /.arduino15/packages/arduino/hardware/sam/1.6.12/ and updating the `platforms.txt`
to use the downloaded compiler. Examples of these updated versions can be seen in the `ArduinoCompilerConfigs` directory. For more information about how to customize the Arduino settings checkout https://arduino.github.io/arduino-cli/0.19/platform-specification/ 

## Quick Start

To run a single example navigate to the `Examples` folder which contains  `FixedPoint` and `FloatingPoint` sub-directories. Each contains their respective version of the classic Burglar Alarm model. Use the following compiler flags
- `g++ -std=c++17 -O3 -w -I./ -I./include BurglarAlarm_double.cpp` (floating point version)
- `g++ -std=c++17 -O3 -w -I./ -I./include BurglarAlarm_fixed.cpp -D FP -D PREC=16 -D DIST_VALUE_PREC=16` (Fixed point version with 16 fractional bits for both PREC and DIST_VALUE_PREC)

Compiling this way will produce an executable that will run 20 separate instances of inference on this model. The results can be the `results.txt` files.

## License
See the LICENSE file. Licenses for the libraries we build upon are available [here](https://github.com/JamesYang007/autoppl/blob/master/LICENSE) and [here](https://github.com/MikeLankamp/fpm/blob/master/LICENSE)




