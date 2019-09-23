// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "wx/wfstream.h"
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include "helper.h"
#include "simple_histogram.h"
#include "A3.h"
#include "A4.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

using namespace Assignment3;
using namespace Assignment4;

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../samples/sample.xpm"
#endif

struct Package;
class MyFrame;
class wxImagePanel;

struct Dialog : public wxDialog
{
  wxButton* ok;
  wxButton* cancel;
  wxPanel* panel;
  wxStaticBitmap* image;
  wxImage baseImage;
  wxImage Output;
  MyFrame* parent;
  wxBoxSizer* vbox;
  wxBoxSizer* hbox;

  void Init(std::string s, wxImage* i, MyFrame* p)
  {
    parent = p;

    panel = new wxPanel(this, wxID_ANY);

    vbox = new wxBoxSizer(wxVERTICAL);
    hbox = new wxBoxSizer(wxHORIZONTAL);

    ok = new wxButton(this, wxID_OK, "OK");
    cancel = new wxButton(this, wxID_CANCEL, "Cancel");

    hbox->Add(ok, 1);
    hbox->Add(cancel, 1);

    vbox->Add(panel, 1);

    if (i->IsOk())
    {
      baseImage = i->Copy();
      Output = i->Copy();
      image = new wxStaticBitmap(this, wxID_ANY, wxBitmap(baseImage));
      vbox->Add(image, 0, wxALL, 5);
    }
  }

  void EndInit()
  {
    vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    SetSizer(vbox);

    SetAutoLayout(true);
    Layout();
    Fit();

    Center();
  }

  Dialog(std::string s)
    : wxDialog(NULL, wxID_ANY, s)
  {
  }

};

struct UnsharpDialog : public Dialog
{
  double sigma;
  int N;
  double K;
  wxSpinCtrlDouble* sigmaSlide;
  wxSpinCtrlDouble* Nslide;
  wxSpinCtrlDouble* Kslide;
  wxStaticText* sigmaText;
  wxStaticText* NText;
  wxStaticText* KText;

  void OnKInput(wxCommandEvent& WXUNUSED(event))
  {
    Kslide->Enable(false);
    Kslide->Enable();
  }

  void OnKSpinner(wxSpinDoubleEvent& WXUNUSED(event))
  {
    if (K == Kslide->GetValue()) return;
    K = Kslide->GetValue();
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = UnsharpMaskOperation(w, h, data, N, sigma, K);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  UnsharpDialog(std::string s, wxImage* i, MyFrame* p, int n, double sig)
    : Dialog(s)
  {
    N = n;
    sigma = sig;
    K = 0.0;
    Init(s, i, p);

    KText = new wxStaticText(this, wxID_ANY, "K Constant");
    Kslide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 18, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    Kslide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    Kslide->SetIncrement(1.0);
    vbox->Add(KText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(Kslide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = UnsharpMaskOperation(w, h, data, N, sigma, K);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }

    EndInit();
  }
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(UnsharpDialog, wxDialog)
  EVT_TEXT_ENTER(wxID_HIGHEST + 18, UnsharpDialog::OnKInput)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 18, UnsharpDialog::OnKSpinner)
END_EVENT_TABLE()

struct GaussianDialog : public Dialog
{
  double sigma;
  int    N;
  wxSpinCtrlDouble* sigmaSlide;
  wxSpinCtrlDouble* Nslide;
  wxStaticText* sigmaText;
  wxStaticText* NText;

  void OnNSpinner(wxSpinDoubleEvent& WXUNUSED(event))
  {
    int tmp = static_cast<int>(Nslide->GetValue());
    if (N == tmp) return;
    N = tmp;
    Nslide->SetValue(N);
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = GaussianBlur(w, h, data, N, sigma);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnNInput(wxCommandEvent& WXUNUSED(event))
  {
    Nslide->Enable(false);
    Nslide->Enable();
  }

  void OnSigmaSpinner(wxSpinDoubleEvent& WXUNUSED(event))
  {
    if (sigma == sigmaSlide->GetValue()) return;
    sigma = sigmaSlide->GetValue();
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = GaussianBlur(w, h, data, N, sigma);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnSigmaInput(wxCommandEvent& WXUNUSED(event))
  {
    sigmaSlide->Enable(false);
    sigmaSlide->Enable();
  }

  GaussianDialog(std::string s, wxImage* i, MyFrame* p)
    : Dialog(s)
  {
    N = 0;
    sigma = 0.0;
    Init(s, i, p);

    NText = new wxStaticText(this, wxID_ANY, "N kernel Size");
    Nslide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 14, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    Nslide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    Nslide->SetIncrement(1.0);
    vbox->Add(NText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(Nslide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    sigmaText = new wxStaticText(this, wxID_ANY, "Sigma");
    sigmaSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 15, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    sigmaSlide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    sigmaSlide->SetIncrement(1.0);
    vbox->Add(sigmaText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(sigmaSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = GaussianBlur(w, h, data, N, sigma);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }

    EndInit();
  }

  DECLARE_EVENT_TABLE()

};

BEGIN_EVENT_TABLE(GaussianDialog, wxDialog)
  EVT_TEXT_ENTER(wxID_HIGHEST + 14,     GaussianDialog::OnNInput)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 14, GaussianDialog::OnNSpinner)
  EVT_TEXT_ENTER(wxID_HIGHEST + 15,     GaussianDialog::OnSigmaInput)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 15, GaussianDialog::OnSigmaSpinner)
END_EVENT_TABLE()


struct GammaDialog : public Dialog
{
  double sigma;
  double value;
  wxSpinCtrlDouble* valueSlide;
  wxSpinCtrlDouble* sigmaSlide;
  wxStaticText* valueText;
  wxStaticText* sigmaText;

  void OnSigmaSpinner(wxSpinDoubleEvent& WXUNUSED(event))
  {
    if (sigma == sigmaSlide->GetValue()) return;
    sigma = sigmaSlide->GetValue();
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = GammaTransform(w, h, data, value, sigma);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnSigmaInput(wxCommandEvent& WXUNUSED(event))
  {
    sigmaSlide->Enable(false);
    sigmaSlide->Enable();
  }

  void OnValueSpinner(wxSpinDoubleEvent& WXUNUSED(event))
  {
    if (value == valueSlide->GetValue()) return;
    value = valueSlide->GetValue();
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = GammaTransform(w, h, data, value, sigma);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnValueInput(wxCommandEvent& WXUNUSED(event))
  {
    valueSlide->Enable(false);
    valueSlide->Enable();
  }

  GammaDialog(std::string s, wxImage* i, MyFrame* p)
    : Dialog(s)
  {
    sigma = value = 0.0;
    Init(s, i, p);

    valueText = new wxStaticText(this, wxID_ANY, "Constant");
    valueSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 11, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    valueSlide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    valueSlide->SetIncrement(0.01);
    vbox->Add(valueText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(valueSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    
    sigmaText = new wxStaticText(this, wxID_ANY, "Sigma");
    sigmaSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 12, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    sigmaSlide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    sigmaSlide->SetIncrement(0.01);
    vbox->Add(sigmaText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(sigmaSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = GammaTransform(w, h, data, value, sigma);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }

    EndInit();
  }

  DECLARE_EVENT_TABLE()

};

BEGIN_EVENT_TABLE(GammaDialog, wxDialog)
  EVT_TEXT_ENTER(wxID_HIGHEST + 11,     GammaDialog::OnValueInput)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 11, GammaDialog::OnValueSpinner)
  EVT_TEXT_ENTER(wxID_HIGHEST + 12,     GammaDialog::OnSigmaInput)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 12, GammaDialog::OnSigmaSpinner)
END_EVENT_TABLE()


struct LogDialog : public Dialog
{
  double base;
  double value;
  wxSpinCtrlDouble* slide;
  wxStaticText* valueText;
  wxSpinCtrlDouble* baseSlide;
  wxStaticText* baseText;

  void OnBaseSpinner(wxSpinDoubleEvent& WXUNUSED(event))
  {
    if (base == baseSlide->GetValue()) return;
    base = baseSlide->GetValue();
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = LogTransform(w, h, data, value, base);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnBaseInput(wxCommandEvent& WXUNUSED(event))
  {
    baseSlide->Enable(false);
    baseSlide->Enable();
  }

  void OnSpinner(wxSpinDoubleEvent& WXUNUSED(event))
  {
    if (value == slide->GetValue()) return;
    value = slide->GetValue();
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = LogTransform(w, h, data, value, base);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }
  void OnInput(wxCommandEvent& WXUNUSED(event))
  {
    slide->Enable(false);
    slide->Enable();
  }
  LogDialog(std::string s, wxImage* i, MyFrame* p)
    : Dialog(s)
  {
    value = 0.0;
    base = 0;
    Init(s, i, p);

    valueText = new wxStaticText(this, wxID_ANY, "Constant");
    slide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 10, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    slide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    slide->SetValue(0.0);
    slide->SetIncrement(1.0);
    vbox->Add(valueText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(slide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    
    baseText = new wxStaticText(this, wxID_ANY, "Base");
    baseSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 24, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    baseSlide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    baseSlide->SetValue(0.0);
    baseSlide->SetIncrement(1.0);
    vbox->Add(baseText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(baseSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    EndInit();

    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = LogTransform(w, h, data, value, base);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }

  }
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(LogDialog, wxDialog)
  EVT_TEXT_ENTER(wxID_HIGHEST + 10, LogDialog::OnInput)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 10, LogDialog::OnSpinner)
  EVT_TEXT_ENTER(wxID_HIGHEST + 24, LogDialog::OnBaseInput)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 24, LogDialog::OnBaseSpinner)
END_EVENT_TABLE()

struct ComponentConnectedDialog : public Dialog
{
  int min;
  int max;
  wxSpinCtrlDouble* minSlide;
  wxStaticText* minText;
  wxSpinCtrlDouble* maxSlide;
  wxStaticText* maxText;

  void OnMinSpinner(wxSpinDoubleEvent& WXUNUSED(event))
  {
    int tmp = static_cast<int>(minSlide->GetValue());
    if (min == tmp) return;
    min = tmp;
    minSlide->SetValue(min);
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = ConnectedComponentLabeling(w, h, data, min, max);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnMinInput(wxCommandEvent& WXUNUSED(event))
  {
    minSlide->Enable(false);
    minSlide->Enable();
  }

  void OnMaxSpinner(wxSpinDoubleEvent& WXUNUSED(event))
  {
    int tmp = static_cast<int>(maxSlide->GetValue());
    if (max == tmp) return;
    max = tmp;
    maxSlide->SetValue(max);
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = ConnectedComponentLabeling(w, h, data, min, max);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnMaxInput(wxCommandEvent& WXUNUSED(event))
  {
    maxSlide->Enable(false);
    maxSlide->Enable();
  }

  ComponentConnectedDialog(std::string s, wxImage* i, MyFrame* p)
    : Dialog(s)
  {
    min = 0;
    max = 0;
    Init(s, i, p);

    minText = new wxStaticText(this, wxID_ANY, "Minimum");
    minSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 25, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    minSlide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    minSlide->SetValue(0.0);
    minSlide->SetIncrement(1.0);
    vbox->Add(minText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(minSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    maxText = new wxStaticText(this, wxID_ANY, "Maximum");
    maxSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 26, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    maxSlide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    maxSlide->SetValue(0.0);
    maxSlide->SetIncrement(1.0);
    vbox->Add(maxText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(maxSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    EndInit();

    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = ConnectedComponentLabeling(w, h, data, min, max);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }

  }
  DECLARE_EVENT_TABLE()

};

  BEGIN_EVENT_TABLE(ComponentConnectedDialog, wxDialog)
    EVT_TEXT_ENTER(wxID_HIGHEST + 25, ComponentConnectedDialog::OnMinInput)
    EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 25, ComponentConnectedDialog::OnMinSpinner)
    EVT_TEXT_ENTER(wxID_HIGHEST + 26, ComponentConnectedDialog::OnMaxInput)
    EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 26, ComponentConnectedDialog::OnMaxSpinner)
  END_EVENT_TABLE()

struct PepperAndSaltDialog : Dialog
{
    double Pa;
    double Pb;
    wxSpinCtrlDouble* PaSlide;
    wxStaticText* PaText;
    wxSpinCtrlDouble* PbSlide;
    wxStaticText* PbText;

    void OnPbSpinner(wxSpinDoubleEvent& WXUNUSED(event))
    {
      double tmp = PbSlide->GetValue();
      if (Pb == tmp) return;
      Pb = tmp;
      PbSlide->SetValue(Pb);
      if (baseImage.IsOk())
      {
        int w = baseImage.GetWidth();
        int h = baseImage.GetHeight();
        unsigned char* data = baseImage.GetData();
        unsigned char* output = PepperSaltNoise(w, h, data, Pa, Pb);
        Output.Create(w, h, output);
        image->SetBitmap(Output);
      }
    }

    void OnPbInput(wxCommandEvent& WXUNUSED(event))
    {
      PbSlide->Enable(false);
      PbSlide->Enable();
    }
    
    void OnPaSpinner(wxSpinDoubleEvent& WXUNUSED(event))
    {
      double tmp = PaSlide->GetValue();
      if (Pa == tmp) return;
      Pa = tmp;
      PaSlide->SetValue(Pa);
      if (baseImage.IsOk())
      {
        int w = baseImage.GetWidth();
        int h = baseImage.GetHeight();
        unsigned char* data = baseImage.GetData();
        unsigned char* output = PepperSaltNoise(w, h, data, Pa, Pb);
        Output.Create(w, h, output);
        image->SetBitmap(Output);
      }
    }

    void OnPaInput(wxCommandEvent& WXUNUSED(event))
    {
      PaSlide->Enable(false);
      PaSlide->Enable();
    }

    PepperAndSaltDialog(std::string s, wxImage* i, MyFrame* p)
      : Dialog(s)
    {
      Pa = 0;
      Pb = 0;
      Init(s, i, p);

      PaText = new wxStaticText(this, wxID_ANY, "Pa");
      PaSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 33, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
      PaSlide->SetRange(0, 1);
      PaSlide->SetValue(0.0);
      PaSlide->SetIncrement(0.1);
      vbox->Add(PaText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
      vbox->Add(PaSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

      PbText = new wxStaticText(this, wxID_ANY, "Pb");
      PbSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 32, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
      PbSlide->SetRange(0, 1);
      PbSlide->SetValue(0.0);
      PbSlide->SetIncrement(0.1);
      vbox->Add(PbText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
      vbox->Add(PbSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

      EndInit();

      if (baseImage.IsOk())
      {
        int w = baseImage.GetWidth();
        int h = baseImage.GetHeight();
        unsigned char* data = baseImage.GetData();
        unsigned char* output = PepperSaltNoise(w, h, data, Pa, Pb);
        Output.Create(w, h, output);
        image->SetBitmap(Output);
      }

    }

    DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(PepperAndSaltDialog, wxDialog)
    EVT_TEXT_ENTER(wxID_HIGHEST + 32, PepperAndSaltDialog::OnPbInput)
    EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 32, PepperAndSaltDialog::OnPbSpinner)
    EVT_TEXT_ENTER(wxID_HIGHEST + 33, PepperAndSaltDialog::OnPaInput)
    EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 33, PepperAndSaltDialog::OnPaSpinner)
END_EVENT_TABLE()

struct LocalNoiseReductionDialog : Dialog
{
  double noiseVariance;
  wxSpinCtrlDouble* noiseVarianceSlide;
  wxStaticText* noiseVarianceText;

  void OnNoiseSlide(wxSpinDoubleEvent& WXUNUSED(event))
  {
    double tmp = noiseVarianceSlide->GetValue();
    if (noiseVariance == tmp) return;
    noiseVariance = tmp;
    noiseVarianceSlide->SetValue(noiseVariance);
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = LocalNoiseReduction(w, h, data, noiseVariance);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnNoiseInput(wxCommandEvent& WXUNUSED(event))
  {
    noiseVarianceSlide->Enable(false);
    noiseVarianceSlide->Enable();
  }

  LocalNoiseReductionDialog(std::string s, wxImage* i, MyFrame* p)
    : Dialog(s)
  {
    noiseVariance = 0;
    Init(s, i, p);

    noiseVarianceText = new wxStaticText(this, wxID_ANY, "Noise Variance");
    noiseVarianceSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 39, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    noiseVarianceSlide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    noiseVarianceSlide->SetValue(0.0);
    noiseVarianceSlide->SetIncrement(1.0);
    vbox->Add(noiseVarianceText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(noiseVarianceSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    EndInit();

    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = LocalNoiseReduction(w, h, data, noiseVariance);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }

  }
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(LocalNoiseReductionDialog, wxDialog)
  EVT_TEXT_ENTER(wxID_HIGHEST + 39, LocalNoiseReductionDialog::OnNoiseInput)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 39, LocalNoiseReductionDialog::OnNoiseSlide)
END_EVENT_TABLE()

struct AdaptiveMedianFilterDialog : Dialog
{
  int windowSize;
  wxSpinCtrlDouble* windowSlide;
  wxStaticText* windowText;

  void OnWindowSlide(wxSpinDoubleEvent& WXUNUSED(event))
  {
    int tmp = static_cast<int>(windowSlide->GetValue());
    if (windowSize == tmp) return;
    windowSize = tmp;
    windowSlide->SetValue(windowSize);
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = AdaptiveMedianFilter(w, h, data, windowSize);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnWindowInput(wxCommandEvent& WXUNUSED(event))
  {
    windowSlide->Enable(false);
    windowSlide->Enable();
  }

  AdaptiveMedianFilterDialog(std::string s, wxImage* i, MyFrame* p)
    : Dialog(s)
  {
    windowSize = 0;
    Init(s, i, p);

    windowText = new wxStaticText(this, wxID_ANY, "Window Size");
    windowSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 41, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    windowSlide->SetRange(0, std::numeric_limits<double>::max());
    windowSlide->SetValue(0.0);
    windowSlide->SetIncrement(1.0);
    vbox->Add(windowText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(windowSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    EndInit();

    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = AdaptiveMedianFilter(w, h, data, windowSize);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }

  }

  DECLARE_EVENT_TABLE()

};

BEGIN_EVENT_TABLE(AdaptiveMedianFilterDialog, wxDialog)
  EVT_TEXT_ENTER(wxID_HIGHEST + 41, AdaptiveMedianFilterDialog::OnWindowInput)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 41, AdaptiveMedianFilterDialog::OnWindowSlide)
END_EVENT_TABLE()


struct GaussianNoiseDialog : Dialog
{
  double mean;
  double variance;

  wxSpinCtrlDouble* meanSlide;
  wxStaticText* meanText;
  wxSpinCtrlDouble* varianceSlide;
  wxStaticText* varianceText;

  void OnMeanSlide(wxSpinDoubleEvent& WXUNUSED(event))
  {
    double tmp = meanSlide->GetValue();
    if (mean == tmp) return;
    mean = tmp;
    meanSlide->SetValue(mean);
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = GaussianNoise(w, h, data, mean, variance);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnMeanText(wxCommandEvent& WXUNUSED(event))
  {
    meanSlide->Enable(false);
    meanSlide->Enable();
  }

  void OnVarianceSlide(wxSpinDoubleEvent& WXUNUSED(event))
  {
    double tmp = varianceSlide->GetValue();
    if (variance == tmp) return;
    variance = tmp;
    varianceSlide->SetValue(variance);
    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = GaussianNoise(w, h, data, mean, variance);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }
  }

  void OnVarianceText(wxCommandEvent& WXUNUSED(event))
  {
    varianceSlide->Enable(false);
    varianceSlide->Enable();
  }

  GaussianNoiseDialog(std::string s, wxImage* i, MyFrame* p)
    : Dialog(s)
  {
    mean = 0;
    variance = 0;
    Init(s, i, p);

    meanText = new wxStaticText(this, wxID_ANY, "Mean");
    meanSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 43, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    meanSlide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    meanSlide->SetValue(0.0);
    meanSlide->SetIncrement(1.0);
    vbox->Add(meanText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(meanSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    varianceText = new wxStaticText(this, wxID_ANY, "Variance");
    varianceSlide = new wxSpinCtrlDouble(this, wxID_HIGHEST + 44, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    varianceSlide->SetRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    varianceSlide->SetValue(0.0);
    varianceSlide->SetIncrement(1.0);
    vbox->Add(varianceText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    vbox->Add(varianceSlide, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    EndInit();

    if (baseImage.IsOk())
    {
      int w = baseImage.GetWidth();
      int h = baseImage.GetHeight();
      unsigned char* data = baseImage.GetData();
      unsigned char* output = GaussianNoise(w, h, data, mean, variance);
      Output.Create(w, h, output);
      image->SetBitmap(Output);
    }

  }

  DECLARE_EVENT_TABLE()

};

BEGIN_EVENT_TABLE(GaussianNoiseDialog, wxDialog)
  EVT_TEXT_ENTER(wxID_HIGHEST + 43, GaussianNoiseDialog::OnMeanText)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 43, GaussianNoiseDialog::OnMeanSlide)
  EVT_TEXT_ENTER(wxID_HIGHEST + 44, GaussianNoiseDialog::OnVarianceText)
  EVT_SPINCTRLDOUBLE(wxID_HIGHEST + 44, GaussianNoiseDialog::OnVarianceSlide)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxImagePanel : public wxPanel
{
public:
  MyFrame* m_parent;
  wxImage image;
  wxBitmap resized;
  int w, h;

  wxImagePanel(wxFrame* parent, MyFrame* p);

  void paintEvent(wxPaintEvent & evt);
  void paintNow(bool b = false);
  void OnSize(wxSizeEvent& event);
  void render(wxDC& dc, bool b);

  unsigned char* NearestNeighbour(int nw, int nh);
  unsigned char* BilinearInterpolation(int nw, int nh);
  void           Negative();
  void           Im_Addition(std::string file1, std::string file2);
  void           Im_Subtraction(std::string file1, std::string file2);
  void           Im_Product(std::string file1, std::string file2);
  void           HistogramEqualization();

  DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)

EVT_PAINT(wxImagePanel::paintEvent)
EVT_SIZE(wxImagePanel::OnSize)
END_EVENT_TABLE()

class MyApp;

struct HistogramFrame : public wxFrame
{
  void OnQuit(wxCommandEvent& WXUNUSED(event))
  {
    Close(true);
  }
  HistogramFrame(const wxString& title = "Histogram")
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 300), wxCAPTION)
  {

  }
  wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(HistogramFrame, wxFrame)
  EVT_MENU(wxID_EXIT, HistogramFrame::OnQuit)
wxEND_EVENT_TABLE()

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
  // ctor(s)
  MyFrame(const wxString& title, MyApp* parent, bool F = false);
  ~MyFrame() { if(m_data) free(m_data); }
  // event handlers (these functions should _not_ be virtual)
  void OnQuit(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnLoad(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnSaveAs(wxCommandEvent& event);
  void OnBilinear(wxCommandEvent& event);
  void SaveFile(std::string);
  bool LoadFile(std::string);
  void OnEnter(wxCommandEvent& event);
  // Operations
  void SetClientSize(int w, int h);
  void OnNegativeOperation(wxCommandEvent& event);
  void OnAditionOperation(wxCommandEvent& event);
  void OnSubtractionOperation(wxCommandEvent& event);
  void OnProductOperation(wxCommandEvent& event);
  void OnLogOperation(wxCommandEvent& event);
  void OnPowerOperation(wxCommandEvent& event);
  void OnHistogramEqualization(wxCommandEvent& event);
  void OnGaussianBlur(wxCommandEvent& event);
  void OnSobel(wxCommandEvent& event);
  void OnUnsharp(wxCommandEvent& event);
  void OnConnectedComponent(wxCommandEvent& event);
  void OnShowHistogram(wxCommandEvent& event);
  void CalculateHistogram();
  void OnDirectFourier(wxCommandEvent& event);
  void OnSeperableFourier(wxCommandEvent& event);
  void OnFastFourierTransform(wxCommandEvent& event);
  void OnShowFourierSpecturm(wxCommandEvent& event);
  void OnShowReconstructImage(wxCommandEvent& event);
  void OnPepperAndSaltNoise(wxCommandEvent& event);
  void OnNoiseReduction0_5(wxCommandEvent& event);
  void OnNoiseReduction0_4(wxCommandEvent& event);
  void OnNoiseReduction0_01(wxCommandEvent& event);
  void OnMedianFilter(wxCommandEvent& event);
  void OnLocalNoiseReduction(wxCommandEvent& event);
  void OnAMF(wxCommandEvent& event);
  void OnGaussianNoise(wxCommandEvent& event);

  // any class wishing to process wxWidgets events must use this macro
  std::string          m_textDirectory;
  std::string          currDir;
  std::string          m_file;
  std::string          m_type;
  std::string          m_message;
  unsigned             m_width;
  unsigned             m_height;
  unsigned             m_maxClr;
  wxImage              m_image;
  MyApp*               m_parent;
  unsigned char*       m_data;
  bool                 m_bilinear;
  wxTextCtrl*          m_text;
  bool                 m_show;
  double               m_sigma;
  int                  m_N;
  bool                 m_bool;
  bool                 m_fourierShow;
  bool                 m_reconstructShow;
  wxDECLARE_EVENT_TABLE();
};

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
  wxBoxSizer* sizer;
  wxImagePanel * drawPane;
  wxImagePanel * FdrawPane;
  wxImagePanel * RdrawPane;
  MyFrame * frame;
  HistogramFrame* histogram;
  SimpleHistogram* histogram_data;
  MyFrame*   fourier;
  MyFrame*   reconstruct;

public:
  // override base class virtuals
  // ----------------------------

  // this one is called on application startup and is a good place for the app
  // initialization (doing it here and not in the ctor allows to have an error
  // return: if OnInit() returns false, the application terminates)
  virtual bool OnInit();
  wxImagePanel* GetImagePanel() { return drawPane; }
  wxBoxSizer* GetSizer() { return sizer; }
  MyFrame* GetFrame() { return frame; }
  HistogramFrame* GetHistogram() { return histogram; }
  SimpleHistogram* GetHistogramData() { return histogram_data; }
  MyFrame* GetFourier() { return fourier; }
  wxImagePanel* GetFourierImage() { return FdrawPane; }
  wxImagePanel* GetReconstructImage() { return RdrawPane; }
  MyFrame* GetReconstruct() { return reconstruct; }
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
  // menu items
  Minimal_Quit = wxID_EXIT,
  Minimal_Load = wxID_OPEN,
  Minimal_Save = wxID_SAVE,
  Minimal_SaveAs = wxID_SAVEAS,
  Minimal_Bilinear = wxID_HIGHEST + 1,
  Minimal_Enter = wxID_HIGHEST + 2,
  Minimal_Addition = wxID_HIGHEST + 3,
  Minimal_Subtraction = wxID_HIGHEST + 4,
  Minimal_Product = wxID_HIGHEST + 5,
  Minimal_Negative = wxID_HIGHEST + 6,
  Minimal_Log = wxID_HIGHEST + 7,
  Minimal_Power = wxID_HIGHEST + 8,
  Minimal_Histogram = wxID_HIGHEST + 9,
  Minimal_LogSpinner = wxID_HIGHEST + 10,
  Minimal_GammaSpinner = wxID_HIGHEST + 11,
  Minimal_GammaSigmaSpinner = wxID_HIGHEST + 12,
  Minimal_Gaussian = wxID_HIGHEST + 13,
  Minimal_GaussianN = wxID_HIGHEST + 14,
  Minimal_GaussianSigma = wxID_HIGHEST + 15,
  Minimal_Sobel = wxID_HIGHEST + 16,
  Minimal_Unsharp = wxID_HIGHEST + 17,
  Minimal_UnsharpK = wxID_HIGHEST + 18,
  Minimal_UnsharpN = wxID_HIGHEST + 19,
  Minimal_UnsharpSigma = wxID_HIGHEST + 20,
  Minimal_ConnectedComponent = wxID_HIGHEST + 21,
  Minimal_HistogramGraph = wxID_HIGHEST + 22,
  Minimal_ActivateHistogram = wxID_HIGHEST + 23,
  Minimal_LogBaseSpinner = wxID_HIGHEST + 24,
  Minimal_ComponentMinSpinner = wxID_HIGHEST + 25,
  Minimal_ComponentMaxSpinner = wxID_HIGHEST + 26,
  Minimal_DirectFourier = wxID_HIGHEST + 27,
  Minimal_SeperableFourier = wxID_HIGHEST + 28,
  Minimal_FastFourier = wxID_HIGHEST + 29,
  Minimal_ActivateFourier = wxID_HIGHEST + 30,
  Minimal_ActivateReconstruct = wxID_HIGHEST + 31,
  Minimal_PepperSaltNoisePb = wxID_HIGHEST + 32,
  Minimal_PepperSaltNoisePa = wxID_HIGHEST + 33,
  Minimal_PepperSaltNoise = wxID_HIGHEST + 34,
  Minimal_NoiseReduction0_5 = wxID_HIGHEST + 35,
  Minimal_NoiseReduction0_4 = wxID_HIGHEST + 36,
  Minimal_NoiseReduction0_01 = wxID_HIGHEST + 37,
  Minimal_MedianFilter = wxID_HIGHEST + 38,
  Minimal_LocalNoiseReductionVariance = wxID_HIGHEST + 39,
  Minimal_LocalNoiseReduction = wxID_HIGHEST + 40,
  Minimal_AMFwindow = wxID_HIGHEST + 41,
  Minimal_AMF = wxID_HIGHEST + 42,
  Minimal_GaussianNoiseMean = wxID_HIGHEST + 43,
  Minimal_GaussianNoiseVariance = wxID_HIGHEST + 44,
  Minimal_GaussianNoise = wxID_HIGHEST + 45

};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
  EVT_MENU(Minimal_Bilinear, MyFrame::OnBilinear)
  EVT_MENU(Minimal_Load, MyFrame::OnLoad)
  EVT_MENU(Minimal_Save, MyFrame::OnSave)
  EVT_MENU(Minimal_SaveAs,  MyFrame::OnSaveAs)
  EVT_MENU(Minimal_Negative,  MyFrame::OnNegativeOperation)
  EVT_MENU(Minimal_Addition, MyFrame::OnAditionOperation)
  EVT_MENU(Minimal_Subtraction, MyFrame::OnSubtractionOperation)
  EVT_MENU(Minimal_Product, MyFrame::OnProductOperation)
  EVT_MENU(Minimal_Log, MyFrame::OnLogOperation)
  EVT_MENU(Minimal_Power, MyFrame::OnPowerOperation)
  EVT_MENU(Minimal_Histogram,  MyFrame::OnHistogramEqualization)
  EVT_MENU(Minimal_Gaussian, MyFrame::OnGaussianBlur)
  EVT_MENU(Minimal_Sobel, MyFrame::OnSobel)
  EVT_MENU(Minimal_Unsharp, MyFrame::OnUnsharp)
  EVT_MENU(Minimal_ConnectedComponent, MyFrame::OnConnectedComponent)
  EVT_MENU(Minimal_ActivateHistogram, MyFrame::OnShowHistogram)
  EVT_MENU(Minimal_DirectFourier, MyFrame::OnDirectFourier)
  EVT_MENU(Minimal_SeperableFourier, MyFrame::OnSeperableFourier)
  EVT_MENU(Minimal_FastFourier, MyFrame::OnFastFourierTransform)
  EVT_MENU(Minimal_ActivateFourier, MyFrame::OnShowFourierSpecturm)
  EVT_MENU(Minimal_ActivateReconstruct, MyFrame::OnShowReconstructImage)
  EVT_MENU(Minimal_PepperSaltNoise, MyFrame::OnPepperAndSaltNoise)
  EVT_MENU(Minimal_NoiseReduction0_5, MyFrame::OnNoiseReduction0_5)
  EVT_MENU(Minimal_NoiseReduction0_4, MyFrame::OnNoiseReduction0_4)
  EVT_MENU(Minimal_NoiseReduction0_01, MyFrame::OnNoiseReduction0_01)
  EVT_MENU(Minimal_MedianFilter, MyFrame::OnMedianFilter)
  EVT_MENU(Minimal_LocalNoiseReduction, MyFrame::OnLocalNoiseReduction)
  EVT_MENU(Minimal_AMF, MyFrame::OnAMF)
  EVT_MENU(Minimal_GaussianNoise, MyFrame::OnGaussianNoise)
  EVT_TEXT_ENTER(Minimal_Enter, MyFrame::OnEnter)
  EVT_CLOSE(MyFrame::OnClose)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    wxInitAllImageHandlers();


    sizer = new wxBoxSizer(wxHORIZONTAL);
    // create the main application window
    frame = new MyFrame("CS370 Assignment", this);

    drawPane = new wxImagePanel(frame, frame);
    sizer->Add(drawPane, 1, wxEXPAND);


    histogram = new HistogramFrame();

    fourier = new MyFrame("Fourier Spectrum", this, true);
    FdrawPane = new wxImagePanel(fourier, fourier);
    fourier->Show(false);

    reconstruct = new MyFrame("Reconstructed Image", this, true);
    RdrawPane = new wxImagePanel(reconstruct, reconstruct);
    reconstruct->Show(false);

    histogram_data = new SimpleHistogram(histogram, Minimal_HistogramGraph);

    frame->SetSizer(sizer);
    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, MyApp* parent, bool F)
  : wxFrame(NULL, wxID_ANY, title), m_data(nullptr), m_bilinear(false), m_show(false),
	m_fourierShow(false), m_reconstructShow(false)
{
  m_bool = F;
  m_N = 3;
  m_sigma = 3.0;
  m_parent = parent;
  // set the frame icon
  SetIcon(wxICON(sample));
  if (!F)
  {
#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *OptMenu = new wxMenu;
    OptMenu->Append(Minimal_Bilinear, "&Bilinear Interpolation", "", wxITEM_CHECK);

    wxMenu *OperationMenu = new wxMenu;
    wxMenu *ImageOperationsMenu = new wxMenu;
    wxMenu *SmoothAndSharpeningMenu = new wxMenu;
    wxMenu *HistogramMenu = new wxMenu;
    wxMenu *FourierMenu = new wxMenu;
  	wxMenu *FourierWindowMenu = new wxMenu;
    wxMenu *NoiseGenerationMenu = new wxMenu;
    wxMenu *NoiseReductionMenu = new wxMenu;

    ImageOperationsMenu->Append(Minimal_Addition, "&Addition", "Addition Operation");
    ImageOperationsMenu->Append(Minimal_Subtraction, "&Subtraction", "Subtraction Operation");
    ImageOperationsMenu->Append(Minimal_Product, "&Product", "Product Operation");
    ImageOperationsMenu->Append(Minimal_Negative, "&Negative", "Negative Operation");
    ImageOperationsMenu->Append(Minimal_Log, "&Log Transform", "Log Transform");
    ImageOperationsMenu->Append(Minimal_Power, "&Power Transform", "Power(Gamma) Transform");
    OperationMenu->AppendSubMenu(ImageOperationsMenu, "Image Operations");

    OperationMenu->Append(Minimal_Histogram, "&Histogram Equalization", "Histogram Equalization");

    SmoothAndSharpeningMenu->Append(Minimal_Gaussian, "&Gaussian Blur", "Gaussian Blur");
    SmoothAndSharpeningMenu->Append(Minimal_Sobel, "&Sobel Filter", "Sobel Edge Detection");
    SmoothAndSharpeningMenu->Append(Minimal_Unsharp, "&Unsharp Filter", "Unsharp-masking operation");
    OperationMenu->AppendSubMenu(SmoothAndSharpeningMenu, "Smoothing And Sharpening Filter");

    OperationMenu->Append(Minimal_ConnectedComponent, "&Connected Component", "Connected Component Labeling");

    FourierMenu->Append(Minimal_DirectFourier, "&Direct Fourier", "Direct Fourier Transformation");
    FourierMenu->Append(Minimal_SeperableFourier, "&Seperable Fourier", "Seperable Fourier Transformation");
    FourierMenu->Append(Minimal_FastFourier, "&Fast Fourier", "Fast Fourier Transformation");

    OperationMenu->AppendSubMenu(FourierMenu, "Fourier Transformations");

    fileMenu->Append(Minimal_Save, "&Save\t", "Save a ppm image file");
    fileMenu->Append(Minimal_SaveAs, "&Save As\tCtrl-S", "Save as a ppm image file");
    fileMenu->Append(Minimal_Load, "&Load\tCtrl-O", "Load a ppm image file");
    fileMenu->Append(Minimal_Quit, "&Exit\tAlt-X", "Quit this program");

    HistogramMenu->Append(Minimal_ActivateHistogram, "&Image Histogram", "", wxITEM_CHECK);

	  FourierWindowMenu->Append(Minimal_ActivateFourier, "&Fourier Window", "", wxITEM_CHECK);
  	FourierWindowMenu->Append(Minimal_ActivateReconstruct, "&Reconstruct Window", "", wxITEM_CHECK);

    NoiseGenerationMenu->Append(Minimal_PepperSaltNoise, "&Pepper and Salt Noise", "Pepper and Salt Noise Generation");
    NoiseGenerationMenu->Append(Minimal_GaussianNoise, "&Gaussian Noise", "Gaussian Noise Generation");

    NoiseReductionMenu->Append(Minimal_NoiseReduction0_5, "Noise Reduction 0.5", "Noise Reduction Pa 0.5 Pb 0.5");
    NoiseReductionMenu->Append(Minimal_NoiseReduction0_4, "Noise Reduction 0.4/0.6", "Noise Reduction Pa 0.4 Pb 0.6");
    NoiseReductionMenu->Append(Minimal_NoiseReduction0_01, "Noise Reduction 0.01/0.9", "Noise Reduction Pa 0.01 Pb 0.9");
    NoiseReductionMenu->Append(Minimal_MedianFilter, "Median Filter", "Median Filter");
    NoiseReductionMenu->Append(Minimal_LocalNoiseReduction, "Local Noise Reduction", "Local Noise Reduction");
    NoiseReductionMenu->Append(Minimal_AMF, "Adaptive Median Filter", "Adaptive Median Filter");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(OptMenu, "&Options");
    menuBar->Append(OperationMenu, "&Operations");
    menuBar->Append(HistogramMenu, "&Show Histogram");
	  menuBar->Append(FourierWindowMenu, "&Fourier Windows");
    menuBar->Append(NoiseGenerationMenu, "&Noise Generation");
    menuBar->Append(NoiseReductionMenu, "&Noise Reduction/Removal");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    auto status = CreateStatusBar(2);
    m_text = new wxTextCtrl(status, Minimal_Enter, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    wxRect rect;
    status->GetFieldRect(0, rect);
    auto sz = m_text->GetSize();
    m_text->SetSize(rect.GetWidth(), sz.GetHeight());
    // m_text->SetSize(w, sz.GetHeight());
    // status->AddChild(m_text);
    SetStatusBarPane(1);
    SetStatusText("Welcome to CS370 Assignment App!", 1);
#endif // wxUSE_STATUSBAR
  }
  else
  {
    SetWindowStyle(wxCAPTION);
  }
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
  Close(true);
  if (!m_bool)
  {
    m_parent->GetHistogram()->Close(true);
    m_parent->GetFourier()->Close(true);
    m_parent->GetReconstruct()->Close(true);
  }
}

void MyFrame::OnClose(wxCloseEvent & WXUNUSED(event))
{
  if (!m_bool)
  {
    m_parent->GetFourier()->Close(true);
    m_parent->GetHistogram()->Close(true);
    m_parent->GetReconstruct()->Close(true);
  }
  Destroy();
}

void MyFrame::OnLoad(wxCommandEvent &  WXUNUSED(event))
{
  wxFileDialog openfileDialog(this, _("Open File"), "", "", "PPM Files (*.ppm)|*.ppm", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openfileDialog.ShowModal() == wxID_CANCEL) return;
  wxFileInputStream input(openfileDialog.GetPath());
  if (!input.IsOk())
  {
    wxLogError("Cannot open file '%s'.", openfileDialog.GetPath());
    return;
  }
  m_file = openfileDialog.GetPath();
  LoadFile(m_file);
  CalculateHistogram();
}

void MyFrame::OnSave(wxCommandEvent & WXUNUSED(event))
{
  if(m_file.empty()) wxLogMessage("No file is opened right now, cannot save");
  else SaveFile(m_file);
}

void MyFrame::OnSaveAs(wxCommandEvent & WXUNUSED(event))
{
  wxFileDialog savefileDialog(this, _("Save File"), "", "", "PPM Files (*.ppm)|*.ppm", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  if (savefileDialog.ShowModal() == wxID_CANCEL) return;
  wxFileOutputStream input(savefileDialog.GetPath());
  if (!input.IsOk())
  {
    wxLogError("Cannot save file '%s'.", savefileDialog.GetPath());
    return;
  }
  std::string filename = savefileDialog.GetPath();
  SaveFile(filename);
}

void MyFrame::OnBilinear(wxCommandEvent & WXUNUSED(event))
{
  m_bilinear = !m_bilinear;
  m_parent->GetImagePanel()->paintNow(true);
}

void MyFrame::SaveFile(std::string filename)
{
  std::ofstream outfile(filename);
  std::string data;
  wxImagePanel* imagePanel = m_parent->GetImagePanel();
  if (!imagePanel->image.IsOk())
  {
    outfile.close();
    return;
  }
  int w = imagePanel->w > 0 ? imagePanel->w : 0;
  int h = imagePanel->h > 0 ? imagePanel->h : 0;

  data += m_type;
  data += "\n";
  data += m_message;
  data += "\n";
  data += std::to_string(w);
  data += " ";
  data += std::to_string(h);
  data += "\n";
  data += std::to_string(m_maxClr);
  data += "\n";
  unsigned char* mdata = imagePanel->image.GetData();
  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      data += std::to_string(static_cast<unsigned char>((mdata[(i * w + j) * 3] / 255.f) * (m_maxClr)));
      data += " ";
      data += std::to_string(static_cast<unsigned char>(((mdata[(i * w + j) * 3 + 1]) / 255.f) * (m_maxClr)));
      data += " ";
      data += std::to_string(static_cast<unsigned char>(((mdata[(i * w + j) * 3 + 2]) / 255.f) * (m_maxClr)));
      if (j != w) data += " ";
    }
    data += "\n";
  }
  outfile << data;
  outfile.close();
}

bool MyFrame::LoadFile(std::string filename)
{
  std::ifstream ifs;
  ifs.open(filename.c_str(), std::ifstream::in);
  std::string str((std::istreambuf_iterator<char>(ifs)),
                   std::istreambuf_iterator<char>());
  if (ifs.fail())
  {
    ifs.close();
    return false;
  }
  if (str.empty())
  {
    if (m_data) free(m_data);
    m_data = nullptr;
    wxImagePanel* imagePanel = m_parent->GetImagePanel();
    imagePanel->image = wxImage{};
    imagePanel->resized = wxBitmap{};
    imagePanel->paintNow();
    return true;
  }
  if (m_data) free(m_data);
  auto type = str.substr(0, str.find_first_of('\n'));
  str.erase(0, str.find_first_of('\n') + 1);
  auto mess = str.substr(0, str.find_first_of('\n'));
  str.erase(0, str.find_first_of('\n') + 1);
  auto width = str.substr(0, str.find_first_of(' '));
  while (width[0] < 48 || width[0] > 57) width.erase(width.begin());
  str.erase(0, str.find_first_of(' ') + 1);
  auto height = str.substr(0, str.find_first_of('\n'));
  str.erase(0, str.find_first_of('\n') + 1);
  auto max = str.substr(0, str.find_first_of('\n'));
  str.erase(0, str.find_first_of('\n') + 1);
  m_type = type;
  m_message = mess;
  m_width = std::stoi(width);
  m_height = std::stoi(height);
  m_maxClr = std::stoi(max);
  ifs.close();
  m_data = (unsigned char*)malloc(m_width * m_height * 3);
  unsigned char* tmp = (unsigned char*)malloc(m_width * m_height * 3);

  char* token;

  token = strtok(const_cast<char*>(str.c_str()), "\n ");
  unsigned i = 0;

  while (token != NULL) {
    auto clr = atoi(token);
    tmp[i] = m_data[i] = static_cast<unsigned char>((clr / static_cast<float>(m_maxClr)) * 255);
    ++i;
    token = strtok(NULL, "\n ");
  }
  wxImagePanel* imagePanel = m_parent->GetImagePanel();
  wxWindow::SetClientSize(m_width, m_height);
  imagePanel->image.Create(m_width, m_height, tmp);
  imagePanel->w = m_width;
  imagePanel->h = m_height;
  auto status = GetStatusBar();
  if (status)
  {
    wxRect rect;
    GetStatusBar()->GetFieldRect(0, rect);
    auto sz = m_text->GetSize();
    m_text->SetSize(rect.GetWidth(), sz.GetHeight());
  }
  imagePanel->resized = wxBitmap(imagePanel->image);
  imagePanel->paintNow();
  return true;
}

void MyFrame::OnEnter(wxCommandEvent& WXUNUSED(event))
{
	auto text = m_text->GetLineText(1);
  std::string s = text.c_str();
  std::string myOutput;

  if (CommandLineInput(s, m_textDirectory, myOutput))
  {
    m_text->Clear();
    if (!myOutput.empty())
    {
      LoadFile(myOutput);
    }
    return;
  }

  if (s.size() == 2 && s[0] == 'l' && s[1] == 's')
  {
    std::string mess = "Current Directory : ";
    mess += m_textDirectory;
    wxLogMessage(mess.c_str());
  }

  if (s.size() >= 3)
  {
    if (s[0] == 'c' && s[1] == 'd' && s[2] == ' ')
    {
      std::string directory = s.substr(3);
      m_textDirectory = directory;
      // wxLogMessage(m_textDirectory.c_str());
    }
  }
  if (s.size() >= 5)
  {
    if (s[0] == 's' && s[1] == 'a' && s[2] == 'v' && s[3] == 'e' && s[4] == ' ')
    {
      std::string filename = s.substr(5);
      size_t lastPos = filename.size() - 1;
      if (filename[lastPos - 3] != '.' || filename[lastPos - 2] != 'p' || filename[lastPos - 1] != 'p' || filename[lastPos] != 'm')
      {
        std::string errorMessage = "Invalid Save file extension, e.g. \" save <filename>.ppm\"";
        wxLogMessage(errorMessage.c_str());
      }
      else
      {
        if(m_textDirectory.empty() || m_textDirectory[m_textDirectory.size() - 1] == '\\' || m_textDirectory[m_textDirectory.size() - 1] == '/')
        {
          // wxLogMessage((m_textDirectory + filename).c_str());
          SaveFile(m_textDirectory + filename);
        }
        else
        {
          // wxLogMessage((m_textDirectory + '\\' + filename).c_str());
          SaveFile(m_textDirectory + '\\' + filename);
        }

      }
    }
    else if (s[0] == 'l' && s[1] == 'o' && s[2] == 'a' && s[3] == 'd' && s[4] == ' ')
    {
      std::string filename = s.substr(5);
      size_t lastPos = filename.size() - 1;
      if (filename[lastPos - 3] != '.' || filename[lastPos - 2] != 'p' || filename[lastPos - 1] != 'p' || filename[lastPos] != 'm')
      {
        std::string errorMessage = "Invalid Load file extension, e.g. \" load <filename>.ppm\"";
        wxLogMessage(errorMessage.c_str());
      }
      else
      {
        if (m_textDirectory.empty() || m_textDirectory[m_textDirectory.size() - 1] == '\\' || m_textDirectory[m_textDirectory.size() - 1] == '/')
        {
          // wxLogMessage((m_textDirectory + filename).c_str());
          bool b = LoadFile(m_textDirectory + filename);
          if (b)
          {
            m_file = m_textDirectory + filename;
            CalculateHistogram();
          }
          else
          {
            std::string errorMessage = m_textDirectory + filename + " file does not exist";
            wxLogMessage(errorMessage.c_str());
          }
        }
        else
        {
          bool b = LoadFile(m_textDirectory + '\\' + filename);
          if (b)
          {
            m_file = m_textDirectory + '\\' + filename;
            CalculateHistogram();
          }
          else
          {
            std::string errorMessage = m_textDirectory + '\\' + filename + " file does not exist";
            wxLogMessage(errorMessage.c_str());
          }
        }
      }
    }

  }
  m_text->Clear();
	// wxLogMessage(text);
}

void MyFrame::SetClientSize(int w, int h)
{
  wxWindow::SetClientSize(w, h);
}

void MyFrame::OnNegativeOperation(wxCommandEvent & WXUNUSED(event))
{
  m_parent->GetImagePanel()->Negative();
}

void MyFrame::OnAditionOperation(wxCommandEvent & WXUNUSED(event))
{
  std::string file1;
  std::string file2;
  wxFileDialog openfile1Dialog(this, _("Open File 1 for addition"), "", "", "PPM Files (*.ppm)|*.ppm", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openfile1Dialog.ShowModal() == wxID_CANCEL) return;
  wxFileInputStream input1(openfile1Dialog.GetPath());
  if (!input1.IsOk())
  {
    wxLogError("Cannot open file '%s'.", openfile1Dialog.GetPath());
    return;
  }

  file1 = openfile1Dialog.GetPath();

  wxFileDialog openfile2Dialog(this, _("Open File 2 for addition"), "", "", "PPM Files (*.ppm)|*.ppm", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openfile2Dialog.ShowModal() == wxID_CANCEL) return;
  wxFileInputStream input2(openfile2Dialog.GetPath());
  if (!input2.IsOk())
  {
    wxLogError("Cannot open file '%s'.", openfile2Dialog.GetPath());
    return;
  }

  file2 = openfile2Dialog.GetPath();

  m_parent->GetImagePanel()->Im_Addition(file1, file2);
}

void MyFrame::OnSubtractionOperation(wxCommandEvent & WXUNUSED(event))
{
  std::string file1;
  std::string file2;
  wxFileDialog openfile1Dialog(this, _("Open File 1 for subtraction"), "", "", "PPM Files (*.ppm)|*.ppm", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openfile1Dialog.ShowModal() == wxID_CANCEL) return;
  wxFileInputStream input1(openfile1Dialog.GetPath());
  if (!input1.IsOk())
  {
    wxLogError("Cannot open file '%s'.", openfile1Dialog.GetPath());
    return;
  }

  file1 = openfile1Dialog.GetPath();

  wxFileDialog openfile2Dialog(this, _("Open File 2 for subtraction"), "", "", "PPM Files (*.ppm)|*.ppm", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openfile2Dialog.ShowModal() == wxID_CANCEL) return;
  wxFileInputStream input2(openfile2Dialog.GetPath());
  if (!input2.IsOk())
  {
    wxLogError("Cannot open file '%s'.", openfile2Dialog.GetPath());
    return;
  }

  file2 = openfile2Dialog.GetPath();

  m_parent->GetImagePanel()->Im_Subtraction(file1, file2);
}

void MyFrame::OnProductOperation(wxCommandEvent & WXUNUSED(event))
{
  std::string file1;
  std::string file2;
  wxFileDialog openfile1Dialog(this, _("Open File 1 for Product"), "", "", "PPM Files (*.ppm)|*.ppm", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openfile1Dialog.ShowModal() == wxID_CANCEL) return;
  wxFileInputStream input1(openfile1Dialog.GetPath());
  if (!input1.IsOk())
  {
    wxLogError("Cannot open file '%s'.", openfile1Dialog.GetPath());
    return;
  }

  file1 = openfile1Dialog.GetPath();

  wxFileDialog openfile2Dialog(this, _("Open File 2 for Mask"), "", "", "PPM Files (*.ppm)|*.ppm", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openfile2Dialog.ShowModal() == wxID_CANCEL) return;
  wxFileInputStream input2(openfile2Dialog.GetPath());
  if (!input2.IsOk())
  {
    wxLogError("Cannot open file '%s'.", openfile2Dialog.GetPath());
    return;
  }

  file2 = openfile2Dialog.GetPath();

  m_parent->GetImagePanel()->Im_Product(file1, file2);
}

void MyFrame::OnLogOperation(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  LogDialog a("Log Transformation", i, this);
  int result = a.ShowModal();
  if (result == wxID_OK)
  {
    double c = a.slide->GetValue();
    double base = a.baseSlide->GetValue();
    unsigned char* output = LogTransform(m_width, m_height, m_data, c, base);
    free(m_data);
    m_data = output;
    
    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);
    
    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnPowerOperation(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  GammaDialog a("Gamma Transformation", i, this);
  int result = a.ShowModal();
  if (result == wxID_OK)
  {
    double c = a.valueSlide->GetValue();
    double sigma = a.sigmaSlide->GetValue();
    unsigned char* output = GammaTransform(m_width, m_height, m_data, c, sigma);
    free(m_data);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnHistogramEqualization(wxCommandEvent & WXUNUSED(event))
{
  m_parent->GetImagePanel()->HistogramEqualization();
  CalculateHistogram();
}

void MyFrame::OnGaussianBlur(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  GaussianDialog a("Gaussian Blur", i, this);
  int result = a.ShowModal();
  if (result == wxID_OK)
  {
    int N = static_cast<int>(a.Nslide->GetValue());
    double sigma = a.sigmaSlide->GetValue();
    unsigned char* output = GaussianBlur(m_width, m_height, m_data, N, sigma);
    m_N = N;
    m_sigma = sigma;
    free(m_data);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnSobel(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  if (!i->IsOk()) return;
  unsigned char* output = EdgeDetetectionSobel(m_width, m_height, m_data);
  free(m_data);
  m_data = output;

  unsigned char* data = nullptr;

  int m_w = i->GetWidth();
  int m_h = i->GetHeight();

  if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
  else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

  i->Create(m_w, m_h, data);

  m_parent->GetImagePanel()->resized = wxBitmap(*i);
  m_parent->GetImagePanel()->paintNow();
  CalculateHistogram();
}

void MyFrame::OnUnsharp(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  UnsharpDialog a("Unsharp Mask Operation", i, this, m_N, m_sigma);
  int result = a.ShowModal();
  if (result == wxID_OK)
  {
    int N = a.N;
    double sigma = a.sigma;
    double K = a.Kslide->GetValue();
    unsigned char* output = UnsharpMaskOperation(m_width, m_height, m_data, N, sigma, K);
    free(m_data);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnConnectedComponent(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  ComponentConnectedDialog a("Connected Component Operation", i, this);
  int result = a.ShowModal();

  if (result == wxID_OK)
  {
    int min = a.min;
    int max = a.max;
    unsigned char* output = ConnectedComponentLabeling(m_width, m_height, m_data, min, max);
    free(m_data);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

  wxImagePanel::wxImagePanel(wxFrame* parent, MyFrame* p) :
  wxPanel(parent), m_parent(p)
{
  w = -1;
  h = -1;
}

void wxImagePanel::paintEvent(wxPaintEvent & WXUNUSED(event))
{
  wxPaintDC dc(this);
  render(dc, false);
}

void wxImagePanel::paintNow(bool b)
{
  wxClientDC dc(this);
  render(dc, b);
}

void wxImagePanel::render(wxDC&  dc, bool b)
{
	int neww, newh;
	dc.GetSize(&neww, &newh);
	if (neww != w || newh != h)
	{
		auto status = m_parent->GetStatusBar();
		if (status)
		{
			wxRect rect;
			m_parent->GetStatusBar()->GetFieldRect(0, rect);
			auto sz = m_parent->m_text->GetSize();
			m_parent->m_text->SetSize(rect.GetWidth(), sz.GetHeight());
		}
	}
  if (!image.IsOk())
  {
    dc.Clear();
    return;
  }


  if (neww != w || newh != h || b)
  {
    if (!m_parent->m_bilinear) // Nearest Neighbour
    {
      unsigned char* data = NearestNeighbour(neww, newh);
      image.Create(neww, newh, data);
      resized = wxBitmap(image);
      w = neww;
      h = newh;
      dc.DrawBitmap(resized, 0, 0, false);
      m_parent->CalculateHistogram();
    }
    else
    {
      unsigned char* data = BilinearInterpolation(neww, newh);
      image.Create(neww, newh, data);
      resized = wxBitmap(image);
      w = neww;
      h = newh;
      dc.DrawBitmap(resized, 0, 0, false);
      m_parent->CalculateHistogram();
    }
  }
  else {
    dc.DrawBitmap(resized, 0, 0, false);
  }
}

unsigned char * wxImagePanel::NearestNeighbour(int nw, int nh)
{
  unsigned char* data = (unsigned char*) malloc(nw * nh * 3);
  int ow = m_parent->m_width;
  int oh = m_parent->m_height;
  unsigned char* odata = m_parent->m_data;
  float sw = (float)ow / (float)nw;
  float sh = (float)oh / (float)nh;
  for (int i = 0; i < nh; ++i)
  {
    int floor_i = (int)(i * sh);
    for (int j = 0; j < nw; ++j)
    {
      int floor_j = (int)(j * sw);
      unsigned pixel = ((i * nw) + j) * 3;
      unsigned o_pixel = ((floor_i * ow) + floor_j) * 3;
      data[pixel] = odata[o_pixel];
      data[pixel + 1] = odata[o_pixel + 1];
      data[pixel + 2] = odata[o_pixel + 2];
    }
  }
  return data;
}

unsigned char * wxImagePanel::BilinearInterpolation(int nw, int nh)
{
  struct clr
  {
    unsigned char r, g, b;
  };

  unsigned char* data = (unsigned char*)malloc(nw * nh * 3);
  int ow = m_parent->m_width;
  int oh = m_parent->m_height;
  unsigned char* odata = m_parent->m_data;
  float sw = (float)ow / (float)nw;
  float sh = (float)oh / (float)nh;
  for (int i = 0; i < nh; ++i)
  {
    float origin_i = i * sh;
    float floor_i = floor(origin_i);
    float ceil_i = floor_i + 1.f;

    for (int j = 0; j < nw; ++j)
    {
      float origin_j = j * sw;
      float floor_j = floor(origin_j);
      float ceil_j = floor_j + 1.f;
      if (ceil_j == ow || ceil_i == oh)
      {
        unsigned pixel = ((i * nw) + j) * 3;
        unsigned o_pixel = ((floor_i * ow) + floor_j) * 3;
        if (ceil_j == ow && ceil_i == oh)
        {
          data[pixel] = odata[o_pixel];
          data[pixel + 1] = odata[o_pixel + 1];
          data[pixel + 2] = odata[o_pixel + 2];
        }
        else if (ceil_j == ow)
        {
          unsigned t_pixel = (((unsigned)floor_i * ow) + (unsigned)floor_j) * 3;
          unsigned b_pixel = (((unsigned)ceil_i * ow) + (unsigned)floor_j) * 3;
          float y1 = origin_i - floor_i;
          float y2 = ceil_i - origin_i;
          data[pixel] = (y2 * odata[t_pixel]) + (y1 * odata[b_pixel]);
          data[pixel + 1] = (y2 * odata[t_pixel + 1]) + (y1 * odata[b_pixel + 1]);
          data[pixel + 2] = (y2 * odata[t_pixel + 2]) + (y1 * odata[b_pixel + 2]);

        }
        else // ceil_i = oh
        {
          unsigned l_pixel = (((unsigned)floor_i * ow) + (unsigned)floor_j) * 3;
          unsigned r_pixel = (((unsigned)floor_i * ow) + (unsigned)ceil_j) * 3;
          float x1 = origin_j - floor_j;
          float x2 = ceil_j - origin_j;
          data[pixel] = (x2 * odata[l_pixel]) + (x1 * odata[r_pixel]);
          data[pixel + 1] = (x2 * odata[l_pixel + 1]) + (x1 * odata[r_pixel + 1]);
          data[pixel + 2] = (x2 * odata[l_pixel + 2]) + (x1 * odata[r_pixel + 2]);
        }
      }
      else
      {
        unsigned pixel = ((i * nw) + j) * 3;
        unsigned tl_pixel = (((unsigned)floor_i * ow) + (unsigned)floor_j) * 3;
        unsigned bl_pixel = (((unsigned)ceil_i * ow) + (unsigned)floor_j) * 3;
        unsigned tr_pixel = (((unsigned)floor_i * ow) + (unsigned)ceil_j) * 3;
        unsigned br_pixel = (((unsigned)ceil_i * ow) + (unsigned)ceil_j) * 3;
      
        // left side
        float y1 = origin_i - floor_i;
        float y2 = ceil_i - origin_i;
        clr left{};
        left.r = (y2 * odata[tl_pixel]) + (y1 * odata[bl_pixel]);
        left.g = (y2 * odata[tl_pixel + 1]) + (y1 * odata[bl_pixel + 1]);
        left.b = (y2 * odata[tl_pixel + 2]) + (y1 * odata[bl_pixel + 2]);
      
        // right side
        clr right{};
        right.r = (y2 * odata[tr_pixel]) + (y1 * odata[br_pixel]);
        right.g = (y2 * odata[tr_pixel + 1]) + (y1 * odata[br_pixel + 1]);
        right.b = (y2 * odata[tr_pixel + 2]) + (y1 * odata[br_pixel + 2]);
      
        // interpolate
        float x1 = origin_j - floor_j;
        float x2 = ceil_j - origin_j;
      
        data[pixel] = (x2 * left.r) + (x1 * right.r);
        data[pixel + 1] = (x2 * left.g) + (x1 * right.g);
        data[pixel + 2] = (x2 * left.b) + (x1 * right.b);
      }
    }
  }
  return data;
}

void wxImagePanel::Negative()
{
  if (w == -1 || h == -1) return;
  int nw = m_parent->m_width;
  int nh = m_parent->m_height;
  unsigned char* odata = m_parent->m_data;
  Inverse(nw, nh, odata);
  
  int m_w = image.GetWidth();
  int m_h = image.GetHeight();
  unsigned char* data = nullptr;
  // unsigned char* ddata = image.GetData();
  // for (int i = 0; i < m_h; ++i)
  // {
  //   for (int j = 0; j < m_w; ++j)
  //   {
  //     unsigned pixel = ((i * m_w) + j) * 3;
  //     data[pixel] = 255 - ddata[pixel];
  //     data[pixel + 1] = 255 - ddata[pixel + 1];
  //     data[pixel + 2] = 255 - ddata[pixel + 2];
  //   }
  // }

  if (m_parent->m_bilinear) data = BilinearInterpolation(m_w, m_h);
  else data = NearestNeighbour(m_w, m_h);

  image.Create(m_w, m_h, data);
  resized = wxBitmap(image);
  paintNow();
  m_parent->CalculateHistogram();
}

void wxImagePanel::Im_Addition(std::string file1, std::string file2)
{
  m_parent->m_file.clear();
  auto result1 = LoadHelper(file1);
  auto result2 = LoadHelper(file2);
  unsigned char* data1 = result1.first;
  unsigned char* data2 = result2.first;
  Package p1 = result1.second;
  Package p2 = result2.second;
  if (!data1 || !data2)
  {
    if (data1) free(data1);
    if (data2) free(data2);
    return;
  }
  unsigned size1 = p1.height * p1.width;
  unsigned size2 = p1.height * p2.width;
  if (size1 > size2)
  {
    data2 = Resize(p1.width, p1.height, data2, p2);
    p2.width = p1.width;
    p2.height = p1.height;
  }
  else if (size2 > size1)
  {
    data1 = Resize(p2.width, p2.height, data1, p1);
    p1.width = p2.width;
    p1.height = p2.height;
  }

  unsigned char* tmp = static_cast<unsigned char*>(malloc(p1.width * p1.height * 3));
  unsigned char* output = Addition(p1.width, p1.height, data1, data2, tmp);

  free(data1); free(data2);
  
  if (m_parent->m_data) free(m_parent->m_data);
  m_parent->m_data = output;
  m_parent->m_type = p1.type;
  m_parent->m_message = p1.message;
  m_parent->m_width = p1.width;
  m_parent->m_height = p1.height;
  m_parent->m_maxClr = 255;

  m_parent->SetClientSize(p1.width, p1.height);
  image.Create(p1.width, p1.height, tmp);
  w = p1.width;
  h = p1.height;
  auto status = m_parent->GetStatusBar();
  if (status)
  {
    wxRect rect;
    m_parent->GetStatusBar()->GetFieldRect(0, rect);
    auto sz = m_parent->m_text->GetSize();
    m_parent->m_text->SetSize(rect.GetWidth(), sz.GetHeight());
  }
 resized = wxBitmap(image);
 paintNow();
 m_parent->CalculateHistogram();
}

void wxImagePanel::Im_Subtraction(std::string file1, std::string file2)
{
  m_parent->m_file.clear();
  auto result1 = LoadHelper(file1);
  auto result2 = LoadHelper(file2);
  unsigned char* data1 = result1.first;
  unsigned char* data2 = result2.first;
  Package p1 = result1.second;
  Package p2 = result2.second;
  if (!data1 || !data2)
  {
    if (data1) free(data1);
    if (data2) free(data2);
    return;
  }
  unsigned size1 = p1.height * p1.width;
  unsigned size2 = p1.height * p2.width;
  if (size1 > size2)
  {
    data2 = Resize(p1.width, p1.height, data2, p2);
    p2.width = p1.width;
    p2.height = p2.height;
  }
  else if (size2 > size1)
  {
    data1 = Resize(p2.width, p2.height, data1, p1);
    p1.width = p2.width;
    p1.height = p2.height;
  }

  unsigned char* tmp = static_cast<unsigned char*>(malloc(p1.width * p1.height * 3));
  unsigned char* output = Subtraction(p1.width, p1.height, data1, data2, tmp);

  free(data1); free(data2);
  
  if (m_parent->m_data) free(m_parent->m_data);
  m_parent->m_data = output;
  m_parent->m_type = p1.type;
  m_parent->m_message = p1.message;
  m_parent->m_width = p1.width;
  m_parent->m_height = p1.height;
  m_parent->m_maxClr = 255;

  m_parent->SetClientSize(p1.width, p1.height);
  image.Create(p1.width, p1.height, tmp);
  w = p1.width;
  h = p1.height;
  auto status = m_parent->GetStatusBar();
  if (status)
  {
    wxRect rect;
    m_parent->GetStatusBar()->GetFieldRect(0, rect);
    auto sz = m_parent->m_text->GetSize();
    m_parent->m_text->SetSize(rect.GetWidth(), sz.GetHeight());
  }
 resized = wxBitmap(image);
 paintNow();
 m_parent->CalculateHistogram();
}

void wxImagePanel::Im_Product(std::string file1, std::string file2)
{
  m_parent->m_file.clear();
  auto result1 = LoadHelper(file1);
  auto result2 = LoadHelper(file2);
  unsigned char* data1 = result1.first;
  unsigned char* data2 = result2.first;
  Package p1 = result1.second;
  Package p2 = result2.second;
  if (!data1 || !data2)
  {
    if (data1) free(data1);
    if (data2) free(data2);
    return;
  }
  unsigned size1 = p1.height * p1.width;
  unsigned size2 = p1.height * p2.width;
  if (size1 > size2)
  {
    data2 = Resize(p1.width, p1.height, data2, p2);
    p2.width = p1.width;
    p2.height = p2.height;
  }
  else if (size2 > size1)
  {
    data1 = Resize(p2.width, p2.height, data1, p1);
    p1.width = p2.width;
    p1.height = p2.height;
  }

  unsigned char* tmp = static_cast<unsigned char*>(malloc(p1.width * p1.height * 3));
  unsigned char* output = Product(p1.width, p1.height, data1, data2, tmp);

  free(data1); free(data2);

  if (m_parent->m_data) free(m_parent->m_data);
  m_parent->m_data = output;
  m_parent->m_type = p1.type;
  m_parent->m_message = p1.message;
  m_parent->m_width = p1.width;
  m_parent->m_height = p1.height;
  m_parent->m_maxClr = 255;

  m_parent->SetClientSize(p1.width, p1.height);
  image.Create(p1.width, p1.height, tmp);
  w = p1.width;
  h = p1.height;
  auto status = m_parent->GetStatusBar();
  if (status)
  {
    wxRect rect;
    m_parent->GetStatusBar()->GetFieldRect(0, rect);
    auto sz = m_parent->m_text->GetSize();
    m_parent->m_text->SetSize(rect.GetWidth(), sz.GetHeight());
  }
  resized = wxBitmap(image);
  paintNow();
  m_parent->CalculateHistogram();
}

void wxImagePanel::HistogramEqualization()
{
  if (w == -1 || h == -1) return;
  unsigned char* odata = m_parent->m_data;
  unsigned int c = m_parent->m_maxClr;
  unsigned int nw = m_parent->m_width;
  unsigned int nh = m_parent->m_height;
  H_Equalization(nw, nh, odata, c);

  int m_w = image.GetWidth();
  int m_h = image.GetHeight();
  unsigned char* data = nullptr;

  if (m_parent->m_bilinear) data = BilinearInterpolation(m_w, m_h);
  else data = NearestNeighbour(m_w, m_h);

  image.Create(m_w, m_h, data);
  resized = wxBitmap(image);
  paintNow();
  m_parent->CalculateHistogram();
}

void wxImagePanel::OnSize(wxSizeEvent& event) {
  Refresh();
  event.Skip();
}

void MyFrame::OnShowHistogram(wxCommandEvent & WXUNUSED(event))
{
  m_show = !m_show;
  CalculateHistogram();
  m_parent->GetHistogram()->Show(m_show);
}

void MyFrame::CalculateHistogram()
{
  if (!m_show) return;
  auto iPanel = m_parent->GetImagePanel();
  auto& im = iPanel->image;
  if (!im.IsOk())
  {
    m_parent->GetHistogramData()->SetData(std::vector<double>{});
    return;
  }
  std::vector<double> histo_data;
  histo_data.resize(m_maxClr + 1);
  unsigned char* data = im.GetData();
  int w = im.GetWidth();
  int h = im.GetHeight();

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      unsigned pixel = ((i * w) + j) * 3;
      data[pixel]     = static_cast<unsigned char>((data[pixel] / 255.f    ) * m_maxClr);
      ++histo_data[data[pixel]];
      data[pixel + 1] = static_cast<unsigned char>((data[pixel + 1] / 255.f) * m_maxClr);
      ++histo_data[data[pixel + 1]];
      data[pixel + 2] = static_cast<unsigned char>((data[pixel + 2] / 255.f) * m_maxClr);
      ++histo_data[data[pixel + 2]];
    }
  }

  m_parent->GetHistogramData()->SetData(histo_data);
  m_parent->GetHistogramData()->PaintNow();
}

void MyFrame::OnDirectFourier(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  if (!i->IsOk()) return;
  ImaginaryNum* out = DirectFourierTransform(m_width, m_height, m_data);
  ImaginaryNum* dout = InverseFourierTransform(m_width, m_height, out);
  unsigned char* output = LogImaginaryToChar(m_width, m_height, out);
  unsigned char* routput = ImaginaryToChar(m_width, m_height, dout);

  // free(m_data);
  unsigned char* tmp = m_data;
  free(dout);
  free(out);

  m_data = output;

  unsigned char* data = nullptr;

  int m_w = i->GetWidth();
  int m_h = i->GetHeight();

  if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
  else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

  m_data = routput;

  unsigned char* rdata = nullptr;

  if (m_bilinear) rdata = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
  else rdata = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

  m_data = tmp;

  wxImage* im = &m_parent->GetFourierImage()->image;
  wxImage* rm = &m_parent->GetReconstructImage()->image;

  m_parent->GetFourier()->m_data = output;
  m_parent->GetFourier()->m_width = m_width;
  m_parent->GetFourier()->m_height = m_height;

  m_parent->GetReconstruct()->m_data = routput;
  m_parent->GetReconstruct()->m_width = m_width;
  m_parent->GetReconstruct()->m_height = m_height;

  auto Size = GetClientSize();

  im->Create(m_w, m_h, data);

  m_parent->GetFourierImage()->resized = wxBitmap(*im);
  m_parent->GetFourierImage()->paintNow();

  m_parent->GetFourier()->SetClientSize(Size.GetWidth(), Size.GetHeight());

  rm->Create(m_w, m_h, rdata);

  m_parent->GetReconstructImage()->resized = wxBitmap(*rm);
  m_parent->GetReconstructImage()->paintNow();

  m_parent->GetReconstruct()->SetClientSize(Size.GetWidth(), Size.GetHeight());

}

void MyFrame::OnSeperableFourier(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  if (!i->IsOk()) return;
  ImaginaryNum* out = SeparableFourierTransform(m_width, m_height, m_data);
  ImaginaryNum* dout = SeperableInverseFourierTransform(m_width, m_height, out);
  unsigned char* output = LogImaginaryToChar(m_width, m_height, out);
  unsigned char* routput = ImaginaryToChar(m_width, m_height, dout);

  // free(m_data);
  unsigned char* tmp = m_data;
  free(dout);
  free(out);

  m_data = output;

  unsigned char* data = nullptr;

  int m_w = i->GetWidth();
  int m_h = i->GetHeight();

  if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
  else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

  m_data = routput;

  unsigned char* rdata = nullptr;

  if (m_bilinear) rdata = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
  else rdata = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

  m_data = tmp;

  wxImage* im = &m_parent->GetFourierImage()->image;
  wxImage* rm = &m_parent->GetReconstructImage()->image;

  m_parent->GetFourier()->m_data = output;
  m_parent->GetFourier()->m_width = m_width;
  m_parent->GetFourier()->m_height = m_height;

  m_parent->GetReconstruct()->m_data = routput;
  m_parent->GetReconstruct()->m_width = m_width;
  m_parent->GetReconstruct()->m_height = m_height;

  auto Size = GetClientSize();

  im->Create(m_w, m_h, data);

  m_parent->GetFourierImage()->resized = wxBitmap(*im);
  m_parent->GetFourierImage()->paintNow();

  m_parent->GetFourier()->SetClientSize(Size.GetWidth(), Size.GetHeight());

  rm->Create(m_w, m_h, rdata);

  m_parent->GetReconstructImage()->resized = wxBitmap(*rm);
  m_parent->GetReconstructImage()->paintNow();

  m_parent->GetReconstruct()->SetClientSize(Size.GetWidth(), Size.GetHeight());
}

void MyFrame::OnFastFourierTransform(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  if (!i->IsOk()) return;
  // resize the image
  int log2width = log2f(m_width);
  int log2height = log2f(m_height);

  int nWidth  = 1 << log2width;
  int nHeight = 1 << log2height;

  unsigned char* resize_image = nullptr;

  if (m_bilinear) resize_image = m_parent->GetImagePanel()->BilinearInterpolation(nWidth, nHeight);
  else resize_image = m_parent->GetImagePanel()->NearestNeighbour(nWidth, nHeight);

  free(m_data);
  m_data = resize_image;
  m_width = nWidth;
  m_height = nHeight;

  ImaginaryNum* out = FastFourierTransform(m_width, m_height, m_data);
  ImaginaryNum* dout = InverseFastFourierTransform(m_width, m_height, out);
  unsigned char* output = LogImaginaryToChar(m_width, m_height, out);
  unsigned char* routput = ImaginaryToChar(m_width, m_height, dout);

  // free(m_data);
  unsigned char* tmp = m_data;
  free(dout);
  free(out);

  m_data = output;

  unsigned char* data = nullptr;

  int m_w = i->GetWidth();
  int m_h = i->GetHeight();

  if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
  else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

  m_data = routput;

  unsigned char* rdata = nullptr;

  if (m_bilinear) rdata = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
  else rdata = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

  m_data = tmp;

  wxImage* im = &m_parent->GetFourierImage()->image;
  wxImage* rm = &m_parent->GetReconstructImage()->image;

  m_parent->GetFourier()->m_data = output;
  m_parent->GetFourier()->m_width = m_width;
  m_parent->GetFourier()->m_height = m_height;

  m_parent->GetReconstruct()->m_data = routput;
  m_parent->GetReconstruct()->m_width = m_width;
  m_parent->GetReconstruct()->m_height = m_height;

  auto Size = GetClientSize();

  im->Create(m_w, m_h, data);

  m_parent->GetFourierImage()->resized = wxBitmap(*im);
  m_parent->GetFourierImage()->paintNow();

  m_parent->GetFourier()->SetClientSize(Size.GetWidth(), Size.GetHeight());

  rm->Create(m_w, m_h, rdata);

  m_parent->GetReconstructImage()->resized = wxBitmap(*rm);
  m_parent->GetReconstructImage()->paintNow();

  m_parent->GetReconstruct()->SetClientSize(Size.GetWidth(), Size.GetHeight());
}

void MyFrame::OnShowFourierSpecturm(wxCommandEvent & WXUNUSED(event))
{
	m_fourierShow = !m_fourierShow;
	m_parent->GetFourier()->Show(m_fourierShow);
}

void MyFrame::OnShowReconstructImage(wxCommandEvent & WXUNUSED(event))
{
	m_reconstructShow = !m_reconstructShow;
	m_parent->GetReconstruct()->Show(m_reconstructShow);
}

void MyFrame::OnPepperAndSaltNoise(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  PepperAndSaltDialog a("Pepper And Salt Noise", i, this);
  int result = a.ShowModal();

  if (result == wxID_OK)
  {
    double Pa = a.Pa;
    double Pb = a.Pb;
    unsigned char* output = PepperSaltNoise(m_width, m_height, m_data, Pa, Pb);
    free(m_data);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnNoiseReduction0_5(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  if (i->IsOk())
  {
    double Pa = 0.50;
    double Pb = 0.50;
    unsigned char* tmp = PepperSaltNoise(m_width, m_height, m_data, Pa, Pb);
    free(m_data);
    unsigned char* output = MedianFilter(m_width, m_height, tmp);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnNoiseReduction0_4(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  if (i->IsOk())
  {
    double Pa = 0.40;
    double Pb = 0.60;
    unsigned char* tmp = PepperSaltNoise(m_width, m_height, m_data, Pa, Pb);
    free(m_data);
    unsigned char* output = MedianFilter(m_width, m_height, tmp);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnNoiseReduction0_01(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  if (i->IsOk())
  {
    double Pa = 0.01;
    double Pb = 0.90;
    unsigned char* tmp = PepperSaltNoise(m_width, m_height, m_data, Pa, Pb);
    free(m_data);
    unsigned char* output = MedianFilter(m_width, m_height, tmp);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnMedianFilter(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  if (i->IsOk())
  {
    unsigned char* output = MedianFilter(m_width, m_height, m_data);
    // free(m_data);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnLocalNoiseReduction(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  LocalNoiseReductionDialog a("Local Noise Reduction", i, this);
  int result = a.ShowModal();

  if (result == wxID_OK)
  {
    double noiseVariance = a.noiseVariance;
    unsigned char* output = LocalNoiseReduction(m_width, m_height, m_data, noiseVariance);
    free(m_data);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnAMF(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  AdaptiveMedianFilterDialog a("Adaptive Median Filter", i, this);
  int result = a.ShowModal();

  if (result == wxID_OK)
  {
    int winsize = a.windowSize;
    unsigned char* output = AdaptiveMedianFilter(m_width, m_height, m_data, winsize);
    free(m_data);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}

void MyFrame::OnGaussianNoise(wxCommandEvent & WXUNUSED(event))
{
  wxImage* i = &m_parent->GetImagePanel()->image;
  GaussianNoiseDialog a("Gaussian Noise", i, this);
  int result = a.ShowModal();

  if (result == wxID_OK)
  {
    double mean = a.mean;
    double variance = a.variance;
    unsigned char* output = GaussianNoise(m_width, m_height, m_data, mean, variance);
    free(m_data);
    m_data = output;

    unsigned char* data = nullptr;

    int m_w = i->GetWidth();
    int m_h = i->GetHeight();

    if (m_bilinear) data = m_parent->GetImagePanel()->BilinearInterpolation(m_w, m_h);
    else data = m_parent->GetImagePanel()->NearestNeighbour(m_w, m_h);

    i->Create(m_w, m_h, data);

    m_parent->GetImagePanel()->resized = wxBitmap(*i);
    m_parent->GetImagePanel()->paintNow();
    CalculateHistogram();
  }
}