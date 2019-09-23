#include "simple_histogram.h"
#include <wx/graphics.h>
#include <memory>
using namespace std;

SimpleHistogram::SimpleHistogram(wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size,
  long style, const wxString& name)
  : wxPanel(parent, id, pos, size, style, name)
{
  Connect(this->GetId(), wxEVT_PAINT, wxPaintEventHandler(SimpleHistogram::OnPaint));
  Connect(this->GetId(), wxEVT_SIZE, wxSizeEventHandler(SimpleHistogram::OnResize));
  offset = 1.0;

}

void SimpleHistogram::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  render();
}

void SimpleHistogram::OnResize(wxSizeEvent& event)
{
  Refresh();
  event.Skip();
}

void SimpleHistogram::PaintNow()
{
  Refresh();
}

void SimpleHistogram::render()
{
  wxPaintDC pdc(this);
  auto_ptr<wxGraphicsContext> dc(wxGraphicsContext::Create(pdc));

  dc->SetBrush(*wxBLUE_BRUSH);

  wxFont font(1, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
  wxClientDC Cdc(this);
  Cdc.SetFont(font);
  Cdc.SetBackgroundMode(wxTRANSPARENT);
  Cdc.SetTextForeground(*wxBLACK);
  Cdc.SetTextBackground(*wxWHITE);

  int width, height;
  GetClientSize(&width, &height);

  const double column_width = static_cast<double>(width) / data.size();

  int i = 0;
  double item_x, item_y, item_ratio, item_height;
  for (double d : data) {
    item_x = i * column_width;
    item_ratio = data_total ? d / data_total : 0;
    item_ratio *= offset;
    item_height = item_ratio * (height - 24.0);
    item_y = height - item_height;

    // dc->DrawRectangle(item_x, item_y - 24.0, column_width, item_height);
    dc->DrawRectangle(item_x, item_y, column_width, item_height);
    // Cdc.DrawText(std::to_string(d), item_x + column_width / 2.0, item_y - 48.0);
    // Cdc.DrawText(std::to_string(static_cast<int>(i)), item_x + column_width / 2.0, height - 24.0);
    i++;
  }
}

void SimpleHistogram::SetData(vector<double> d)
{
  double max = 0.0;
  data.assign(d.begin(), d.end());
  data_total = 0;
  for(double tmp : data) {
    data_total += tmp;
    max = tmp > max ? tmp : max;
  }
  offset = data_total / max;
}