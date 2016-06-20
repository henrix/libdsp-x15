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
sinID = fopen('data/sine.txt', 'r');
fftID = fopen('data/fft_sine.txt', 'r');
ifftID = fopen('data/ifft_sine_spectrum.txt', 'r');
formatSpec = '%f';
sinData = fscanf(sinID, formatSpec);
fftData = fscanf(fftID, formatSpec);
ifftData = fscanf(ifftID, formatSpec);
fclose(sinID);
fclose(fftID);
fclose(ifftID);

N = (16*1024);
t = 0:1/16384:1 - 1/16384;
f = -(N/2):(N/2)-1;
[minFFT, minFFT_i] = min(fftData);
[maxFFT, maxFFT_i] = max(fftData);
[minIFFT, minIFFT_i] = min(ifftData);
[maxIFFT, maxIFFT_i] = max(ifftData);

figure(1)
clf();
subplot(3,1,1)
stem(t(1:50), sinData(1:50))
grid on
title('Plot of generated sine')
xlabel('Time (s)')
ylabel('Amplitude')
subplot(3,1,2)
length(f)
length(fftData)
plot(f, fftData)
% strMin = [num2str(f(minFFT_i)), ' Hz'];
% strMax = [num2str(f(maxFFT_i)), ' Hz'];
% text(f(minFFT_i),minFFT,strMin,'HorizontalAlignment','left');
% text(f(maxFFT_i),maxFFT,strMax,'HorizontalAlignment','left');
line()
grid on
title('Plot of computed sine spectrum (frequency not scaled)')
xlabel('Frequency (Hz)')
ylabel('Amplitude')
subplot(3,1,3)
stem(t(1:50), ifftData(1:50))
strMin = [num2str(f(minIFFT_i)), ' Hz'];
strMax = [num2str(f(maxIFFT_i)), ' Hz'];
text(f(minIFFT_i),minIFFT,strMin,'HorizontalAlignment','left');
text(f(maxIFFT_i),maxIFFT,strMax,'HorizontalAlignment','left');
line()
grid on
title('Plot of reconstructed sine')
xlabel('Time (s)')
ylabel('Amplitude')

% pause(10)

print fft_test_sine.pdf
