#######################################################################
# FFT Test Plot
#
# Author: Henrik Langer (henni19790@gmail.com)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#######################################################################
 
sinID = fopen('data/fft_ifft/sine.txt', 'r');
fftID = fopen('data/fft_ifft/spectrum_mag.txt', 'r');
fftPhaseID = fopen('data/fft_ifft/spectrum_phase.txt', 'r');
ifftID = fopen('data/fft_ifft/ifft_spectrum.txt', 'r');
formatSpec = '%f';
sinData = fscanf(sinID, formatSpec);
fftData = fscanf(fftID, formatSpec);
fftDataPhase = fscanf(fftPhaseID, formatSpec);
ifftData = fscanf(ifftID, formatSpec);
fclose(sinID);
fclose(fftID);
fclose(fftPhaseID);
fclose(ifftID);

N = 512;
Fs = 48000;
w = (0:255)/256*(Fs/2);
t = 0:1/N:1 - 1/N;
fSampleIndex = 0:N-1;
size_period_sine = 64;

hf = figure(1);
clf();
subplot(3,1,1)
stem(t(1:size_period_sine*2), sinData(1:size_period_sine*2))
grid on
title('Generated 3 kHz sine sampled with 48 kHz')
xlabel('Time (s)')
ylabel('Amplitude')
subplot(3,1,2)
plot(w, log10(fftData(1:256)))
grid on
set(gca,'yscal','log')
title('Magnitude Spectrum from FFT with block size 512 (N)')
xlabel('Frequency (Hz)')
ylabel('Magnitude (dBFS)')
%subplot(4,1,3)
%plot(w, fftDataPhase(1:256))
%grid on
%set(gca,'xscal','log')
%title('Phase Spectrum (N = 512)')
%xlabel('Sample Index')
%ylabel('Phase')
subplot(3,1,3)
stem(t(1:size_period_sine*2), ifftData(1:size_period_sine*2))
grid on
title('Plot of reconstructed sine')
xlabel('Time (s)')
ylabel('Amplitude')

set(hf,'PaperOrientation','landscape');
set(hf,'PaperUnits','normalized');
set(hf,'PaperPosition', [0 0 1 1]);
print (hf, '-dpdf', 'fft_test_sine.pdf');
