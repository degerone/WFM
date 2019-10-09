class TH1;

class waveform {

 protected:
  
  Int_t fNsample; //Number of data points
  Double_t *fAmp; //[fNsample]Amplitude array
  Double_t *fTime;//[fNsample]Time array
  Double_t fTimeMin;
  Double_t fTimeMax;
  Double_t fSamplingInterval;
  
 public:

  waveform(const TH1 *h);

  Int_t GetNsample() const{return fNsample;}
  Double_t *GetAmp() const{return fAmp;}
  Double_t *GetTime() const{return fTime;}
  void SetTime(Double_t tmin, Double_t tmax);
  Double_t GetTimeMin() const{return fTimeMin;}
  Double_t GetTimeMax() const{return fTimeMax;}
  Double_t GetTimeAt(Int_t sample);

};
