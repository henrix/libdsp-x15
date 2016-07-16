/***********************************************************************
 * Author: Henrik Langer (henni19790@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

#include "WaveIO.hpp"

int WaveIO::readWAV(const std::string &filename, float *wav, int numItems, int numChannels){
	//int framesRead;
	SF_INFO wavMetaData;
	char *cstr = new char[filename.length()+1];
	std::strcpy(cstr, filename.c_str());
	SNDFILE *inFile = sf_open(cstr, SFM_READ, &wavMetaData);
	
	return sf_read_float(inFile, wav, numItems * numChannels);
}

int WaveIO::writeWAV(const std::string &filename, float *wav, int numItems, int numChannels, int samplerate){
	SF_INFO wavMetaData;
	char *cstr = new char[filename.length()+1];
	std::strcpy(cstr, filename.c_str());
	wavMetaData.samplerate = samplerate;
	wavMetaData.channels = numChannels;
	wavMetaData.format = SF_FORMAT_WAV;
	SNDFILE *outFile = sf_open(cstr, SFM_WRITE, &wavMetaData);

	return sf_write_float(outFile, wav, numItems * numChannels);
}
