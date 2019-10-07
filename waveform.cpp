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
    cout << "ampiezza del bin " << iPnt << " " << fAmp[iPnt] << endl;
  }

}
