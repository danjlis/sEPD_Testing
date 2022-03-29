#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
#include "draw_scan.C"
#include "/sphenix/u/yeonjugo/yjUtility.h"

void make_sEPD_rootfile_v1(TString inputFileName, TString cap);
// void isSourceOnTopOfTheTile(const int tile, const double xorigin, const double yorigin, const double rot); //to_be_updated

void scan_sEPD_v4(bool makeRootFile = true){

  gStyle->SetOptStat(0);
  SetyjPadStyle();

  /////////////////////////////////////
  // Match version and the text file name
  int version = 8;
  TString cap = Form("_v%d", version);
  if(makeRootFile){
    if(version == 7){
      make_sEPD_rootfile_v1("20220121-1054_TEST_OUTPUT_x7to15_y15to23_dx2_dy2.txt",cap);//v7
    } else if(version == 8){
      make_sEPD_rootfile_v1("20220121-1720_TEST_OUTPUT.txt",cap);//v8
      //make_sEPD_rootfile_v1("20220121-1720_TEST_OUTPUT_temp_x85_y0.txt",cap);//v8
      //make_sEPD_rootfile_v1("20220121-1720_TEST_OUTPUT_temp_x75_y15.txt",cap);//v8
    } else if(version == 9){
      make_sEPD_rootfile_v1("20220125-0925_TEST_OUTPUT_x15_y30.txt",cap);//v9
      //make_sEPD_rootfile_v1("20220125-0925_TEST_OUTPUT_x5_y22p5.txt",cap);//v9
    } else if(version == 10){
      make_sEPD_rootfile_v1("20220125-1951_TEST_OUTPUT.txt",cap);//v10
    } else if(version == 11){
      make_sEPD_rootfile_v1("20220128-1823_TEST_OUTPUT_v11.txt",cap);//v11
      //make_sEPD_rootfile_v1("20220128-1812_TEST_OUTPUT.txt",cap);//v11_temp
    }
  }


  /////////////////////////////////////
  // IMPORT THE TREE FROM ROOT FILE
  TFile* fin = new TFile(Form("output/sEPD%s.root",cap.Data()), "read");
  TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
  Float_t xpos, ypos;
  vector<Int_t>* ch=nullptr;
  vector<Int_t>* tile=nullptr;
  vector<Int_t>* device=nullptr;
  vector<Float_t>* imon=nullptr;
  vector<Float_t>* rmon=nullptr;
  vector<Float_t>* vcomp=nullptr;
  inTree_p->SetBranchAddress("xpos", &xpos);
  inTree_p->SetBranchAddress("ypos", &ypos);
  inTree_p->SetBranchAddress("device", &device);
  inTree_p->SetBranchAddress("channel", &ch);
  inTree_p->SetBranchAddress("tile", &tile);
  inTree_p->SetBranchAddress("imon", &imon);
  inTree_p->SetBranchAddress("rmon", &rmon);
  inTree_p->SetBranchAddress("vcomp", &vcomp);

  TFile* fout = new TFile(Form("output/hist_sEPD%s.root",cap.Data()), "recreate");

  /////////////////////////////////////
  // DEFINE HISTOGRAMS
  const int nTILE = 32;

  TH1D* h1D_imon[nTILE];//imon dist for all source positions including dark current
  TH1D* h1D_imon_dc[nTILE];//imon dist for only for dark current

  const float xMin = 0.0;
  const float xMax = 96;
  const float yMin = 0.0;
  const float yMax = 50;
  float nSep_x = 0.8;//x bin width in cm
  float nSep_y = 2.5;//y bin width in cm
  //float nSep_y = 0.25;//y bin width in cm
  int nx = (xMax-xMin)/nSep_x;
  int ny = (yMax-yMin)/nSep_y;
  TH2D* h2D_x_y_imon_all = new TH2D(Form("h2D_x_y_imon_%s", "all"), ";x [cm];y [cm]", nx, xMin, xMax, ny, yMin, yMax);//source x,y position map for all source positions, to check the total scanning map
  TH2D* h2D_x_y_imon[nTILE];//imon response (z-axis) as a function of source x, y positions for each tile separately.

  for(int it = 0; it < nTILE; it++){
    h2D_x_y_imon[it] = new TH2D(Form("h2D_x_y_imon_tile%d", it), ";x [cm];y [cm]", nx, xMin, xMax, ny, yMin, yMax);
    h1D_imon[it] = new TH1D(Form("h1D_imon_tile%d", it), ";SiPM current (IMON) [#muA];", 2000, 0, 20.000);
    h1D_imon_dc[it] = new TH1D(Form("h1D_imon_darkCurrent_tile%d", it), ";SiPM current (IMON) [#muA];", 2000, 0, 20.000);
  }

  /////////////////////////////////////
  // EVENT LOOP
  ULong64_t nEntries = inTree_p->GetEntries();
  ULong64_t nDiv = TMath::Max((ULong64_t)1, nEntries/100);
  std::cout << "Total number of scan steps = " << nEntries << std::endl;
  for(ULong64_t entry = 0; entry < nEntries; ++entry){
    if(nEntries%nDiv == 0) std::cout << " Entry " << entry << "/" << nEntries << std::endl;
    inTree_p->GetEntry(entry);

    for(int it = 0; it<tile->size(); it++){
      double imonTemp = imon->at(it);
      h1D_imon[tile->at(it)]->Fill(imonTemp);//imon dist for all source positions including dark current

      if(xpos==0 && ypos==0){//for dark current
        h1D_imon_dc[tile->at(it)]->Fill(imonTemp);
      } else {
        h2D_x_y_imon_all->Fill(xpos, ypos, imonTemp);
        h2D_x_y_imon[tile->at(it)]->Fill(xpos, ypos, imonTemp);
      }
    }//tile
  }//event loop

  /////////////////////////////////////
  // Calculate Dark Current
  double mean_dc[nTILE];
  double rms_dc[nTILE];
  int nScanCount[nTILE];
  for(int it = 0; it < nTILE; it++){
    mean_dc[it] = h1D_imon_dc[it]->GetMean();
    rms_dc[it] = h1D_imon_dc[it]->GetRMS();
    nScanCount[it] = 0;
  }

  /////////////////////////////////////
  // EVENT LOOP - To estimate normalization factor
  for(ULong64_t entry = 0; entry < nEntries; ++entry){
    if(nEntries%nDiv == 0) std::cout << " Entry " << entry << "/" << nEntries << std::endl;
    inTree_p->GetEntry(entry);

    for(int it = 0; it<tile->size(); it++){
      double imonTemp = imon->at(it);
      int tilePos = tile->at(it);

      // 2022Jan30: in the future, add a function/condition which examine if the source is on top of the tile
      if(imonTemp > (mean_dc[tilePos]+3.0*rms_dc[tilePos])){
        nScanCount[tilePos]+=1;
      }
    }//tile
  }//event loop


  ////////////////////////////////////////////////////////
  // 2022Jan29: Work-in-progress
  // normalize the 2D hist
  // for(int it = 0; it < nTILE; it++){
  //   cout << "Tile #" << it << ": dark current mean, rms, number of scan = " << mean_dc[it] << ", " << rms_dc[it] << ", " << nScanCount[it] << endl;
  //   if(it==nTILE-1) {
  //     cout << "before normalization" << endl;
  //     h2D_x_y_imon[it]->Print("all");
  //   }
  //   h2D_x_y_imon[it]->Scale(1./(double)nScanCount[it]);
  //   for(int ix = 0; ix < h2D_x_y_imon[it]->GetNbinsX(); ++ix){
  //     for(int iy = 0; iy < h2D_x_y_imon[it]->GetNbinsY(); ++iy){
  //       float tempCont = h2D_x_y_imon[it]->GetBinContent(ix+1,iy+1);
  //       if(tempCont!=0){
  //         // cout << "OrgCont, mean_dc[it], bkSubCont = " << tempCont << ", " << mean_dc[it] << ", " << tempCont-mean_dc[it] << endl;
  //         h2D_x_y_imon[it]->SetBinContent(ix+1,iy+1, tempCont-mean_dc[it]);
  //       }
  //     }
  //   }
  // if(it==nTILE-1){
  // cout << "after normalization" << endl;
  //  h2D_x_y_imon[it]->Print("all");
  // }
  for(int it = 0; it < nTILE; it++){
    h2D_x_y_imon[it]->Scale(1./10);
    for(int ix = 0; ix < h2D_x_y_imon[it]->GetNbinsX(); ++ix){
      for(int iy = 0; iy < h2D_x_y_imon[it]->GetNbinsY(); ++iy){
        float tempCont = h2D_x_y_imon[it]->GetBinContent(ix+1,iy+1);
        if(tempCont!=0){
          h2D_x_y_imon[it]->SetBinContent(ix+1,iy+1, tempCont-mean_dc[it]);
        }
      }
    }
  }

  /////////////////////////////////////
  // START DRAWING
  TString savedir = "/sphenix/u/yeonjugo/sEPD";
  float xPos = 0.12;
  //float xPos = 0.85;
  float yPos = 0.86;
  float yPos2 = 0.2;
  float dy = 0.05;
  float dy2 = 0.05;
  float fontType = 43;
  float fontSize = 13;

  //tile overlay position
  const float xorigin = 87;
  const float yorigin = 23.5;
  const float rot = 3*TMath::Pi()/2-0.005;

  /////////////////////////////////////
  // DRAW PLOTS 2D IMON
  cout << "Plotting x-y imon weighted 2D histograms for each tile" << endl;

  TCanvas* c_2d_2 = new TCanvas(Form("c_2d_1_tile%s","all"),"", 750,400);
  c_2d_2->SetRightMargin(0.13);
  c_2d_2->SetLeftMargin(0.10);
  SetHistTextSize(h2D_x_y_imon_all);
  h2D_x_y_imon_all->SetTitleOffset(0.8, "Y");
  h2D_x_y_imon_all->Draw("colz");
  draw_scan(1, xorigin, yorigin, rot);
  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("STAR EPD",xPos,yPos-dy2,0, 1, fontSize, fontType);
  c_2d_2->SaveAs(Form("%s/figures/hist2D_x_y_IMON%s_allChannel.pdf",savedir.Data(),cap.Data()));

  /////////////////////////////////////
  // DRAW PLOTS 2D IMON
  cout << "Plotting x-y imon weighted 2D histograms for each tile" << endl;

  TCanvas* c_2d_1[nTILE];
  for(int it = 1; it < nTILE; it++){
    c_2d_1[it] = new TCanvas(Form("c_2d_1_tile%d",it),"", 750,400);
    c_2d_1[it]->SetRightMargin(0.13);
    c_2d_1[it]->SetLeftMargin(0.10);
    SetHistTextSize(h2D_x_y_imon[it]);
    h2D_x_y_imon[it]->SetTitleOffset(0.8, "Y");
    float zMax = h2D_x_y_imon[it]->GetBinContent(h2D_x_y_imon[it]->GetMaximumBin());
    cout << " zMax = " << zMax << endl;
    h2D_x_y_imon[it]->GetZaxis()->SetRangeUser(0.2,2.0);
    //h2D_x_y_imon[it]->GetZaxis()->SetRangeUser(-0.5,zMax*1.2);
    h2D_x_y_imon[it]->Draw("colz");

    if(it==1)
      draw_scan(it, xorigin, yorigin, rot);
    else if(it!=1 && it%2==0)
      draw_scan(it+1, xorigin, yorigin, rot);
    else if(it!=1 && it%2==1)
      draw_scan(it-1, xorigin, yorigin, rot);

    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText("STAR EPD",xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Tile ##bf{%d}", it),xPos,yPos-dy2*2,0, 1, fontSize, fontType);
    drawText(Form("Background-subtracted current%s",""),xPos,yPos2,0, 1, fontSize, fontType);

    c_2d_1[it]->SaveAs(Form("%s/figures/hist2D_x_y_IMON%s_tile%d.pdf",savedir.Data(),cap.Data(),it));
  }

  ///////////////////////////////////////
  //// DRAW PLOTS 2D IMON IN TWO CANVAS
  //cout << "Plotting x-y imon weighted 2D histograms for each channel in TWO canvas" << endl;
  //xPos = 0.10;
  //yPos = 0.86;
  //yPos2 = 0.25;

  //TCanvas* c_2d_3[2];
  //for(int i=0; i<2;i++){
  //  c_2d_3[i] = new TCanvas(Form("c_2d_3_canPos%d",i),"", 750*2,400*2);
  //  c_2d_3[i]->Divide(4,4, 0.0, 0.0);
  //  c_2d_3[i]->SetLeftMargin(0);
  //  c_2d_3[i]->SetRightMargin(0);
  //  c_2d_3[i]->SetBottomMargin(0);
  //  c_2d_3[i]->SetTopMargin(0);
  //  for(int j=0; j<16;j++){
  //    c_2d_3[i]->cd(j+1);
  //    gPad->SetLeftMargin(0.1);
  //    gPad->SetRightMargin(0.1);
  //    gPad->SetBottomMargin(0.2);
  //    gPad->SetTopMargin(0.01);
  //  }

  //  // makeMultiPanelCanvas(c_2d_3[i], 4, 4, 0.02, 0.04, 0.02, 0.05, 0.1, 0.2, 0.1);
  //}
  //for(int it = 1; it < nTILE; it++){
  //  int canPos = 0;
  //  if(it>16) canPos = 1;
  //  if(it<=16)
  //    c_2d_3[canPos]->cd(it);
  //  else
  //    c_2d_3[canPos]->cd(it-16);

  //  h2D_x_y_imon[it]->Draw("colz");
  //  h2D_x_y_imon[it]->SetTitleOffset(0.5, "Y");
  //  h2D_x_y_imon[it]->SetTitleOffset(3.3, "X");

  //  if(it==1)
  //    draw_scan(it, xorigin, yorigin, rot);
  //  else if(it!=1 && it%2==0)
  //    draw_scan(it+1, xorigin, yorigin, rot);
  //  else if(it!=1 && it%2==1)
  //    draw_scan(it-1, xorigin, yorigin, rot);

  //  drawText(Form("Tile ##bf{%d}", it),xPos,yPos-dy2*2,0, 1, fontSize, fontType);
  //  drawText(Form("Background-subtracted current%s",""),xPos,yPos2,0, 1, fontSize, fontType);

  //}
  //c_2d_3[1]->cd(16);
  //drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  //drawText("STAR EPD",xPos,yPos-dy2,0, 1, fontSize, fontType);
  //c_2d_3[0]->SaveAs(Form("%s/figures/hist2D_x_y_IMON%s_inTwoCanvas_tile1to16.pdf",savedir.Data(),cap.Data()));
  //c_2d_3[1]->SaveAs(Form("%s/figures/hist2D_x_y_IMON%s_inTwoCanvas_tile17to31.pdf",savedir.Data(),cap.Data()));

  /////////////////////////////////////
  // DRAW PLOTS 1D IMON FOR EACH TILE
  cout << "Plotting imon 1D dist for each tile" << endl;
  int colHere[] = {kPink+5,kRed+1,kMagenta+2,kGreen+1,kGreen+3};
  xPos = 0.85;
  yPos = 0.86;
  dy = 0.05;
  dy2 = 0.06;
  fontType = 43;
  fontSize = 13;

  TLegend* l1[nTILE];
  TCanvas* c_1d_1[nTILE];
  for(int it = 1; it < nTILE; it++){
    c_1d_1[it] = new TCanvas(Form("c_1d_1_tile%d",it),"", 450,400);
    c_1d_1[it]->SetRightMargin(0.13);

    l1[it] = new TLegend(0.18,0.90-0.15*1,0.50,0.90);
    legStyle(l1[it]);
    l1[it]->SetTextSize(0.03);
    l1[it]->SetMargin(0.14);

    h1D_imon[it]->Draw("hist");
    // l1[it]->AddEntry(h1D_imon_woSr[it], Form("#splitline{w/o source}{#splitline{Mean %.3f}{RMS %.3f}}",mean_woSr[it],rms_woSr[it]), "l");
    //l1[it]->AddEntry(h1D_imon_woSr[it], Form("#splitline{x,y=(0,0)cm ~ w/o source}{#splitline{Mean %.3f}{RMS %.3f}}",mean_woSr[it],rms_woSr[it]), "l");
    setHistLineStyle(h1D_imon[it], kBlack, 1, 2);
    float xmax_temp = getHistHighXvalue(h1D_imon[it]);
    h1D_imon[it]->GetXaxis()->SetRangeUser(0.0, xmax_temp*1.2);
    h1D_imon[it]->GetYaxis()->SetRangeUser(0.5, 10000);
    gPad->SetLogy();

    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,1, 1, fontSize+2, fontType);
    drawText("STAR EPD",xPos,yPos-dy2,1, 1, fontSize, fontType);
    drawText(Form("Tile ##bf{%d}", it),xPos,yPos-dy2*2,1, 1, fontSize, fontType);

    c_1d_1[it]->SaveAs(Form("%s/figures/hist1D_IMON%s_tile%d.pdf",savedir.Data(),cap.Data(),it));
  }

  /////////////////////////////////////
  // DRAW PLOTS 1D IMON FOR EACH TILE
  cout << "Plotting imon 1D dist for each tile in one canvas" << endl;
  TCanvas* c_1d_2 = new TCanvas("c_1d_2", "", 6*250,6*200);
  makeMultiPanelCanvas(c_1d_2, 6, 6, 0.02, 0.04, 0.02, 0.1, 0.15, 0.10, 0.05);
  int colTemp = 9;
  for(int it = 1; it < nTILE; it++){
    c_1d_2->cd(it);
    SetHistTextSize(h1D_imon[it]);

    h1D_imon[it]->Draw("hist");
    h1D_imon[it]->GetXaxis()->SetTitleOffset(6.5);
    setHistLineStyle(h1D_imon[it], colTemp, 1, 1);
    gPad->SetLogy();

    if(it<=6) yPos = 0.75;
    else yPos = 0.82;
    if((it-1)%6==0) xPos = 0.85;
    else xPos = 0.80;
    // if((it-1)%6==0) xPos = 0.2;
    // else xPos = 0.18;
    drawText(Form("#color[%d]{Tile ##bf{%d}}", kBlack, it), xPos,yPos, 1,1, fontSize+2, fontType);
    //drawText(Form("#color[%d]{Tile ##bf{%d}}", colTemp, it), xPos,yPos, 1,1, fontSize+2, fontType);
    // drawText(Form("#color[%d]{Mean %.3f}", colTemp, mean_woSr[it]), xPos,yPos-dy, 0,1, fontSize, fontType);
    // drawText(Form("#color[%d]{RMS %.3f}", colTemp, rms_woSr[it]), xPos,yPos-dy*2, 0,1, fontSize, fontType);
    // drawText(Form("CH ##bf{%d}", it), xPos,yPos, 0,1, fontSize+2, fontType);
    // drawText(Form("Mean %.3f", mean_woSr[it]), xPos,yPos-dy, 0,1, fontSize, fontType);
    // drawText(Form("RMS %.3f", rms_woSr[it]), xPos,yPos-dy*2, 0,1, fontSize, fontType);
  }
  c_1d_2->cd(nTILE+1-1);
  xPos = 0.2;
  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("STAR EPD",xPos,yPos-dy2,0, 1, fontSize, fontType);
  // drawText(Form("With {}^{90}_{38}Sr on CH##color[%d]{%d}",col,ch_s),xPos,yPos-dy2*2,0, 1, fontSize, fontType);
  // drawText("#splitline{(CH# does not}{ match to tile #)}",xPos,yPos-dy2*3.5,0, 1, fontSize, fontType);

  c_1d_2->SaveAs(Form("%s/figures/hist1D_IMON%s_allChannels.pdf",savedir.Data(),cap.Data()));

  /////////////////////////////////////
  // DRAW PLOTS 1D IMON Dark Current FOR EACH TILE
  cout << "Plotting imon 1D dist for dark current for each tile in one canvas" << endl;
  TCanvas* c_1d_3 = new TCanvas("c_1d_3", "", 6*250,6*200);
  makeMultiPanelCanvas(c_1d_3, 6, 6, 0.02, 0.04, 0.02, 0.1, 0.15, 0.10, 0.05);

  for(int it = 1; it < nTILE; it++){
    c_1d_3->cd(it);
    SetHistTextSize(h1D_imon_dc[it]);

    h1D_imon_dc[it]->Draw("hist");
    h1D_imon_dc[it]->GetXaxis()->SetTitleOffset(6.5);
    h1D_imon_dc[it]->SetTitle(Form(";Dark Current [#muA];"));
    float xmax_temp = getHistHighXvalue(h1D_imon_dc[it]);
    h1D_imon_dc[it]->GetXaxis()->SetRangeUser(0.0, 0.6);
    setHistLineStyle(h1D_imon_dc[it], colTemp, 1, 1);
    // gPad->SetLogy();

    if(it<=6) yPos = 0.75;
    else yPos = 0.82;
    if((it-1)%6==0) xPos = 0.85;
    else xPos = 0.80;
    drawText(Form("#color[%d]{Tile ##bf{%d}}", kBlack, it), xPos,yPos, 1,1, fontSize+2, fontType);
  }
  c_1d_3->cd(nTILE+1-1);
  xPos = 0.2;
  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("STAR EPD",xPos,yPos-dy2,0, 1, fontSize, fontType);
  drawText("Dark Current",xPos,yPos-dy2*2,0, 1, fontSize, fontType);

  c_1d_3->SaveAs(Form("%s/figures/hist1D_IMON_darkCurrent%s_allChannels.pdf",savedir.Data(),cap.Data()));


  ///////////////////////////////////////
  //// SAVE HISTOGRAMS

  //fout->cd();
  //for(int it = 1; it < nTILE; it++){
  //  h1D_imon_woSr[it]->Write();
  //}
  //h1D_mean->Write();
  //h1D_rms->Write();
  //fout->Close();
}

void make_sEPD_rootfile_v1(TString inputFileName = "20220113-1447_TEST_OUTPUT.txt", TString cap=""){
  cout << "RUNNING: make_sEPD_rootfile_v1 for " << inputFileName << " input file" << endl;

  ifstream infile(Form("input/%s", inputFileName.Data()));
  Float_t xpos, ypos;
  vector<Int_t> device;
  vector<Int_t> ch;
  vector<Int_t> tile;
  vector<Float_t> imon, rmon, vcomp;

  /////////////////////////////////////
  // DEFINE TREE AND OUTPUT ROOT FILE
  TString fname = Form("output/sEPD%s.root",cap.Data());
  TFile* fout = new TFile(Form("%s",fname.Data()), "recreate");
  TTree* tr = new TTree("sEPDTree", "");
  tr->Branch("xpos", &xpos, "xpos/F");
  tr->Branch("ypos", &ypos, "xpos/F");
  tr->Branch("device", &device);
  tr->Branch("channel", &ch);
  tr->Branch("tile", &tile);
  tr->Branch("imon", &imon);
  tr->Branch("rmon", &rmon);
  tr->Branch("vcomp", &vcomp);

  /////////////////////////////////////
  // IMPORT NUMBERS FROM THE TXT FILE
  Long64_t nLines = 0;
  Bool_t isDiffPosition = false;
  Float_t xpos_temp = 0;
  Float_t ypos_temp = 0;
  while (infile)
  {
    string s;
    if (!getline( infile, s )) break;
    if (s.empty()) continue;
    if (!isdigit(s.at(0))) continue;

    istringstream ss( s );
    vector<string> vec_str;

    while (ss)
    {
      string stemp;
      if (!getline( ss, stemp, ',' )) break;
      vec_str.push_back( stemp );
    }

    if(vec_str.size()==9){
      device.push_back(stoi(vec_str[0]));
      ch.push_back(stoi(vec_str[1]));
      tile.push_back(stoi(vec_str[2]));
      xpos = stof(vec_str[4]);
      ypos = stof(vec_str[5]);
      imon.push_back(stof(vec_str[6]));
      rmon.push_back(stof(vec_str[7]));
      vcomp.push_back(stof(vec_str[8]));
      nLines++;
    }

    if(tile.size()==32){
      tr->Fill();
      device.clear();
      ch.clear();
      tile.clear();
      imon.clear();
      rmon.clear();
      vcomp.clear();
    }
  }

  cout << "total numer of lines analyzed = " << nLines << endl;
  fout->cd();
  tr->Write("", TObject::kOverwrite);
  delete tr;
  fout->Close();
  cout << "DONE: " << fname << " has been created" << endl;
}

// void isSourceOnTopOfTheTile(const int tile = 8, const double xorigin = 96., const double yorigin = 25., const double rot = 3*TMath::Pi()/2){
// }
