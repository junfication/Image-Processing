#pragma once
#include <wx/wx.h>
#include <vector>

#ifndef SIMPLE_HISTOGRAM_INC
#define SIMPLE_HISTOGRAM_INC

class SimpleHistogram : public wxPanel {
public:
  SimpleHistogram(wxWindow* parent, wxWindowID id,
                  const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                  long style = wxNO_BORDER, const wxString& name = wxPanelNameStr);
  void SetData(std::vector<double> d);
  void OnPaint(wxPaintEvent& event);
  void OnResize(wxSizeEvent& event);
  void PaintNow();
private:
  std::vector<double> data;
  double data_total;
  double offset;
  void render();
};

#endif