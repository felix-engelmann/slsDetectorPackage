#ifndef SINGLEPHOTONDETECTOR_H
#define SINGLEPHOTONDETECTOR_H

#include "analogDetector.h" 

#include "single_photon_hit.h"



//#define MYROOT1

#ifdef MYROOT1
#include <TTree.h>
#endif


#ifndef EVTYPE_DEF
#define EVTYPE_DEF
enum eventType {
  PEDESTAL=0,
  NEIGHBOUR=1,
  PHOTON=2,
  PHOTON_MAX=3,
  NEGATIVE_PEDESTAL=4,
  UNDEFINED_EVENT=-1
};
#endif

//template <class dataType> class singlePhotonDetector : 
//public analogDetector<dataType> {
class singlePhotonDetector : 
public analogDetector<uint16_t> {

  /** @short class to perform pedestal subtraction etc. and find single photon clusters for an analog detector */

 public:


  /**

     Constructor (no error checking if datasize and offsets are compatible!)
     \param d detector data structure to be used
     \param csize cluster size (should be an odd number). Defaults to 3
     \param nsigma number of rms to discriminate from the noise. Defaults to 5
     \param sign 1 if photons are positive, -1 if  negative
     \param cm common mode subtraction algorithm, if any. Defaults to NULL i.e. none
     \param nped number of samples for pedestal averaging
     \param nd number of dark frames to average as pedestals without photon discrimination at the beginning of the measurement


  */
  

 singlePhotonDetector(slsDetectorData<uint16_t> *d,
		      int csize=3,
		      double nsigma=5,
		      int sign=1,
		      commonModeSubtraction *cm=NULL,
		      int nped=1000,
		      int nd=100, int nnx=-1, int nny=-1, double *gm=NULL) : analogDetector<uint16_t>(d, sign, cm, nped, nnx, nny, gm),   nDark(nd), eventMask(NULL),nSigma (nsigma), clusterSize(csize), clusterSizeY(csize), cluster(NULL),   quad(UNDEFINED_QUADRANT), tot(0), quadTot(0) {
    
    
    
    
    eventMask=new eventType*[ny];
    for (int i=0; i<ny; i++) {
      eventMask[i]=new eventType[nx];
    }
    
    if (ny==1)
      clusterSizeY=1;

    cluster=new single_photon_hit(clusterSize,clusterSizeY);
    setClusterSize(csize);
    
  };
    /**
       destructor. Deletes the cluster structure and the pdestalSubtraction array
    */
  virtual ~singlePhotonDetector() {delete cluster; for (int i=0; i<ny; i++) delete [] eventMask[i]; delete [] eventMask; };

    
  
    /* /\** resets the eventMask to undefined and the commonModeSubtraction *\/ */
    /* void newFrame(){analogDetector::newFrame(); for (int iy=0; iy<ny; iy++) for (int ix=0; ix<nx; ix++) eventMask[iy][ix]=UNDEFINED_EVENT; }; */

    
 
 singlePhotonDetector(singlePhotonDetector *orig) : analogDetector<uint16_t>(orig) {

    nDark=orig->nDark;
    
    
    eventMask=new eventType*[ny];
    for (int i=0; i<ny; i++) {
      eventMask[i]=new eventType[nx];
    }
    
    
    nSigma=orig->nSigma;
    clusterSize=orig->clusterSize;
    clusterSizeY=orig->clusterSizeY;
    cluster=new single_photon_hit(clusterSize,clusterSizeY);

    quad=UNDEFINED_QUADRANT;
    tot=0;
    quadTot=0;
    gmap=orig->gmap;

  }
  

  virtual singlePhotonDetector *Clone() {
    return new singlePhotonDetector(this);
  }
    /** sets/gets number of rms threshold to detect photons
	\param n number of sigma to be set (0 or negative gets)
	\returns actual number of sigma parameter
    */
    double setNSigma(double n=-1){if (n>0) nSigma=n; return nSigma;}
  
    /** sets/gets cluster size
	\param n cluster size to be set, (0 or negative gets). If even is incremented by 1.
	\returns actual cluster size
    */
    int setClusterSize(int n=-1){
      if (n>0 && n!=clusterSize) {
	if (n%2==0)
	  n+=1;
	clusterSize=n;
	if (cluster)
	  delete cluster;
	if (ny>1)
	  clusterSizeY=clusterSize;
	cluster=new single_photon_hit(clusterSize,clusterSizeY);
      }
      return clusterSize;
    }



    virtual int *getNPhotons(char *data, double thr=-1, int *nph=NULL) {
      
      double val;
      if (nph==NULL)
	nph=image;
      //nph=new int[nx*ny];
      
      double rest[ny][nx];
      int cy=(clusterSizeY+1)/2;
      int cs=(clusterSize+1)/2;
      
      int ccs=clusterSize;
      int ccy=clusterSizeY;
      
      double g=1.;

      
      double tthr=thr;
      int nn=0;
      double max=0, tl=0, tr=0, bl=0,br=0, v;
      
      
      if (thr>=0) {
	cy=1;
	cs=1;
	ccs=1;
	ccy=1;
      }
      
      if (iframe<nDark) { 
	//cout << "ped " << iframe << endl;
	addToPedestal(data);
	return nph;
      }	else {
	if (thr>0) {
	cout << "threshold" << endl;
	for (int ix=0; ix<nx; ix++) {
	  for (int iy=0; iy<ny; iy++) {
	    //	cout << id << " " << ix << " " << iy << endl;
	    
	    if (gmap) {
	      g=gmap[iy*nx+ix];
	      if (g==0) g=-1.;
	    }
	    

	    //return UNDEFINED_EVENT;
    
	    val=subtractPedestal(data,ix,iy);
	    
	    
	    /* if (thr<=0) { */
	      
	    /*   tthr=nSigma*getPedestalRMS(ix,iy)/g; */
	      
	    /* } */
	    
	    /*  */
		  nn=analogDetector<uint16_t>::getNPhotons(data,ix,iy,tthr);
		  nph[ix+nx*iy]+=nn;
		  rest[iy][ix]=(val-nn*tthr);
	    
	  }
	}
	}
	for (int ix=0; ix<nx; ix++) {
	  for (int iy=0; iy<ny; iy++) {
	    
	    //  for (int ix=clusterSize/2; ix<clusterSize/2-1; ix++) {
	    // for (int iy=clusterSizeY/2; iy<ny-clusterSizeY/2; iy++) {
	    eventMask[iy][ix]=PEDESTAL;
	    
	    if (thr<=0) {
	      tthr=nSigma*getPedestalRMS(ix,iy)/g;
	      if (ix==0 || iy==0)
		rest[iy][ix]=subtractPedestal(data,ix,iy);
	     
	    }
	    
	    max=0;
	    tl=0;
	    tr=0;
	    bl=0;
	    br=0;
	    tot=0;
	    quadTot=0;

	    for (int ir=-(clusterSizeY/2); ir<(clusterSizeY/2)+1; ir++) {
	      for (int ic=-(clusterSize/2); ic<(clusterSize/2)+1; ic++) {
		if ((iy+ir)>=0 && (iy+ir)<ny && (ix+ic)>=0 && (ix+ic)<nx) {
		  //cluster->set_data(rest[iy+ir][ix+ic], ic, ir);
		  
		  if (thr<=0 && ir>=0 && ic>=0 ) 
		    rest[iy+ir][ix+ic]=subtractPedestal(data,ix+ic,iy+ir);
		  
		  v=rest[iy+ir][ix+ic];//cluster->get_data(ic,ir);
		  tot+=v;
		  
		  if (ir<=0 && ic<=0)
		    bl+=v;
		  if (ir<=0 && ic>=0)
		    br+=v;
		  if (ir>=0 && ic<=0)
		    tl+=v;
		  if (ir>=0 && ic>=0)
		    tr+=v;
		  
		  if (v>max) {
		    max=v;
		  }
		  // if (ir==0 && ic==0) {
		    if (v>tthr) {
		      eventMask[iy][ix]=NEIGHBOUR;
		    }
		    //}
		}
	      }
	    }
	    if (rest[iy][ix]<=-tthr) {
	      eventMask[iy][ix]=NEGATIVE_PEDESTAL;
	      //if (cluster->get_data(0,0)>=max) {
	    } else if (rest[iy][ix]>=max) { 
	      if (bl>=br && bl>=tl && bl>=tr) {
		quad=BOTTOM_LEFT;
		quadTot=bl;
	      } else if (br>=bl && br>=tl && br>=tr) {
		quad=BOTTOM_RIGHT;
		quadTot=br;
	      } else if (tl>=br && tl>=bl && tl>=tr) {
		quad=TOP_LEFT;
		quadTot=tl;
	      } else if   (tr>=bl && tr>=tl && tr>=br) {
		quad=TOP_RIGHT;
		quadTot=tr;
	      }
	      if (max>tthr || tot>sqrt(ccy*ccs)*tthr || quadTot>sqrt(cy*cs)*tthr) {
	      /* 	cout << ix << " " << iy << " " << rest[iy][ix] <<" " <<  tot << " " << quadTot << endl; */
	      /* 	 for (int ir=-(clusterSizeY/2); ir<(clusterSizeY/2)+1; ir++) { */
	      /* for (int ic=-(clusterSize/2); ic<(clusterSize/2)+1; ic++) */
	      /* 	cout << rest[iy+ir][ix+ic] << " " ; */
	      /* cout << endl; */
	      /* 	 } */
		eventMask[iy][ix]=PHOTON;
		nph[ix+nx*iy]++;
		// rest[iy][ix]-=tthr;
	      } //else if (thr<=0 ) {
		//addToPedestal(data,ix,iy);
	      // }
	    }
	    if (thr<0 && eventMask[iy][ix]==PEDESTAL) {
	      addToPedestal(data,ix,iy);
	    }
	  }
	}
      }
      
      //	}
      // }
      
      
      
      return nph;

    }



    /** finds event type for pixel and fills cluster structure. The algorithm loops only if the evenMask for this pixel is still undefined.
	if pixel or cluster around it are above threshold (nsigma*pedestalRMS) cluster is filled and pixel mask is PHOTON_MAX (if maximum in cluster) or NEIGHBOUR; If PHOTON_MAX, the elements of the cluster are also set as NEIGHBOURs in order to speed up the looping
	if below threshold the pixel is either marked as PEDESTAL (and added to the pedestal calculator) or NEGATIVE_PEDESTAL is case it's lower than -threshold, otherwise the pedestal average would drift to negative values while it should be 0.

	/param data pointer to the data
	/param ix pixel x coordinate
	/param iy pixel y coordinate
	/param cm enable(1)/disable(0) common mode subtraction (if defined).
	/returns event type for the given pixel
    */
    eventType getEventType(char *data, int ix, int iy, int cm=0) {

      // eventType ret=PEDESTAL;
      double max=0, tl=0, tr=0, bl=0,br=0, v;
      //  cout << iframe << endl;
    
      int cy=(clusterSizeY+1)/2;
      int cs=(clusterSize+1)/2;
      double val;
      tot=0;
      quadTot=0;
      quad=UNDEFINED_QUADRANT;

      if (iframe<nDark) {
	addToPedestal(data, ix,iy);
	return UNDEFINED_EVENT;
      }


     
      

      //   if (eventMask[iy][ix]==UNDEFINED) {
	
      eventMask[iy][ix]=PEDESTAL;
	
	
      cluster->x=ix;
      cluster->y=iy;
      cluster->rms=getPedestalRMS(ix,iy);
      cluster->ped=getPedestal(ix,iy, cm);
	

      for (int ir=-(clusterSizeY/2); ir<(clusterSizeY/2)+1; ir++) {
	for (int ic=-(clusterSize/2); ic<(clusterSize/2)+1; ic++) {
	    if ((iy+ir)>=0 && (iy+ir)<ny && (ix+ic)>=0 && (ix+ic)<nx) {
	      
	      v=subtractPedestal(data, ix+ic, iy+ir);
	      
	      cluster->set_data(v, ic, ir);
	      //  v=cluster->get_data(ic,ir);
	      tot+=v;
	      if (ir<=0 && ic<=0)
		bl+=v;
	      if (ir<=0 && ic>=0)
		br+=v;
	      if (ir>=0 && ic<=0)
		tl+=v;
	      if (ir>=0 && ic>=0)
		tr+=v;
	      
	      if (v>max) {
		max=v;
	      }
	      if (ir==0 && ic==0) {
		if (v<-nSigma*cluster->rms)
		  eventMask[iy][ix]=NEGATIVE_PEDESTAL;
	      }
	    }
	  }
	}
	
	if (bl>=br && bl>=tl && bl>=tr) {
	  quad=BOTTOM_LEFT;
	  quadTot=bl;
	} else if (br>=bl && br>=tl && br>=tr) {
	  quad=BOTTOM_RIGHT;
	  quadTot=br;
	} else if (tl>=br && tl>=bl && tl>=tr) {
	  quad=TOP_LEFT;
	  quadTot=tl;
     	} else if   (tr>=bl && tr>=tl && tr>=br) {
	  quad=TOP_RIGHT;
	  quadTot=tr;
	}
	
	if (max>nSigma*cluster->rms || tot>sqrt(clusterSizeY*clusterSize)*nSigma*cluster->rms || quadTot>cy*cs*nSigma*cluster->rms) {
	  if (cluster->get_data(0,0)>=max) {
	    eventMask[iy][ix]=PHOTON_MAX;
	  } else {
	    eventMask[iy][ix]=PHOTON;
	  }
	} else if (eventMask[iy][ix]==PEDESTAL) {
	  if (cm==0) {
	    if (det)
	      val=dataSign*det->getValue(data, ix, iy);
	    else
	      val=((double**)data)[iy][ix];
	    addToPedestal(val,ix,iy);
	  }
	}
      


      return  eventMask[iy][ix];

  };


int getClusters(char *data, single_photon_hit *clusters) {

 
  int nph=0;
  double val[ny][nx];
  int cy=(clusterSizeY+1)/2;
  int cs=(clusterSize+1)/2;
  int ir, ic;
  
  double max=0, tl=0, tr=0, bl=0,br=0, *v, vv;

  if (iframe<nDark) {
    addToPedestal(data);
    return 0;
  }
  newFrame();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      
      max=0;
      tl=0;
      tr=0;
      bl=0;
      br=0;
      tot=0;
      quadTot=0;
      quad=UNDEFINED_QUADRANT;

     

      eventMask[iy][ix]=PEDESTAL;
      
	
      (clusters+nph)->rms=getPedestalRMS(ix,iy);
      
      
      
      for (int ir=-(clusterSizeY/2); ir<(clusterSizeY/2)+1; ir++) {
	for (int ic=-(clusterSize/2); ic<(clusterSize/2)+1; ic++) {
	  
	  if ((iy+ir)>=iy && (iy+ir)<ny && (ix+ic)>=ix && (ix+ic)<nx) {
	    val[iy+ir][ix+ic]=subtractPedestal(data,ix+ic,iy+ir);
	  }
	  
	  v=&(val[iy+ir][ix+ic]);
	  tot+=*v;
	  if (ir<=0 && ic<=0)
	    bl+=*v;
	  if (ir<=0 && ic>=0)
	    br+=*v;
	  if (ir>=0 && ic<=0)
	    tl+=*v;
	  if (ir>=0 && ic>=0)
	    tr+=*v;
	  if (*v>max) {
	    max=*v;
	  }
	  
	  
	  if (ir==0 && ic==0) {
	    if (*v<-nSigma*cluster->rms)
	      eventMask[iy][ix]=NEGATIVE_PEDESTAL;
	  }
	  
	}
      }
      
      if (bl>=br && bl>=tl && bl>=tr) {
	(clusters+nph)->quad=BOTTOM_LEFT;
	(clusters+nph)->quadTot=bl;
      } else if (br>=bl && br>=tl && br>=tr) {
	(clusters+nph)->quad=BOTTOM_RIGHT;
	(clusters+nph)->quadTot=br;
      } else if (tl>=br && tl>=bl && tl>=tr) {
	(clusters+nph)->quad=TOP_LEFT;
	(clusters+nph)->quadTot=tl;
      } else if   (tr>=bl && tr>=tl && tr>=br) {
	(clusters+nph)->quad=TOP_RIGHT;
	(clusters+nph)->quadTot=tr;
      }
      
      if (max>nSigma*cluster->rms || tot>sqrt(clusterSizeY*clusterSize)*nSigma*cluster->rms || ((clusters+nph)->quadTot)>sqrt(cy*cs)*nSigma*cluster->rms) {
	if (val[iy][ix]>=max) {
	  eventMask[iy][ix]=PHOTON_MAX;
	  (clusters+nph)->tot=tot;
	  (clusters+nph)->x=ix;
	  (clusters+nph)->y=iy;
	  (clusters+nph)->ped=getPedestal(ix,iy,0);
	  for (int ir=-(clusterSizeY/2); ir<(clusterSizeY/2)+1; ir++) {
	    for (int ic=-(clusterSize/2); ic<(clusterSize/2)+1; ic++) {
	      (clusters+nph)->set_data(val[iy+ir][ix+ic],ic,ir);
	    }
	  }
	  nph++;


	  } else {
	    eventMask[iy][ix]=PHOTON;
	  }
      } else if (eventMask[iy][ix]==PEDESTAL) {
	addToPedestal(data,ix,iy);
      }


    }
  }

  return  nph;
  
};


    /**<
       returns the total signal in a cluster
       \param size cluser size  should be 1,2 or 3
       \returns cluster center if size=1, sum of the maximum quadrant if size=2, total of the cluster if size=3 or anything else
    */

    double getClusterTotal(int size) {
      switch (size) {
      case 1:
	return getClusterElement(0,0);
      case 2:
	return quadTot;
      default:
	return tot;
      };
    };

      /**<
       retrurns the quadrant with maximum signal
       \returns quadrant where the cluster is located    */

    quadrant getQuadrant() {return quad;};

  
    /** returns value for cluster element in relative coordinates
	\param ic x coordinate (center is (0,0))
	\param ir y coordinate (center is (0,0))
	\returns cluster element
    */
    double getClusterElement(int ic, int ir=0){return cluster->get_data(ic,ir);};

    /** returns event mask for the given pixel
	\param ic x coordinate (center is (0,0))
	\param ir y coordinate (center is (0,0))
	\returns event mask enum for the given pixel
    */
    eventType getEventMask(int ic, int ir=0){return eventMask[ir][ic];};
 

#ifdef MYROOT1
    /** generates a tree and maps the branches
	\param tname name for the tree
	\param iFrame pointer to the frame number
	\returns returns pointer to the TTree
    */
    TTree *initEventTree(char *tname, int *iFrame=NULL) {
      TTree* tall=new TTree(tname,tname);

      if (iFrame)
	tall->Branch("iFrame",iFrame,"iframe/I");
      else
	tall->Branch("iFrame",&(cluster->iframe),"iframe/I");

      tall->Branch("x",&(cluster->x),"x/I");
      tall->Branch("y",&(cluster->y),"y/I");
      char tit[100];
      sprintf(tit,"data[%d]/D",clusterSize*clusterSizeY);
      tall->Branch("data",cluster->data,tit);
      tall->Branch("pedestal",&(cluster->ped),"pedestal/D");
      tall->Branch("rms",&(cluster->rms),"rms/D");
      tall->Branch("tot",&(cluster->tot),"tot/D");
      tall->Branch("quadTot",&(cluster->quadTot),"quadTot/D");
      tall->Branch("quad",&(cluster->quad),"quad/I");
      return tall;
    };
#else
    /** write cluster to filer*/
    void writeCluster(FILE* myFile){cluster->write(myFile);};

#endif

    virtual void processData(char *data, frameMode i=eFrame, int *val=NULL) {
      // cout << "sp" << endl;
      switch(i) {
      case ePedestal:
	addToPedestal(data);
	break;
      default:
	getNPhotons(data,-1,val);
      }
      iframe++;
      //	cout << "done" << endl;
    };


 protected:
  

    int nDark; /**< number of frames to be used at the beginning of the dataset to calculate pedestal without applying photon discrimination */
    eventType **eventMask; /**< matrix of event type or each pixel */
    double nSigma; /**< number of sigma parameter for photon discrimination */
    int clusterSize; /**< cluster size in the x direction */
    int clusterSizeY; /**< cluster size in the y direction i.e. 1 for strips, clusterSize for pixels */
    single_photon_hit *cluster; /**< single photon hit data structure */
    quadrant quad; /**< quadrant where the photon is located */
    double tot; /**< sum of the 3x3 cluster */
    double quadTot; /**< sum of the maximum 2x2cluster */


};





#endif
