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

Int_t main(Int_t n, Char_t* argv[]) {


  cout << "\nAcquisisco waveform da file  "<< argv[1] << endl;
  cout << "\nSalvo dati in file " <<  argv[2] << endl;

  //Tree definition
  TTree* tree = new TTree("TES", "TES");

  //Variable to be saved on branch
  Double_t v_baseline;
  Double_t v_maximum;
  Double_t v_amplitude;//maximum - baseline
  //  double v_integral;  
  
  /*
double v_integral_mod;
  double v_baseline_RMS;
  double v_decay_time;
  double v_pulse_amplitude;
  double v_risetime;
  double v_chi2;
  double v_conv;
  double v_max;
  bool v_issaturated;
  */


  //Branch definition
  TBranch *b_baseline = tree->Branch("b_baseline", &v_baseline, "v_baseline/D");  
  TBranch *b_maximum = tree->Branch("b_maximum", &v_maximum, "v_maximum/D");  
  TBranch *b_amplitude = tree->Branch("b_amplitude", &v_amplitude, "v_amplitude/D");  
  
/*
  TBranch *b_baseline_RMS = tree->Branch("b_baseline_RMS", &v_baseline_RMS, "v_baseline_RMS/D");    
  TBranch *b_integral = tree->Branch("b_integral", &v_integral, "v_integral/D");  
  TBranch *b_integral_mod = tree->Branch("b_integral_mod", &v_integral_mod, "v_integral_mod/D");  
  TBranch *b_decay_time = tree->Branch("b_decay_time", &v_decay_time, "v_decay_time/D");  
    TBranch *b_chi2 = tree->Branch("b_chi2",&v_chi2,"v_chi2/D");
  TBranch *b_issaturated = tree->Branch("b_issaturated", &v_issaturated, "v_issaturated/B");
  TBranch *b_conv = tree->Branch("b_conv", &v_conv, "v_conv/D");
  */


  //number of wfm to be acquired - should not be hardocded...
  Int_t n_wfm = 100;
  //Waveform array
  waveform *wfm[n_wfm];
  waveform *invert_wfm[n_wfm];

  //Root file with raw data (waveform histograms from DAQ.cpp)
  TFile* in_file= TFile::Open(argv[1]);

  //Waveform acquisition from histograms
  for(int wfm_id=0; wfm_id<n_wfm; wfm_id++)
    {
      stringstream z;
      z << wfm_id;
      string name = "wfm" + z.str() + ";1";
      wfm[wfm_id] = new waveform((TH1*)in_file->Get(name.c_str()));
      wfm[wfm_id]->SetTime(0,2e-4);
      invert_wfm[wfm_id] = wfm[wfm_id]->Invert_wfm();
    }

  Double_t t_min = wfm[1]->GetTimeMin();//t_min and t_max are supposed to be the same for all the waveform
  Double_t t_max = wfm[1]->GetTimeMax();

  for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++)
    {
      v_baseline = wfm[wfm_id]->CalculateBaseline(0,4e-5);
      v_maximum = invert_wfm[wfm_id]->GetMaximum(t_min, t_max);
      v_amplitude = v_maximum - v_baseline;
      tree->Fill();
      if(wfm_id%10==0)
	cout << "Analized event number: " << wfm_id << endl;
    }
  
  in_file->Close();
  TFile f(argv[2],"recreate");
  tree->Print();
  tree->Write();
  f.Write();
  return 0;
  
}//end of main
