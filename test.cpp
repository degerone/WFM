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
#include <TGraph.h>

#include "waveform.h"

using namespace std;

TApplication app("gui",0,NULL);

  //test of fitting with single exp function
Double_t Pfnc(Double_t *x, Double_t *par){
  
  Double_t out = 0.;
  if(x[0]<par[0])
    out = 0.;
  else
    out = par[1]*(exp((-x[0]+par[0])/par[2])-exp((-x[0]+par[0])/par[3]));
  return out;
  
} 

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

    wfm[wfm_id]->SetTime(0, 2e-4);
  }
 
  /*
  for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++){

    Double_t *time = wfm[wfm_id]->GetTime();
    cout << "t min della wfm_id " << wfm_id << "  " <<  wfm[wfm_id]->GetTimeMin() << endl;
    cout << "t max della wfm_id " << wfm_id << "  " <<  wfm[wfm_id]->GetTimeMax() << endl;
    Int_t npoints = wfm[wfm_id]->GetNsample();

    Int_t bin;
    cout << "bin del tempo 0.6: " << wfm[wfm_id]->FindBin(-6e-5) << endl;

    Double_t baseline =0.;
    baseline = wfm[wfm_id]->CalculateBaselineBin(0,40);
    //    cout << "baseline :" << baseline << endl;
    Double_t baseline_time = 0.;
    Double_t start_time = (-wfm_id * 1e-5);
    Double_t end_time = (-wfm_id * 1e-5)+((wfm_id * 1e-5)*40/398); //fNSample (399) - 1??
    Int_t check_bin_start = wfm[wfm_id]->FindBin(start_time);
    Int_t check_bin_end = wfm[wfm_id]->FindBin(end_time);

    baseline_time = wfm[wfm_id]->CalculateBaseline(start_time, end_time);
    cout << "bin check start : " << check_bin_start << " check bin end : " << check_bin_end << endl;
    cout << "baseline :" << baseline << " baseline time : " << baseline_time << endl;

  }
  */

  /*
  waveform *test = new waveform();
  test->SetNsample(10);
  for(Int_t k=0; k<10; k++){

    test->SetAmpAt(0.3*k,k);

  }
  */

  /*
  Double_t *ampiezze = test->GetAmp();

  for(Int_t k=0; k<10; k++){

    cout << "ampiezza : " << ampiezze[k] << endl;

  }
  */

  
  waveform *invert[n_wfm];

  for(Int_t j=0; j<n_wfm; j++){

    invert[j] = wfm[j]->Invert_wfm();

  }
  

  TH1D *histo[10];
  TH1D *inverted_histo[10];

  for(Int_t h = 0 ; h< 10; h++){

    histo[h] = new TH1D("histo", "histo", 400, 0 ,400);
    inverted_histo[h] = new TH1D("inverted_histo", "inverted_histo", 400, 0, 400);

    /*
    for(Int_t j = 0; j<400; j++){


      Double_t inv_valore = invert[h]->GetAmpAt(j);
      inverted_histo[h]->SetBinContent(j,inv_valore);
      Double_t valore = wfm[h]->GetAmpAt(j);
      histo[h]->SetBinContent(j,valore);
    }
    */

    /*
    histo[h]->Write();
    inverted_histo[h]->Write();
    */
  }

  //Test della routine di fitting

  /*
    for(Int_t sample = 0; sample < npoints; sample++){

      cout << "tempo del sample " << sample << " della wfm " << wfm_id << " :" << time[sample] << endl;

    }
    */


  for(Int_t y=0; y<n_wfm; y++){


    Double_t tmax = invert[y]->GetTimeMax();
    Double_t tmin = invert[y]->GetTimeMin();

    cout << "analizzo la wfm : " << y << endl;
    cout << "tmax " << tmax << endl;
    cout << "tmin " << tmin << endl;

    TF1 *p = new TF1("Pulse", Pfnc, 0, 2e-4, 4);

    Double_t max, min;
    Int_t  max_bin, min_bin;

    //    cout << "fin qui " << endl;

    invert[y]->GetMaximum(max, max_bin, tmin, tmax);
    invert[y]->GetMinimum(min, min_bin, tmin, tmax);
    invert[y]->Fit(p);

    cout << " massimo : " << max << " nel bin " << max_bin << endl; 
    cout << " minimo : " << min << " nel bin " << min_bin << endl; 

    TGraph *g = new TGraph();
    g = invert[y]->GetGraph();

    //    Int_t enne = wfm[y]->GetNsample();

    //    Double_t *ampiezze = new Double_t[enne];
    //Double_t *ampiezze = wfm[y]->GetAmp();
    //Double_t *ampiezze = g->GetX();
    //Double_t *tempi = wfm[y]->GetTime();
    
    //for(Int_t p =0; p<enne; p++){

    // cout << "ampiezza bin " << p << " tempo " << tempi[p] <<" e' " << ampiezze[p] << endl;
    //}


    //TGraph *g = new TGraph(enne, tempi, ampiezze);    
    g->Write();
    //g->Delete();
    //g->Draw("AP*");
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
