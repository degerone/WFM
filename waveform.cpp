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
