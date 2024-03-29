#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


#include <TH1.h>
#include <TF1.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
//#include <TApplication.h>
//#include <TCanvas.h>
//#include <TSystem.h>
//#include <TPad.h>
//#include <TChain.h>
//#include <TCanvas.h>
//#include <TIterator.h>
//#include <TKey.h>

#include "waveform.h"

using namespace std;

//Fitting function with exp(rise) + exp(decay) function
//currently baseline is fixed to 0
//from code by M. Biasotti
// currently: PAR 2 = decay; PAR 3 = rise

Double_t Pfnc(Double_t *x, Double_t *par){

  Double_t out = 0.;
  if(x[0]<par[0])
    out = 0.;
  else
    out = par[1]*(exp((-x[0]+par[0])/par[2])-exp((-x[0]+par[0])/par[3]));
  return out;

}

Int_t main(Int_t n, Char_t* argv[]) {

  //Root file with raw data (waveform histograms from DAQ.cpp)
  TFile* in_file= TFile::Open(argv[1]);
  TFile f(argv[2],"recreate");

  cout << "\nAcquisisco waveform da file  "<< argv[1] << endl;
  cout << "\nSalvo dati in file " <<  argv[2] << endl;

  //Tree definition
  TTree* tree = new TTree("TES", "TES");

  //Variable to be saved on branch
  Double_t v_baseline;
  Double_t v_maximum;
  Double_t v_amplitude;//maximum - baseline
  Double_t v_conv_maximum;
  Double_t v_decay;//decay time from fit
  Double_t v_rise;//rise time from fit

  //  double v_integral;  
  
  Bool_t draw_conv = false; // on/off convolution waveform saving on root file
  Bool_t draw_fit = true; // on/off saving graph with superimposed fit function in output tree
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
  TBranch *b_conv_maximum = tree->Branch("b_conv_maximum", &v_conv_maximum, "v_conv_maximum/D");  
  TBranch *b_decay = tree->Branch("b_decay", &v_decay, "v_decay/D");
  TBranch *b_rise = tree->Branch("b_rise", &v_rise, "v_rise/D");
  
/*
  TBranch *b_baseline_RMS = tree->Branch("b_baseline_RMS", &v_baseline_RMS, "v_baseline_RMS/D");    
  TBranch *b_integral = tree->Branch("b_integral", &v_integral, "v_integral/D");  
  TBranch *b_integral_mod = tree->Branch("b_integral_mod", &v_integral_mod, "v_integral_mod/D");  
  TBranch *b_decay_time = tree->Branch("b_decay_time", &v_decay_time, "v_decay_time/D");  
    TBranch *b_chi2 = tree->Branch("b_chi2",&v_chi2,"v_chi2/D");
  TBranch *b_issaturated = tree->Branch("b_issaturated", &v_issaturated, "v_issaturated/B");
  TBranch *b_conv = tree->Branch("b_conv", &v_conv, "v_conv/D");
  */
  

  Int_t n_wfm = 0;
  //number of wfm to be acquired - should not be hardcoded...
  //Int_t n_wfm = 1000;
  //following lines allow to count histograms in source file, this will be n_wfm 
  //comment and set n_wfm by hand if you want to set n_wfm to different number
  TKey *key;
  TIter next((TList *)in_file->GetListOfKeys());
  while((key = (TKey *)next()))
    {
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if(cl->InheritsFrom("TH1"))
	{
	  TH1 *h = (TH1 *)key->ReadObj(); 
	  n_wfm++;
	}
    }
  cout << "Found " << n_wfm << " wfm in file " << argv[1] << endl;
  
  //Waveform array
  waveform *wfm[n_wfm];            //waveform array
  waveform *invert_wfm[n_wfm];     //inverted waveform array
  waveform *temp = new waveform(); //template waveform
  waveform *conv[n_wfm];           //convoluted wfm array

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
  Double_t t_max_conv = 2*t_max; //convolution domain is twice the original wfm

  //template creation for optimum filtering
  temp->MakeTemplate(invert_wfm,50);
  TGraph *g_template = new TGraph();
  g_template = temp->Graph_from_wfm();
  g_template->SetName("template_wfm");
  g_template->SetTitle("template_wfm");
  g_template->Write();




  //Execute tasks and save variables to tree
  //for(Int_t wfm_id=0; wfm_id<100; wfm_id++)
  for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++)
    {
      v_baseline = invert_wfm[wfm_id]->CalculateBaseline(0,4e-5);
      v_maximum = invert_wfm[wfm_id]->GetMaximum(t_min, t_max);
      v_amplitude = v_maximum - v_baseline;
      conv[wfm_id] = invert_wfm[wfm_id]->Convolution(temp);
      v_conv_maximum = conv[wfm_id]->GetMaximum(t_min, t_max_conv);
      
      //FIT ROUTINE STARTS HERE!
      TF1 *func = new TF1("Pulse", Pfnc, 0, 2e-4, 4);
      //      cout << "check Fit, GetMaximum, GetMinimum " << endl;
      // for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++)
      //{
      Double_t tmax = invert_wfm[wfm_id]->GetTimeMax();
      Double_t tmin = invert_wfm[wfm_id]->GetTimeMin();
      Double_t max, min;
      Int_t  max_bin, min_bin;
      
      invert_wfm[wfm_id]->GetMaximum(max, max_bin, tmin, tmax);
      invert_wfm[wfm_id]->GetMinimum(min, min_bin, tmin, tmax);
      invert_wfm[wfm_id]->Fit(func, "WRMQ");
      //FIT ROUTINE FINISHES HERE!
      //NOW DRAW (OPTIONAL) AND EXTRACT PARAMETERS
      if(draw_fit)
	{
	  TGraph *g_fit = new TGraph();
	  stringstream z;
	  z << wfm_id;
	  string name_fit = "graph_fit_inv_wfm_" + z.str();
	  char name_fit_c[name_fit.size()+1];
	  strcpy(name_fit_c, name_fit.c_str());
	  g_fit = invert_wfm[wfm_id]->GetGraph();
	  g_fit->SetName(name_fit_c);
	  g_fit->SetTitle(name_fit_c);
	  g_fit->Write();
	}
      
      //CHECK PARAMETERS!
      v_rise = func->GetParameter(2);
      v_decay = func->GetParameter(3);
  
      //Only to draw convolution to root file
      if(draw_conv)
	{
	  TGraph *g_conv = new TGraph();
	  stringstream z;
	  z << wfm_id;
	  string name_conv = "graph_conv_inv_wfm_" + z.str();
	  char name_conv_c[name_conv.size()+1];
	  strcpy(name_conv_c, name_conv.c_str());
	  g_conv = conv[wfm_id]->Graph_from_wfm();
	  g_conv->SetName(name_conv_c);
	  g_conv->SetTitle(name_conv_c);
	  g_conv->Write();
	}
      tree->Fill();
    }
  
  in_file->Close();
  tree->Print();
  //tree->Write();
  f.Write();
  return 0;
  
}//end of main
