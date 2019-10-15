#include <Riostream.h>

#include <TH1.h>

#include "waveform.h"

using namespace std;


//Standard waveform creator from histogram. 
//It allocates amplitude and time arrays with size fNsample given by the number of bin from original histogram. 
//It fills amplitude array. 
//It does NOT fill time array
//For a complete waveform definition should call SetTime after this
waveform::waveform(const TH1 *h){

  fNsample = ((TH1*)h)->GetXaxis()->GetNbins();
  fAmp = new Double_t[fNsample];
  fTime = new Double_t[fNsample];
  
  for(Int_t iSample=0; iSample<fNsample; iSample++){

    fAmp[iSample] = h->GetBinContent(iSample+1);

  }
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
  Double_t number = this->CalculateBaselineBin(bin_start, bin_end);
  return number;

}
