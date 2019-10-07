class TH1;

class waveform {

 protected:
  
  Int_t fNsample; //Number of data points
  Double_t *fAmp; //[fNsample]Amplitude array
  
 public:

  waveform(const TH1 *h);

  Int_t GetNsample() const{return fNsample;}
  Double_t *GetAmp() const{return fAmp;}



};
