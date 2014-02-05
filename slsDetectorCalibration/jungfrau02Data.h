#ifndef JUNGFRAU02DATA_H
#define  JUNGFRAU02DATA_H

#include "chiptestBoardData.h"



class jungfrau02Data : public chiptestBoardData {
 public:




  /**
     Implements the chiptestBoardData structure for the jungfrau02 prototype 
     (48x48 pixels, ADC2 for analog output, eventually 2 more for digital bits , offset configurable etc.)
     \param c crosstalk parameter for the output buffer

  */
  

  jungfrau02Data(int nadc, int offset, double c=0): chiptestBoardData(48, 48, nadc, offset), 
    xtalk(c) {




    uint16_t **dMask;
    int **dMap;



    dMask=new uint16_t*[48];
    dMap=new int*[48];
    for (int i = 0; i < 48; i++) {
      dMap[i] = new int[48];
      dMask[i] = new uint16_t[48];
    }


	for (int iy=0; iy<48; iy++) {
	  for (int ix=0; ix<48; ix++) {

	    dMap[iy][ix]=offset+(iy*48+ix)*nAdc;
	    // cout << ix << " " << iy << " " << dMap[ix][iy] << endl;
	  }
	}

    for (int ix=0; ix<48; ix++) {
      for (int iy=0; iy<48; iy++)
	dMask[iy][ix]=0x0;
    }

    setDataMap(dMap);
    setDataMask(dMask);


    

  };
    
   /**

     Returns the value of the selected channel for the given dataset. Since the ADC is only 14bit wide, if the gain bits are read out they are returned in bit 14-15
     \param data pointer to the dataset (including headers etc)
     \param ix pixel number in the x direction
     \param iy pixel number in the y direction
     \returns data for the selected channel, with inversion if required

  */
 
 virtual uint16_t getChannel(char *data, int ix, int iy=0) {
    uint16_t m=0, d=0;
    if (ix>=0 && ix<nx && iy>=0 && iy<ny && dataMap[iy][ix]>=0 && dataMap[iy][ix]<dataSize) {
      m=dataMask[iy][ix];
      d=*((uint16_t*)(data+dataMap[iy][ix]));
	if (nAdc==3) {
           if (*(uint16_t*)(data+dataMap[iy][ix]+1)>8000)
		d|=(1<<14); // set gain bit 0
           if (*(uint16_t*)(data+dataMap[iy][ix]+2)>8000)
		d|=(1<<15); // set gain bit 1

        }

    }  
    return d^m;
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
    if (xtalk==0 || ix==0)
      return  (double)(getChannel(data, ix, iy)&0x3ff);
    else
      return  (double)(getChannel(data, ix, iy)&0x3ff)-xtalk* (double)(getChannel(data, ix, iy)&0x3ff);
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


};



#endif
