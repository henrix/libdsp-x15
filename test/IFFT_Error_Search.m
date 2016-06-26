N = 16384;
N_sine = N/4;
f_sine = 2048;
t = 0:16384-1;
sig_sine = sin(2*pi*f_sine / N_sine);
plot(sig_sine(1:50))

pause(10)