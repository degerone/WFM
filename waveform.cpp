#include <Riostream.h>

#include <TH1.h>
#include <TGraphErrors.h>
#include <TF1.h>

#include "waveform.h"

using namespace std;

//Waveform creator from histogram. 
//It allocates amplitude and time arrays with size fNsample given by the number of bin from original histogram. 
//It fills amplitude array. 
//It does NOT fill time array
//For a complete waveform definition should call SetTime after this
waveform::waveform(const TH1 *h)
  : fNsample(0), fGraph(0), fAmp(0), fTime(0)

{

  fNsample = ((TH1*)h)->GetXaxis()->GetNbins();
  fAmp = new Double_t[fNsample];
  fTime = new Double_t[fNsample];
  
  for(Int_t iSample=0; iSample<fNsample; iSample++){

    fAmp[iSample] = h->GetBinContent(iSample+1);

  }
}


//-------------------------------------------------------------------------------------------------------------//
//waveform basic constructor. Must be followed by a SetNsample
waveform::waveform()
  : fNsample(0), fGraph(0), fAmp(0), fTime(0)

{
}


//-------------------------------------------------------------------------------------------------------------//
//Allocate fAmp[n_sample], fTime[n_sample] for waveform created with waveform()
void waveform::SetNsample(Int_t n_sample){

  fNsample = n_sample;
  fAmp = new Double_t[n_sample];
  fTime = new Double_t[n_sample];
  memset(fAmp, 0, sizeof(Double_t)*n_sample);
  memset(fTime, 0, sizeof(Double_t)*n_sample);

}


//-------------------------------------------------------------------------------------------------------------//
//Set time range for waveform.
//It fills time array.
void waveform::SetTime(Double_t tmin, Double_t tmax){

  fTimeMin = tmin;
  fTimeMax = tmax;
  fSamplingInterval = (tmax-tmin)/(fNsample - 1);
  for(Int_t iSample = 0; iSample < fNsample; iSample++){
    
    fTime[iSample] = fSamplingInterval * iSample;
  }
}


//-------------------------------------------------------------------------------------------------------------//
//Return time of sample
//If sample is lower (greater) than 0 (fNsample) returns -1e5 (1e5)
Double_t waveform::GetTimeAt(Int_t sample){

  if(sample < 0) return -1e5;
  if(sample >= fNsample) return 1e5;
  Double_t time = fTimeMin + sample*fSamplingInterval;
  return time;

}


//-------------------------------------------------------------------------------------------------------------//
//Return bin number containing time 
Int_t waveform::FindBin(Double_t time){

  Int_t bin;
  if(time < fTimeMin) bin = -1;
  else if (time > fTimeMax) bin = fNsample - 1;
  else bin = static_cast<int>((fNsample - 1) * (time - fTimeMin) / (fTimeMax - fTimeMin));
  return bin;

}


//-------------------------------------------------------------------------------------------------------------//
//Return baseline calculated from bin start_bin to bin end_bin
Double_t waveform::CalculateBaselineBin(Double_t start_bin, Double_t end_bin){

  Double_t baseline = 0.;
  Int_t n_bin = end_bin - start_bin;
  if(n_bin < 0) return 0;
  for(Int_t i_bin = 0; i_bin < n_bin; i_bin++){

    baseline += fAmp[i_bin];

  }

  baseline /= n_bin;
  return baseline;
}


//-------------------------------------------------------------------------------------------------------------//
//Return baseline calculated from t_start to t_end
Double_t waveform::CalculateBaseline(Double_t t_start, Double_t t_end){

  //  Double_t baseline = 0.;
  Double_t bin_start = this->FindBin(t_start);
  Double_t bin_end = this->FindBin(t_end);
  Double_t baseline_time = this->CalculateBaselineBin(bin_start, bin_end);
  return baseline_time;

}


//-------------------------------------------------------------------------------------------------------------//
//Return inverted wfm: fAmp[i] -> -fAmp[i]
waveform* waveform::Invert_wfm(){

  waveform *inverted = new waveform();
  Int_t n_sample = this->GetNsample();
  inverted->SetNsample(n_sample);
  
  for(Int_t i_bin = 0; i_bin<fNsample; i_bin++){

    Double_t amplitude = this->GetAmpAt(i_bin);
    inverted->SetAmpAt(-amplitude, i_bin);

  }
 
  Double_t time_min = this->GetTimeMin();
  Double_t time_max = this->GetTimeMax();
  inverted->SetTime(time_min, time_max);

  return inverted;
}


//-------------------------------------------------------------------------------------------------------------//
//Return the maximum and the corresponding bin of the current waveform calculated in the range (tmin, tmax)
void waveform::GetMaximum(Double_t &max, Int_t &max_bin, Double_t tmin, Double_t tmax){

  Int_t start_bin;
  Int_t end_bin;

  start_bin = FindBin(tmin);
  end_bin = FindBin(tmax);

  Double_t *pointer_to_max = max_element(fAmp + start_bin, fAmp + end_bin);
  max = *pointer_to_max;
  max_bin = pointer_to_max - fAmp;

}


//-------------------------------------------------------------------------------------------------------------//
//Return the minimum and the corresponding bin if the current waveform calculated in the range (tmimn, tmax)
void waveform::GetMinimum(Double_t &min, Int_t &min_bin, Double_t tmin, Double_t tmax){

  Int_t start_bin;
  Int_t end_bin;

  start_bin = FindBin(tmin);
  end_bin = FindBin(tmax);

  Double_t *pointer_to_min = min_element(fAmp + start_bin, fAmp + end_bin);
  min = *pointer_to_min;
  min_bin = pointer_to_min - fAmp;

}


//-------------------------------------------------------------------------------------------------------------//
//Fit the current waveform. Currently with a 2 exp function (1 exp for rise and 1 exp for decay)
//STILL TO BE IMPROVED!!
void waveform::Fit(TF1 *func){

  if(!fGraph) 
    fGraph = new TGraph(fNsample);
  memcpy(fGraph->GetX(), fTime, sizeof(Double_t)*fNsample);
  memcpy(fGraph->GetY(), fAmp, sizeof(Double_t)*fNsample);
  Int_t max_bin;
  Double_t max;
  this->GetMaximum(max, max_bin, fTimeMin, fTimeMax);
  Double_t time_max = GetTimeAt(max_bin);

  //  cout<< "fNsample " << fNsample << endl;
  //  cout << "fNsamplingInterval : " << fSamplingInterval << endl;
  //  cout << "time max nella funzione di fit : " << time_max << endl;

  func->SetParLimits(0,time_max - 2e-5, time_max);
  func->SetParLimits(1,0,5.);
  func->SetParLimits(2,0,1e-4);
  func->SetParLimits(3,0,1e-4);

  func->SetParameter(0, time_max - 3e-6);
  func->SetParameter(1, max);
  func->SetParameter(2, 2e-6);
  func->SetParameter(3, 3e-5);

  fGraph->Fit(func, "WRM", 0, fTimeMax/2);

}


//-------------------------------------------------------------------------------------------------------------//
//Obtain a TGraph from the current waveform
TGraph* waveform::Graph_from_wfm(){

  fGraph = new TGraph(fNsample);
  memcpy(fGraph->GetX(), fTime, sizeof(Double_t)*fNsample);
  memcpy(fGraph->GetY(), fAmp, sizeof(Double_t)*fNsample);
  
  return fGraph;

}


//-------------------------------------------------------------------------------------------------------------//
//Creates a template waveform starting from wfm array using n_wfm_used waveforms.
waveform* waveform::MakeTemplate(waveform* wfm[], Int_t n_wfm_used){

  Int_t n_sample = wfm[0]->GetNsample();//all waveform are equal with same number of sample, use wfm[0]
  this->SetNsample(n_sample);//allocates fAmp and fTime
  Double_t *Amp = this->GetAmp();

  for(Int_t i_wfm = 0; i_wfm < n_wfm_used; i_wfm++){
    
    for(Int_t i_bin = 0; i_bin < n_sample; i_bin++){

        Amp[i_bin] += wfm[i_wfm]->GetAmpAt(i_bin)/n_wfm_used;
    }
  }

  Double_t t_min = wfm[0]->GetTimeMin();
  Double_t t_max = wfm[0]->GetTimeMax();
  this->SetTime(t_min, t_max);
  return this;
}


//-------------------------------------------------------------------------------------------------------------//
//Return convolution between wfm and a kernel wfm (usually a template wfm)
//Convolution wfm has double entries as wfm/kernel ones
waveform* waveform::Convolution(waveform* wfm, waveform* kernel){

  Int_t n_sample = wfm->GetNsample();
  Double_t t_min = wfm->GetTimeMin();
  Double_t t_max = wfm->GetTimeMax();

  this->SetNsample(n_sample*2);
  Double_t temp_value = 0.;

  for(Int_t i_bin_conv = 0; i_bin_conv< 2*n_sample-1; i_bin_conv++)
      {
	temp_value=0.0;
	
	for(Int_t j_bin = 0; j_bin < n_sample; j_bin++)
	  {
	    if((i_bin_conv - j_bin) >=0 && (i_bin_conv - j_bin) < n_sample)
	     
	      temp_value = temp_value + kernel->GetAmpAt(i_bin_conv - j_bin) * wfm->GetAmpAt(j_bin);
        
	  }

	this->SetAmpAt(temp_value, i_bin_conv);
	
      }
  
  this->SetTime(t_min, t_max*2);
  return this;
}
