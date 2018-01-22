#include <cstdlib>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <vector>
#include <string.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_statistics.h>

double eps = 1.0e-9;

using namespace std;

//Usage: hist datafile histfile nbins log/uni min max norm weight
int main(int argc, char *argv[])
{
  string fdataName, fhistName;
  int nbins;
  double maxval = -1.0e-99;
  double minval = 1.0e+99;
  bool minmaxGiven = false;
  bool normGiven = false;
  double norm = 1.;
  bool weighted =  false;
  const char*  histtype;
  
  fdataName = "btc.dat";
  fhistName = "btc-hist.dat";
  nbins = 30;
  histtype = "log";
  
  if(argc>=3){
    fdataName = argv[1];
    fhistName = argv[2];
    
    nbins = atoi(argv[1]);
  }
  if(argc>=4){
    nbins  = atoi(argv[3]);
  }

  if(argc>=5){
    histtype = argv[4];
  }
  
  if(argc>=7){
    minval = atof(argv[5]);
    maxval = atof(argv[6]);
    minmaxGiven = true;
  }
  if(argc>=8){
    norm = atof(argv[7]);
    normGiven = true;
  }
  if(argc==9){
    weighted = atoi(argv[8]);
  }
  cout << "input file: " << fdataName << endl;
  cout << "output file: " << fhistName << endl;
  cout << "nbins: " << nbins << endl;
  cout << "norm: " << norm << endl;
  cout << "norm given: " << normGiven << endl;
  cout << "min: "  << minval << endl;
  cout << "max: "  << maxval << endl;
  cout << "minmaxgiven: " << minmaxGiven << endl;
  cout << "type: "  << histtype << endl;
  cout << "weighted: " << weighted << endl;
  
  // Read data to get max and minimum
  fstream fdata;
  double val;
  if(!minmaxGiven || !normGiven){
    fdata.open(fdataName.c_str(),std::fstream::in);
    double norm2 = 0;
    
    while(!fdata.eof()) {
      fdata >> val;

      if(!minmaxGiven){
        if(val>maxval){
          maxval = val;
        }
        if(val<minval){
          minval = val;
        }
      }
            if(!normGiven){
        norm2 +=1;
       }
    }
    
    cout << "computed max: " << maxval << endl;
    cout << "computed min: " << minval << endl;
    cout << "computed norm: " << norm2 << endl;

    fdata.close();

    minval = minval - eps;
    maxval = maxval + eps;
  }
  
  //histograms setup

  gsl_histogram * hist;
  gsl_histogram * hist2;
  
  if(strcmp (histtype,"log")==0){
    double lmin, lmax;
    int ibin;
    double *range = new double[nbins+1];
    double *dx = new double[nbins];
    double aux;
    
    lmin = log10(minval);
    lmax = log10(maxval);
    
    for(ibin = 0; ibin < nbins; ibin++){
      aux = lmin + ibin*(lmax-lmin)/(floor(nbins+1) - 1.);
      range[ibin] = pow(10.,aux);
    }

    range[nbins] = pow(10.,lmax);

    for(ibin = 0; ibin < nbins; ibin++){
      dx[ibin] = range[ibin+1] - range[ibin];
    }
    
    hist = gsl_histogram_alloc(nbins);
    hist2 = gsl_histogram_alloc(nbins);    
    gsl_histogram_set_ranges(hist, range,nbins+1);
    gsl_histogram_set_ranges(hist2, range,nbins+1);
    
    
    for(ibin = 0; ibin < nbins; ibin++){
      gsl_histogram_accumulate(hist2,range[ibin],1./dx[ibin]);
    }
  }
  
  if(strcmp (histtype,"uni")==0){
    hist = gsl_histogram_alloc(nbins);
    gsl_histogram_set_ranges_uniform (hist, minval, maxval);
  }
  // adds data
  fdata.open(fdataName.c_str(),std::fstream::in);
  double weight;
  while(!fdata.eof()) {

    if(weighted){
      fdata >> val >> weight;
      gsl_histogram_accumulate (hist, val, weight);
    }
    else{
      fdata >> val;
      gsl_histogram_increment (hist, val);
    }
  }

  fdata.close();

  if(!normGiven || norm<1.e-9){
    norm = gsl_histogram_sum(hist);
    cout << "normsum " << norm << endl;
  }
  
  gsl_histogram_scale (hist,1.0/norm);
  
  if(strcmp (histtype,"log")==0){
    gsl_histogram_mul(hist,hist2);
  }
  else{
    double delta = (maxval-minval)/nbins;
    gsl_histogram_scale (hist,1.0/delta);
  }

  FILE* fhist;

  
  fhist = fopen(fhistName.c_str(),"w");
  gsl_histogram_fprintf (fhist, hist, "%15.5g", "%15.5g");
  gsl_histogram_free (hist);

  if(strcmp (histtype,"log")==0){
    gsl_histogram_free (hist2);
  }
       
  fclose(fhist);
   
  cout << "End hist.cpp" <<endl;

  return EXIT_SUCCESS;
}
