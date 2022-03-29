#include "AtlasUtils.C"
#include "draw_scan.C"

int Scan_Histogram(const int tile = 15){//}, const char *input = "../data/source_on_tile30_doorside.txt" ) {

  //FILE *input_file;
  //input_file = fopen(input, "r");

  //string line;
  //vector<double> x;
  //vector<double> y;
  //vector<double> IMON;
  //vector<double> RMON;
  //vector<double> VCOMP;
  //double tx, ty, tIMON, tRMON, tVCOMP;
  /*
  if (input_file == 0)
  {
     //fopen returns 0, the NULL pointer, on failure
     perror("Canot open input file\n");
     return 0;
  }
  else
  {
    getline(input, line);
    getline(input, line);
    while (getline(input, line)){
      stringstream ss(line);
      string word;
      while(getline(ss, word, ", "))

    }
  }
  */
  double x1 = 50.;
  double x2 = 56.;
  double y1 = 25.;
  double y2 = 36.;
  TRandom2 *tr = new TRandom2();
  gStyle->SetOptStat(0);
  TH2D *h_scan = new TH2D("h_scan",";x;y", 100, 0, 100, 50, 0, 50);

  h_scan->Draw("colz");
  draw_scan(17);
  myText(0.14, 0.84, kBlack, "Prospective Test: Middle Tile Scans", 0.04);
  TLine *line = new TLine(11, 26, 95, 36);
  line->SetLineWidth(2);
  line->SetLineColor(kGreen +2);

  line->Draw("same");

  line = new TLine(11, 24, 95, 14);
  line->SetLineWidth(2);
  line->SetLineColor(kGreen + 2);

  line->Draw("same");

  line = new TLine(48, 40, 48, 10);
  line->SetLineWidth(2);
  line->SetLineColor(kGreen + 2);
  line->Draw("same");

  line = new TLine(69, 45, 69, 5);
  line->SetLineWidth(2);
  line->SetLineColor(kGreen + 2);

  line->Draw("same");
  return 0;
}
