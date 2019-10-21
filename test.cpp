// Macro used for debugging methods of class waveforms.
// Usage: compile with 
// g++ test.cpp waveform.cpp `root-config --cflags --glibs` -o test
// Launch with:
// ./test input_file_name.root output_file_name.root
// input_file_name is currently a file like Th229notte0.root (DAQ for Th229 experiment)
// Activate method by setting corresponding bool variable true


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
 
//Fitting function with exp(rise) + exp(decay) function
//baseline fixed to 0
Double_t Pfnc(Double_t *x, Double_t *par){
  
  Double_t out = 0.;
  if(x[0]<par[0])
    out = 0.;
  else
    out = par[1]*(exp((-x[0]+par[0])/par[2])-exp((-x[0]+par[0])/par[3]));
  return out;
  
} 


int main(int n, char* argv[]) {

  cout << "\nAcquiring waveforms from file:  "<< argv[1] << endl;
  cout << "\nSave data to file: " <<  argv[2] << endl;

  Int_t n_wfm =10;      //number of waveform to be acquired
  waveform *wfm[n_wfm]; //wfm array creation
  waveform *inverted[n_wfm];//inverted wfm array creation

  //Root file with raw data (waveform histograms from DAQ file)
  TFile* in_file= TFile::Open(argv[1]);
  TFile f(argv[2],"recreate");

  //Boolean variables to activate debugging outputs
  bool check_wfm_name = true;  
  bool check_time = true; 
  bool check_find_bin = true;
  bool check_baseline_bin = true;
  bool check_baseline_time = true;
  bool check_set_amp = true;
  bool check_graph_from_wfm = true;
  bool check_invert = false;
  bool check_fit = true; //need check_invert = true

  //Waveform acquisition from histograms
  //wfm name is defined by DAQ file 
  if(check_wfm_name)
	  cout << "Check acquired waveforms name: " << endl;
  for(int wfm_id=0; wfm_id<n_wfm; wfm_id++)
    {
      stringstream z;
      z << wfm_id;
      string name = "wfm" + z.str() + ";1";
      if(check_wfm_name)
	  cout << name  << endl; 
      wfm[wfm_id] = new waveform((TH1*)in_file->Get(name.c_str()));                                             
    }

  //Set time for each waveform 
  //(currently time min and max are hard coded and set to (0, 2e-4) from DAQ
  for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++)
    {
      wfm[wfm_id]->SetTime(0, 2e-4);
    }
 
  //Check waveform time, if activated
  if(check_time)
    {
      cout << "Check the time for each waveform: " << endl;
      for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++)
	{
	  cout << "wfm_id: " << wfm_id << " t min: " <<  wfm[wfm_id]->GetTimeMin() << " t_max: " <<  wfm[wfm_id]->GetTimeMax() << endl;
	}
    }

  //Check find bin, if activated
  if(check_find_bin)
    {
      cout << "Check the time of some bin (wfm_id * 1e-6us): " << endl;
      for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++)
	{
	  Double_t time = wfm_id * 1e-6;
	  cout << "wfm_id: " << wfm_id << " time: " << time << " bin: " << wfm[wfm_id]->FindBin(time) << endl;
	}
    }
  
  //check baseline calculation in a bin range, if activated
  if(check_baseline_bin)
    {
    cout << "Check baseline calculation in the range bin(0,40): " << endl;
      for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++)
	{
	    Double_t baseline =0.;
	    baseline = wfm[wfm_id]->CalculateBaselineBin(0,40);
	    cout << "wfm_id: " << wfm_id << " baseline: " << baseline << endl;
	}
    }

  //check baseline calculation in a time range, if activated
  if(check_baseline_time)
    {
    cout << "Check baseline calculation in the time range corresponding to bin (0,40): " << endl;
      for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++)
	{
	  Double_t time_min = wfm[wfm_id]->GetTimeAt(0);
	  Double_t time_max = wfm[wfm_id]->GetTimeAt(40);
	  Double_t baseline_time =0.;
	  baseline_time = wfm[wfm_id]->CalculateBaseline(time_min,time_max);
	  cout << "wfm_id: " << wfm_id << " baseline: " << baseline_time << endl;
	}
    }

  //check SetAmpAt, GetAmp, GetAmpAt methods
  if(check_set_amp)
    {
      waveform *test = new waveform();
      test->SetNsample(10);
      cout << "Check SetAmpAt() " << endl;
      for(Int_t k=0; k<10; k++) //loop for amplitude allocation
	{
	  test->SetAmpAt(0.3*k,k);
	}
      Double_t *amp = test->GetAmp();

      for(Int_t k_bin=0; k_bin<10; k_bin++)
	{
	  cout << "Amplitude of bin : " << k_bin << " is: " <<  amp[k_bin] << endl;
	  cout << "Amplitude of bin : " << k_bin << " is: " << test->GetAmpAt(k_bin) << " also with GetAmpAt() " << endl;
	}    
    }
  
  //Check graph from wfm -> graphs must appear in output_file.root!
  if(check_graph_from_wfm)
    {
      cout << "Graph from file is activated! You should find some graphs in output file! " << endl;
      for(Int_t wfm_id =0; wfm_id < n_wfm; wfm_id++)
	{
	  stringstream z;
	  z << wfm_id;
	  string name = "graph_wfm_" + z.str();
	  char name_c[name.size()+1];
	  strcpy(name_c, name.c_str());
	  TGraph *g = new TGraph();
	  g = wfm[wfm_id]->Graph_from_wfm();
	  g->SetName(name_c);
	  g->SetTitle(name_c);
	  g->Write();
	}
    }


  //check wfm inverted
  if(check_invert)
    {
      cout << "Inverted wfm is activated! You should find some inverted wfms in output file! " << endl;
      for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++)
	{ 
	  inverted[wfm_id] = wfm[wfm_id]->Invert_wfm();
	  stringstream z;
	  z << wfm_id;
	  string name_inv = "graph_inv_wfm_" + z.str();
	  char name_inv_c[name_inv.size()+1];
	  strcpy(name_inv_c, name_inv.c_str());
	  TGraph *g = new TGraph();
	  g = inverted[wfm_id]->Graph_from_wfm();
	  g->SetName(name_inv_c);
	  g->SetTitle(name_inv_c);
	  g->Write();
	}
    }

  
  //Test della routine di fitting
  //inverted wfm is created in check_invert, so it should be set to true

  if(check_fit)
    {
      if(check_invert == false) 
	{
	  cout << "check invert is false! inverted wfm are not created -> Fit is skipped!" << endl;
	}
      else
	{
	  TF1 *func = new TF1("Pulse", Pfnc, 0, 2e-4, 4);
	  cout << "check Fit, GetMaximum, GetMinimum " << endl;
	  for(Int_t wfm_id=0; wfm_id<n_wfm; wfm_id++)
	    {
	      Double_t tmax = inverted[wfm_id]->GetTimeMax();
	      Double_t tmin = inverted[wfm_id]->GetTimeMin();
	      Double_t max, min;
	      Int_t  max_bin, min_bin;

	      inverted[wfm_id]->GetMaximum(max, max_bin, tmin, tmax);
	      inverted[wfm_id]->GetMinimum(min, min_bin, tmin, tmax);

	      cout << "Wfm id: " << wfm_id << " has a maximum " << max << " in bin: " << max_bin << endl; 
	      cout << "Wfm id: " << wfm_id << " has a minimum " << min << " in bin: " << min_bin << endl; 
	    	    
	      inverted[wfm_id]->Fit(func);
	     
	      TGraph *g_fit = new TGraph();
	      stringstream z;
	      z << wfm_id;
	      string name_fit = "graph_fit_inv_wfm_" + z.str();
	      char name_fit_c[name_fit.size()+1];
	      strcpy(name_fit_c, name_fit.c_str());
	      TGraph *g = new TGraph();
	      g = inverted[wfm_id]->GetGraph();
	      g->SetName(name_fit_c);
	      g->SetTitle(name_fit_c);
	      g->Write();
	    }
	}  
    }
  


  // waveform* base = new waveform();

  //waveform *temp = base->MakeTemplate(wfm,2);

  /*  
  waveform *temp = new waveform();
  temp->MakeTemplate(wfm,5);

//      temp->MakeTemplate(wfm, 2);
      
      Double_t *amp_templ = temp->GetAmp();
      Int_t bin_templ = temp->GetNsample();
      cout << "bin_templ " << bin_templ << endl;

      TH1D *h = new TH1D("h", "h", bin_templ, 0, bin_templ);

      for(Int_t i_bin = 0; i_bin < bin_templ; i_bin ++){

	h->SetBinContent(i_bin, amp_templ[i_bin]);

      }

      h->Write();
  */

  /*  
      waveform *conv = new waveform();
      conv->Convolution(wfm[0],temp);
      cout << "check time min " << wfm[0]->GetTimeMin() << " check time max " << wfm[0]->GetTimeMax() << endl;

      //      cout << "prova conv : fNsample " << conv->GetNsample() << endl;
      Double_t *amp_conv;
      amp_conv = conv->GetAmp();
      
      for(Int_t i_conv = 0; i_conv < conv->GetNsample(); i_conv++){

	cout << "ampiezza conv " << amp_conv[i_conv] << endl;
      }
      
      TGraph *grafico = new TGraph("test");
      grafico = conv->Graph_from_wfm();
      grafico->Write();

      TH1D *h2 = new TH1D("h2", "h2", conv->GetNsample(), 0, conv->GetNsample());
*/


  /*
      for(Int_t i_bin = 0; i_bin < conv->GetNsample(); i_bin ++){

	h2->SetBinContent(i_bin, conv->GetAmpAt(i_bin));

      }

      h2->Write();
  */


      //      temp->Fit(p);
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
