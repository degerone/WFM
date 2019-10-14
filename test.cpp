#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <TApplication.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TPad.h>
#include <TChain.h>
#include <TCanvas.h>

#include "waveform.h"

using namespace std;

TApplication app("gui",0,NULL);

int main(int n, char* argv[]) {

  cout << "\nAcquisisco waveform da file  "<< argv[1] << endl;
  cout << "\nSalvo dati in file " <<  argv[2] << endl;

  Int_t n_wfm =10;
  waveform *wfm[n_wfm];

  //Root file with raw data (waveform histograms from DAQ.cpp)
  TFile* in_file= TFile::Open(argv[1]);
  TFile f(argv[2],"recreate");
  
  
  TCanvas *canvas = new TCanvas();
  
  //Waveform acquisition from histograms
  for(int wfm_id=0; wfm_id<n_wfm; wfm_id++)
    {
      stringstream z;
      z << wfm_id;
      string name = "wfm" + z.str() + ";1";
      cout << name  << endl; 
      wfm[wfm_id] = new waveform((TH1*)in_file->Get(name.c_str()));                                             
      cout << "TEST 0 " << endl;
      //      Int_t npoints = wfm[wfm_id]->GetNsample();

      // Double_t *amp = new Double_t[npoints];
      //	= wfm[wfm_id]->GetAmp();
      //Double_t *time = wfm[wfm_id]->GetTime();
      cout << "TEST 0.1 " << endl;
    }

  for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++){

    wfm[wfm_id]->SetTime((-wfm_id * 1e-5),0.);
  }
 
  for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++){

    Double_t *time = wfm[wfm_id]->GetTime();
    cout << "t min della wfm_id " << wfm_id << "  " <<  wfm[wfm_id]->GetTimeMin() << endl;
    cout << "t max della wfm_id " << wfm_id << "  " <<  wfm[wfm_id]->GetTimeMax() << endl;
    Int_t npoints = wfm[wfm_id]->GetNsample();

    Int_t bin;
    cout << "bin del tempo 0.6: " << wfm[wfm_id]->FindBin(-6e-5) << endl;

    Double_t baseline =0.;
    baseline = wfm[wfm_id]->CalculateBaselineBin(0,40);
    cout << "baseline :" << baseline << endl;
    /*
    for(Int_t sample = 0; sample < npoints; sample++){

      cout << "tempo del sample " << sample << " della wfm " << wfm_id << " :" << time[sample] << endl;

    }
    */


  }




      cout << "TEST 2" << endl;
      //histo->Write();
      //histo->Delete();
      //cout << "TEST 3" << endl;
      //delete [] amp;
      //delete[] time;
      cout << "TEST 3" << endl;
      // }      
  
  f.Write();  
  return 0;
}
