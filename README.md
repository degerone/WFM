# L103


compilare con

g++ DAQ.cpp `root-config --cflags --glibs` -o DAQ

g++ ANAL.cpp waveform.cpp `root-config --cflags --glibs` -o ANAL

esecuzione:

ls *dat >> out.dat

./DAQ out.dat out.root

./ANAL out.root demo.root


DA IMPLEMENTARE:

Integrale della funzione esponenziale fittata

Trasformata di Fourier

Riconoscimento impulsi

Pile up

Eventi saturati