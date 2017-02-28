#######################################################################
# Filter Biquad Test Plot
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

NUM_SAMPLES = 512;
Fs = 48000;
t = 0:1/Fs:NUM_SAMPLES * 1/Fs - 1/Fs;

formatSpec = '%f';
sin1kID = fopen('data/filter/sine_1kHz.txt');
sin10kID = fopen('data/filter/sine_10kHz.txt');
transferXID = fopen('data/filter/transferX.txt');
transferYID = fopen('data/filter/transferY.txt');
out1kID = fopen('data/filter/filtered_sine_1kHz.txt');
out10kID = fopen('data/filter/filtered_sine_10kHz.txt');
sin1kData = fscanf(sin1kID, formatSpec);
sin10kData = fscanf(sin10kID, formatSpec);
transferXData = fscanf(transferXID, formatSpec);
transferYData = fscanf(transferYID, formatSpec);
outData1k = fscanf(out1kID, formatSpec);
outData10k = fscanf(out10kID, formatSpec);
fclose(sin1kID);
fclose(sin10kID);
fclose(transferXID);
fclose(transferYID);
fclose(out1kID);
fclose(out10kID);

hf = figure(1);
clf()
subplot(5,1,1)
plot(transferXData, transferYData)
grid on
title('Transfer Function of Notch Filter (Fc = 10 kHz, Fs = 48 kHz, Q = 1)')
xlabel('Frequency (Hz)')
ylabel('Magnitude (dB)')
subplot(5,1,2)
stem(t, sin1kData)
grid on
title('Generated 1 kHz sine')
xlabel('Time (s)')
ylabel('Amplitude')
subplot(5,1,3)
stem(t(1:256), sin10kData(1:256))
grid on
title('Generated 10 kHz sine')
xlabel('Time (s)')
ylabel('Amplitude')
subplot(5,1,4)
stem(t, outData1k)
grid on
title('Filtered 1 kHz sine')
xlabel('Time (s)')
ylabel('Amplitude')
subplot(5,1,5)
stem(t(1:256), outData10k(1:256))
grid on
title('Filtered 10 kHz sine')
xlabel('Time (s)')
ylabel('Amplitude')

set(hf,'PaperOrientation','portrait');
set(hf,'PaperUnits','normalized');
set(hf,'PaperPosition', [0 0 1 1]);
print (hf, '-dpdf', 'filter_biquad_test.pdf');
