#include <Riostream.h>

#include <TH1.h>

#include "waveform.h"

using namespace std;

waveform::waveform(const TH1 *h){

  fNsample = ((TH1*)h)->GetXaxis()->GetNbins();
  fAmp = new Double_t[fNsample];

  Int_t iPnt;
  for(iPnt=0; iPnt<fNsample; iPnt++){

    fAmp[iPnt] = h->GetBinContent(iPnt+1);
    //cout << "ampiezza del bin " << iPnt << " " << fAmp[iPnt] << endl;
  }
}

void waveform::SetTime(Double_t tmin, Double_t tmax){

  fTimeMin = tmin;
  fTimeMax = tmax;
  fSamplingInterval = (tmax-tmin)/(fNsample - 1);
}

Double_t waveform::GetTimeAt(Int_t sample){

  
  if(sample < 0) return -1e5;
  if(sample >= fNsample) return 1e5;
  Double_t time = fTimeMin + sample*fSamplingInterval;
  return time;

}
