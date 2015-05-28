#ifndef EIGERMODULEDATA_H
#define  EIGERMODULEDATA_H
#include "slsReceiverData.h"



class eigerHalfModuleData : public slsReceiverData<uint32_t> {
public:




	/**
     Implements the slsReceiverData structure for the eiger prototype read out by a half module i.e. using the slsReceiver
     (256*256 pixels, 512 packets for 16 bit mode, 256 for 8, 128 for 4, 1024 for 32, 1040  etc.)
     \param d dynamic range
     \param c crosstalk parameter for the output buffer

	 */


	eigerHalfModuleData(int dr, int np, int bsize, int dsize, bool top, double c=0): slsReceiverData<uint32_t>(xpixels, ypixels, np, bsize),
	xtalk(c), bufferSize(bsize), dataSize(dsize), dynamicRange(dr), numberOfPackets(np), top(top){


		int **dMap;
		uint32_t **dMask;

		dMap=new int*[ypixels];
		dMask=new uint32_t*[ypixels];


		for (int i = 0; i < ypixels; i++) {
			dMap[i] = new int[xpixels];
			dMask[i] = new uint32_t[xpixels];
		}

		//Map
		int totalNumberOfBytes = 1040 * dynamicRange * 16 *2; //for both 1g and 10g
		int iPacket1 = 8;
		int iPacket2 = (totalNumberOfBytes/2) + 8;
		int iData1 = 0, iData2 = 0;
		int increment = (dynamicRange/8);
		int ic_increment = 1;
		if (dynamicRange == 4) {
			increment = 1;
			ic_increment = 2;
		}
		int iPort;


		if(top){
			for (int ir=0; ir<ypixels; ir++) {
				for (int ic=0; ic<xpixels; ic = ic + ic_increment) {
					iPort = ic / (xpixels/2);
					if(!iPort){
						dMap[ir][ic]  = iPacket1;
						iPacket1 += increment;
						iData1 += increment;
						//increment header
						if(iData1 >= dataSize){
							iPacket1 += 16;
							iData1 = 0;
						}
					}else{
						dMap[ir][ic]  = iPacket2;
						iPacket2 += increment;
						iData2 += increment;
						//increment header
						if(iData2 >= dataSize){
							iPacket2 += 16;
							iData2 = 0;
						}
					}
				}
			}
		}


		//bottom
		else{
			iData1 = 0; iData2 = 0;
			int numbytesperlineperport = 1024;
			if (dynamicRange == 8)
				numbytesperlineperport = 512;
			else if (dynamicRange == 4)
				numbytesperlineperport = 256;
			else if (dynamicRange == 32)
				numbytesperlineperport = 2048;



			iPacket1 = (totalNumberOfBytes/2) - numbytesperlineperport - 8;
			iPacket2 = totalNumberOfBytes - numbytesperlineperport - 8;
			if (dynamicRange == 32){
				iPacket1 -= 16;
				iPacket2 -= 16;
			}

			for (int ir=0; ir<ypixels; ir++) {
				for (int ic=0; ic<xpixels; ic = ic + ic_increment) {
					iPort = ic / (xpixels/2);
					if(!iPort){
						dMap[ir][ic]  = iPacket1;
						iPacket1 += increment;
						iData1 += increment;
						if(dynamicRange == 32){
							if(iData1 == numbytesperlineperport){
								iPacket1 -= (numbytesperlineperport*2 + 16*3);
								iData1 = 0;
							}
							if(iData1 == dataSize){
								iPacket1 += 16;
							}
						}else if((iData1 % numbytesperlineperport) == 0){
							iPacket1 -= (numbytesperlineperport*2);
							if(iData1 == dataSize){
								iPacket1 -= 16;
								iData1 = 0;
							}

						}
					}
					//other port
					else{
						dMap[ir][ic]  = iPacket2;
						iPacket2 += increment;
						iData2 += increment;
						if(dynamicRange == 32){
							if(iData2 == numbytesperlineperport){
								iPacket2 -= (numbytesperlineperport*2 + 16*3);
								iData2 = 0;
							}
							if(iData2 == dataSize){
								iPacket2 += 16;
							}
						}else if((iData2 % numbytesperlineperport) == 0){
							iPacket2 -= (numbytesperlineperport*2);
							if(iData2 == dataSize){
								iPacket2 -= 16;
								iData2 = 0;
							}
						}
					}
				}
			}
		}




		//Mask
		for(int ir=0; ir<ypixels; ++ir)
			for(int ic=0; ic<xpixels; ++ic)
				dMask[ir][ic] = 0x0;

		setDataMap(dMap);
		setDataMask(dMask);




	};






	/** Returns the frame number for the given dataset.
	     \param buff pointer to the dataset
	     \returns frame number
	 */
	int getFrameNumber(char *buff){
		return(*(unsigned int*)(((eiger_packet_header *)((char*)buff))->num1));
	};


	





	/** gets the packets number (last packet is labelled with 0 and is replaced with 40)
     	 \param buff pointer to the memory
     	 \returns packet number
	 */
	int getPacketNumber(char *buff){
#ifdef VERY_DEBUG
		cprintf(RED, "\n0x%x - %d - %d",
			(*(uint8_t*)(((eiger_packet_header *)((char*)(buff)))->num3)),
			(*(uint8_t*)(((eiger_packet_header *)((char*)(buff)))->num4)),
			(*(uint16_t*)(((eiger_packet_header *)((char*)(buff)))->num2)));
#endif
		//32 bit packet number written in num2
		if(dynamicRange == 32){
			//both ports have same packet numbers, so reconstruct, ports interchanged for bottom
			if((*(uint8_t*)(((eiger_packet_header *)((char*)buff))->num3))){
				if (top)
					return ((*(uint16_t*)(((eiger_packet_header *)((char*)buff))->num2))+(numberOfPackets/2) +1);
				else
					return ((*(uint16_t*)(((eiger_packet_header *)((char*)buff))->num2))+1);
			}else{
				if (top)
					return ((*(uint16_t*)(((eiger_packet_header *)((char*)buff))->num2))+1);
				else
					return ((*(uint16_t*)(((eiger_packet_header *)((char*)buff))->num2))+(numberOfPackets/2) +1);
			}
		}
		else{
			//both ports have same packet numbers, so reconstruct
			if((*(uint8_t*)(((eiger_packet_header *)((char*)buff))->num3))){
				if (top)
					return ((*(uint8_t*)(((eiger_packet_header *)((char*)buff))->num4))+(numberOfPackets/2) +1);
				else
					return ((*(uint8_t*)(((eiger_packet_header *)((char*)buff))->num4))+1);
			}else{
				if (top)
					return ((*(uint8_t*)(((eiger_packet_header *)((char*)buff))->num4))+1);
				else
					return ((*(uint8_t*)(((eiger_packet_header *)((char*)buff))->num4))+(numberOfPackets/2) +1);
			}
		}
	};


	/**
    returns the pixel value as double correcting for the output buffer crosstalk
     \param data pointer to the memory
     \param ix coordinate in the x direction
     \param iy coordinate in the y direction
     \returns channel value as double

	 */
	double getValue(char *data, int ix, int iy=0) {
		//  cout << "##" << (void*)data << " " << ix << " " <<iy << endl;
		if (xtalk==0)
			  return slsDetectorData<uint32_t>::getValue(data, ix, iy);
		else
			return slsDetectorData<uint32_t>::getValue(data, ix, iy)-xtalk*slsDetectorData<uint32_t>::getValue(data, ix-1, iy);
	};



	/** sets the output buffer crosstalk correction parameter
      \param c output buffer crosstalk correction parameter to be set
      \returns current value for the output buffer crosstalk correction parameter

	 */
	double setXTalk(double c) {xtalk=c; return xtalk;}


	/** gets the output buffer crosstalk parameter
      \returns current value for the output buffer crosstalk correction parameter
	 */
	double getXTalk() {return xtalk;}







private:

	double xtalk; /**<output buffer crosstalk correction parameter */

	const static int xpixels = 1024;
	const static int ypixels = 256;
	const int bufferSize;
	const int dataSize;
	const int dynamicRange;
	const int numberOfPackets;
	bool top;


	/** structure of an eiger image header*/
	typedef struct
	{
		unsigned char num1[4];
		unsigned char num2[2];
		unsigned char num3[1];
		unsigned char num4[1];
	} eiger_packet_header;

};



#endif
