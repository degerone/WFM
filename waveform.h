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

  waveform();
  waveform(const TH1 *h);

  Int_t GetNsample() const{return fNsample;}
  void SetNsample(Int_t n_sample);
  Double_t *GetAmp() const{return fAmp;}
  Double_t *GetTime() const{return fTime;}
  void SetTime(Double_t tmin, Double_t tmax);
  void SetAmpAt(Double_t value, Int_t bin) {fAmp[bin] = value;}
  Double_t GetTimeMin() const{return fTimeMin;}
  Double_t GetTimeMax() const{return fTimeMax;}
  Double_t GetTimeAt(Int_t sample);
  Int_t FindBin(Double_t time);
  Double_t CalculateBaselineBin(Double_t start_bin, Double_t end_bin);
  Double_t CalculateBaseline(Double_t t_start, Double_t t_end);
  waveform Invert_wfm();

};
