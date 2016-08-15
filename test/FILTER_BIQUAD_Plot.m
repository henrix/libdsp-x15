#######################################################################
# Filter biquad Test Plot
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

t = 0:1/48000:512 * 1/48000 - 1/48000;
length(t)

sinID = fopen('data/crossover/sine.txt');
outID = fopen('data/crossover/output.txt');
formatSpec = '%f';
sinData = fscanf(sinID, formatSpec);
outData = fscanf(outID, formatSpec);
fclose(sinID);
fclose(outID);

figure(1)
clf()
subplot(2,1,1)
stem(t, sinData)
grid on
title('Plot of generated sine')
xlabel('Time (s)')
ylabel('Amplitude')
subplot(2,1,2)
stem(t, outData)
grid on
title('Plot of filter output')
xlabel('Time (s)')
ylabel('Amplitude')

print filter_biquad_test.pdf
