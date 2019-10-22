# L103

- waveform.h and waveform.cpp are the include and source files for class waveform which currently includes all methods for analyzing waveforms.

- test.cpp is a macro for testing new methods of waveform class.

- ANAL.cpp is a preliminary analysis macro

How to compile:

g++ test.cpp waveform.cpp `root-config --cflags --glibs` -o test
g++ ANAL.cpp waveform.cpp `root-config --cflags --glibs` -o ANAL

How to execute:

./test ThXXX.root test_output.root
./ANAL ThXXX.root ANAL_output.root

TO DO:

  - baseline		        ->OK
  - baseline rms		     
  - integrale
  - amplitude			->OK
  - get max			->OK
  - get min			->OK
  - template			->OK
  - fit				->??
  - integrale del fit
  - convolution / filtering	->OK
  - macro per analisi generica	->first version
  - macro per visualizzazione
  - saturazione
  - FFT?

