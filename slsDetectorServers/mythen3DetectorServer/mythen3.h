#ifndef MYTHEN3_H
#define MYTHEN3_H

#include "Pattern.h"

/** Signal Definitions */
#define SIGNAL_TBLoad_1    (0)
#define SIGNAL_TBLoad_2    (1)
#define SIGNAL_TBLoad_3    (2)
#define SIGNAL_TBLoad_4    (3)
#define SIGNAL_TBLoad_5    (4)
#define SIGNAL_TBLoad_6    (5)
#define SIGNAL_TBLoad_7    (6)
#define SIGNAL_TBLoad_8    (7)
#define SIGNAL_TBLoad_9    (8)
#define SIGNAL_TBLoad_10   (9)
#define SIGNAL_AnaMode     (10)
#define SIGNAL_CHSserialIN (11)
#define SIGNAL_READOUT     (12)
#define SIGNAL_pulse       (13)
#define SIGNAL_EN1         (14)
#define SIGNAL_EN2         (15)
#define SIGNAL_EN3         (16)
#define SIGNAL_clk         (17)
#define SIGNAL_SRmode      (18)
#define SIGNAL_serialIN    (19)
#define SIGNAL_STO         (20)
#define SIGNAL_STATLOAD    (21)
#define SIGNAL_resStorage  (22)
#define SIGNAL_resCounter  (23)
#define SIGNAL_CHSclk      (24)
#define SIGNAL_exposing    (25)


//CHIP STARTUS REGISTER BITS
#define CSR_spypads  0
#define CSR_invpol  4
#define CSR_dpulse  5
#define CSR_interp  6
#define CSR_C10pre  7 //#default
#define CSR_pumprobe  8
#define CSR_apulse  9
#define CSR_C15sh  10 
#define CSR_C30sh  11 //#default
#define CSR_C50sh  12
#define CSR_C225ACsh  13 // Connects 225fF SHAPER AC cap (1: 225 to shaper, 225 to GND. 0: 450 to shaper) 
#define CSR_C15pre  14 

#define CSR_default  (1<<CSR_C10pre )|(1<< CSR_C30sh)

enum {Cp_0, Cp_10, Cp_15, Cp_45};
enum {Csh_0,Csh_15,Csh_30,Csh_45,Csh_50,Csh_65,Csh_80,Csh_95};
enum {Cac225, Cac_450};



#define default_gain 
int setBit(int ibit, int patword);
int clearBit(int ibit, int patword);
patternParameters *setChipStatusRegister(int csr);
patternParameters *setChannelRegisterChip(int ichip, int *mask, int *trimbits);
patternParameters *setInterpolation(int mask);
patternParameters *setPumpProbe(int mask);
patternParameters *setDigitalPulsing(int mask);
patternParameters *setAnalogPulsing(int mask);
patternParameters *setNegativePolarity(int mask);
patternParameters *setChipGain(int pgain, int shgain, int acgain);
int setGainAC(int shgain, int *csr);
int setGainShaper(int shgain, int *csr);
int setGainPreamp(int pgain, int *csr);

#endif