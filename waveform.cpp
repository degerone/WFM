#include <Riostream.h>

#include <TH1.h>
#include <TGraphErrors.h>

#include "waveform.h"

using namespace std;

Double_t Pfnc(Double_t *x, Double_t *par){

  Double_t out = 0.;
    if(x[0]<par[0])
      out = 0;
    else
      out = par[1]*(exp((-x[0]+par[0])/par[2])-exp((-x[0]+par[0])/par[3]));
  return out;

}



//Standard waveform creator from histogram. 
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
  memset(fAmp, 0, sizeof(Double_t)*n_sample);
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

void waveform::Fit(){

  if(!fGraph) 
    //    fGraph = new TGraph(fNsample, fTime, fAmp);
    fGraph = new TGraph(fNsample);
  memcpy(fGraph->GetX(), fTime, sizeof(Double_t)*fNsample);
  memcpy(fGraph->GetY(), fAmp, sizeof(Double_t)*fNsample);
 
}
