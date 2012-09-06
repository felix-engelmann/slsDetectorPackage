#ifndef BAD_CHANNEL_CORRECTIONS_H
#define BAD_CHANNEL_CORRECTIONS_H


#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
class badChannelCorrections{

 public:

  

  static int readBadChannelCorrectionFile(ifstream &infile, int &nbad, int *badlist, int moff=0){ \
    int interrupt=0;							\
    int ich;							\
    int chmin,chmax;						\
    string str;							\
    nbad=0;							\
    while (infile.good() and interrupt==0) {			\
      getline(infile,str);					\
      istringstream ssstr;					\
      ssstr.str(str);						\
      if (ssstr.bad() || ssstr.fail() || infile.eof()) {	\
	interrupt=1;						\
	break;							\
      }								\
      if (str.find('-')!=string::npos) {			\
	ssstr >> chmin ;					\
	ssstr.str(str.substr(str.find('-')+1,str.size()));	\
	ssstr >> chmax;						\
	for (ich=chmin; ich<=chmax; ich++) {			\
	  badlist[nbad]=ich;					\
	  nbad++;							\
	}								\
      } else {								\
	ssstr >> ich;							\
	badlist[nbad]=ich;						\
	nbad++;								\
      }									\
    }									\
    return nbad;  };
  
 protected:
  

  char *badChanFile;
  int *nBadChans;
  int *badChansList;
  int *nBadFF;
  int *badFFList;




};
#endif
