class TH1;
class TGraph;
class TF1;

class waveform {

 protected:
  
  Int_t fNsample; //Number of data points
  Double_t *fAmp; //[fNsample]Amplitude array
  Double_t *fTime;//[fNsample]Time array
  Double_t fTimeMin;
  Double_t fTimeMax;
  Double_t fSamplingInterval;
  TGraph *fGraph;//Graph used for fitting and drawing
  
 public:

  waveform();             //creator
  waveform(const TH1 *h); //creator from histogram

  void SetNsample(Int_t n_sample);
  void SetTime(Double_t tmin, Double_t tmax);
  void SetAmpAt(Double_t value, Int_t bin) {fAmp[bin] = value;}

  Int_t GetNsample() const{return fNsample;}
  Double_t *GetAmp() const{return fAmp;}
  Double_t GetAmpAt(Int_t bin) {return fAmp[bin];}
  Double_t *GetTime() const{return fTime;}
  Double_t GetTimeMin() const{return fTimeMin;}
  Double_t GetTimeMax() const{return fTimeMax;}
  Double_t GetTimeAt(Int_t sample);
  TGraph *GetGraph() {return fGraph;}
  void GetMaximum(Double_t &max, Int_t &max_bin, Double_t tmin, Double_t tmax);
  void GetMinimum(Double_t &min, Int_t &min_bin, Double_t tmin, Double_t tmax);

  Int_t FindBin(Double_t time);
  Double_t CalculateBaselineBin(Double_t start_bin, Double_t end_bin);
  Double_t CalculateBaseline(Double_t t_start, Double_t t_end);
  void Fit(TF1 *func);//options are "WRM" by default 
  void Fit(TF1 *func, Option_t *opt);
  TGraph *Graph_from_wfm();
  waveform *Invert_wfm();
  waveform* MakeTemplate(waveform  *wfm[], Int_t n_wfm_used);
  waveform* Convolution(waveform* kernel);

};
