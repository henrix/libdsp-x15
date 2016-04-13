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
 
# Sine test
sinID = fopen('data/1kHz_sin.txt', 'r');
fftID = fopen('data/fft_1kHz_sin.txt', 'r');
formatSpec = '%f';
sinData = fscanf(sinID, formatSpec);
fftData = fscanf(fftID, formatSpec);
fclose(sinID);
fclose(fftID);

N = 16384;
t = 0:1/16384:1 - 1/16384;
f = -(N/2):0.5:(N/2)-0.5;
[minFFT, minFFT_i] = min(fftData);
[maxFFT, maxFFT_i] = max(fftData);

figure(1)
clf();
subplot(2,1,1)
plot(t(1:50), sinData(1:50))
grid on
title('Plot of 1kHz sine with 16384 Hz sampling rate')
xlabel('Time (s)')
ylabel('Amplitude')
subplot(2,1,2)
plot(f, fftData)
strMin = [num2str(f(minFFT_i)), ' Hz'];
strMax = [num2str(f(maxFFT_i)), ' Hz'];
text(f(minFFT_i),minFFT,strMin,'HorizontalAlignment','left');
text(f(maxFFT_i),maxFFT,strMax,'HorizontalAlignment','left');
line()
grid on
title('Plot of computed sine spectrum using DSP FFT with N = 16384')
xlabel('Frequency (Hz)')
ylabel('Amplitude')

print fft_test_sine.pdf

# Rect test
rectID = fopen('data/1kHz_rect.txt', 'r');
fftID = fopen('data/fft_1kHz_rect.txt', 'r');
formatSpec = '%f';
rectData = fscanf(rectID, formatSpec);
fftData = fscanf(fftID, formatSpec);
fclose(rectID);
fclose(fftID);

N = 16384;
t = 0:1/16384:1 - 1/16384;
f = 0:0.5:16384 - 0.5;
[minFFT, minFFT_i] = min(fftData);
[maxFFT, maxFFT_i] = max(fftData);

figure(2)
clf();
subplot(2,1,1)
plot(t(1:50), rectData(1:50))
grid on
title('Plot of 1kHz rect with 16384 Hz sampling rate')
xlabel('Time (s)')
ylabel('Amplitude')
subplot(2,1,2)
plot(f, fftData)
strMin = [num2str(minFFT_i/2 - 1), ' Hz'];
strMax = [num2str(maxFFT_i/2 - 1), ' Hz'];
text(f(minFFT_i),minFFT,strMin,'HorizontalAlignment','left');
text(f(maxFFT_i),maxFFT,strMax,'HorizontalAlignment','left');
line()
grid on
title('Plot of computed rect spectrum using DSP FFT with N = 16384')
xlabel('Frequency (Hz)')
ylabel('Amplitude')


# IFFT of rect spectrum test
ifftID = fopen('data/ifft_1kHz_rect_spectrum.txt', 'r');
formatSpec = '%f';
ifftData = fscanf(ifftID, formatSpec);
fclose(ifftID);

N = 16384;
t = 0:1/16384:1 - 1/16384;
f = 0:0.5:16384 - 0.5;
[minFFT, minFFT_i] = min(ifftData);
[maxFFT, maxFFT_i] = max(ifftData);

figure(2)
clf();
subplot(2,1,1)
plot(f, fftData)
grid on
title('Plot of rect spectrum using DSP FFT with N = 16384')
xlabel('Frequency (Hz)')
ylabel('Amplitude')
subplot(2,1,2)
plot(t(1:50), ifftData(1:50))
grid on
title('Plot of computed IFFT of rect spectrum using DSP IFFT with N = 16384')
xlabel('Time (s)')
ylabel('Amplitude')

#pause(10)

print ifft_test_rect.pdf