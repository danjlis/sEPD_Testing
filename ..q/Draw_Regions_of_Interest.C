#include "sEPD_functions.C"
#include "yjUtility.h"
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
void Draw_Regions_of_Interest(){
  gStyle->SetOptStat(0);

  Tile *arr_tile[31];
  int cheese = TileInfo(arr_tile);
  for(int i = 0; i < 31; i++){
    cout<<"tile : "<<arr_tile[i]->tile<<endl;
  }
  int colors4[4] = {kBlack, kRed+1, kBlue+1, kGreen-3}; //, kCyan+1, kOrange -3, kMagenta -3, kGray, kRed -1, kBlue -1, kGreen -7, kCyan - 3, kOrange+1, kMagenta -5, kViolet+6, kAzure +3};

  const double yo = 0;
  const double xMin = 3;
  const double xMax = 95;
  const double yMin = 0.0;
  const double yMax = 49.0;
  double nSep_x = .5;//x bin width in cm
  double nSep_y = .5;//y bin width in cm
  //float nSep_y = 0.25;//y bin width in cm
  int nx = (xMax - xMin)/nSep_x;
  int ny = (yMax - yMin)/nSep_y;
  cout<<"bins: "<<nx<<" , "<<ny<<endl;
  TH2D* h2D_x_y_imon_all_norm = new TH2D(Form("h2D_x_y_imon_%s", "all_norm"), ";x [cm];y [cm]", nx, xMin, xMax, ny, yMin, yMax);
  TCanvas *c1 = new TCanvas("c1","c1", 750, 400);
  h2D_x_y_imon_all_norm->Draw();
  const float xorigin = 90.5;
  const float yorigin = 24;
  const float rot = 3*TMath::Pi()/2;
  const float sx = 23.;
  const float sy = 86.;
  draw_scan(1, xorigin, yorigin, rot, sx, sy);
  TBox *boxx;
  TLine *line;
  TGraph *tcenters = new TGraph(31);
  double txc, tyc, txr, tyr;
  for(int i = 0; i < 31; i++){
    txc = arr_tile[i]->xc;
    txr = arr_tile[i]->xr;
    tyc = arr_tile[i]->yc;
    tyr = arr_tile[i]->yr;
    cout<<"("<<txc - txr<<", "<< tyc - tyr<<") -> (" <<txc + txr<<", " <<tyc + tyr<<") "<<endl;
    boxx = new TBox(txc - txr, tyc - tyr, txc + txr, tyc + tyr);
    boxx->SetLineColor(colors4[i%4]);
    boxx->SetLineWidth(1);
    boxx->SetFillStyle(0);

    boxx->Draw("same l");

    tcenters->SetPoint(i, txc, tyc);
  }
  tcenters->SetMarkerStyle(5);
  tcenters->SetMarkerSize(1);
  tcenters->Draw("P");
  int ttt = 21;
  double x1 = arr_tile[ttt]->xm + 2;
  double y1 = 24;
  double x2 = arr_tile[ttt]->xm - abs(arr_tile[ttt]->yc - 24)*TMath::Tan(TMath::Pi()/24);
  double y2 = arr_tile[ttt]->yc;
  line = new TLine(x1, y1, x2, y2);
  line->SetLineWidth(2);
  line->SetLineColor(kOrange);
  line->Draw("same");

return;
}
