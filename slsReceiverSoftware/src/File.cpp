/************************************************
 * @file File.h
 * @short sets/gets properties for the file,
 * creates/closes the file and writes data to it
 ***********************************************/

#include "File.h"

#include <iostream>
using namespace std;


File::File(int ind, char* fname, char* fpath, uint64_t* findex,
		bool* frindexenable, bool* owenable, int* dindex, int* nunits, uint64_t* nf, uint32_t* dr):
			index(ind),
			fileNamePrefix(fname),
			filePath(fpath),
			fileIndex(findex),
			frameIndexEnable(frindexenable),
			overWriteEnable(owenable),
			detIndex(dindex),
			numUnitsPerDetector(nunits),
			numImages(nf),
			dynamicRange(dr)
{
	master = index?false:true;
}

File::~File() {}

string File::GetCurrentFileName() {
	return currentFileName;
}

void File::PrintMembers() {
	printf("\nGeneral Writer Variables:"
			"Index: %d\n"
			"File Name Prefix: %s\n"
			"File Path: %s\n"
			"File Index: %lld\n"
			"Frame Index Enable: %d\n"
			"Over Write Enable: %d\n"
			"Detector Index: %d\n"
			"Number of Units Per Detector: %d\n",
			index,
			fileNamePrefix,
			filePath,
			(long long int)*fileIndex,
			*frameIndexEnable,
			*overWriteEnable,
			*detIndex,
			*numUnitsPerDetector);
}


void File::GetMemberPointerValues(char*& fname, char*& fpath, uint64_t*& findex,
		bool*& frindexenable, bool*& owenable, int*& dindex, int*& nunits, uint64_t*& nf, uint32_t* dr)
{
	fname = fileNamePrefix;
	fpath = filePath;
	findex = fileIndex;
	frindexenable = frameIndexEnable;
	owenable = overWriteEnable;
	dindex = detIndex;
	nunits = numUnitsPerDetector;
	nf = numImages;
	dr = dynamicRange;
}
