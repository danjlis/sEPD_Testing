#ifndef UTILITY_Yeonju_H
#define UTILITY_Yeonju_H

//TREE,HIST,GRAPH,VECTOR ... 
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TEfficiency.h>
#include <TProfile.h>
#include <TF1.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TLorentzVector.h>
//COSTMETIC, CANVAS, LEGEND, LATEX ...
#include <TAxis.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TLine.h>
#include <TLegend.h>
#include <TPaletteAxis.h>
#include <TBox.h>
#include <TEnv.h>
//FILE, TREE
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TNtuple.h>
#include <TObjArray.h>
//SYSTEM
#include <TROOT.h>
#include <TCut.h>
#include <TSystem.h>
#include <TDatime.h>
#include <TMath.h>
#include <stdio.h>
//C++, STRING
#include <TString.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::find()
#include <iomanip>      // std::setprecision()
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <math.h>
//RANDOM
#include <TRandom.h>
#include <TStopwatch.h>
#include <ctime>        // std::clock()
//etc. 
//#include "tdrstyle.C"   // std::clock()
using namespace std;

const int col[] = {1,2,3,4,6,7,28,46,41};
const int ycol[] = {8,9,28,46,41};
const int marker[] = {24,25,26,27,28,29,31,33,34};

#define TH1_TO_TGRAPH_2(hist, graph)         \
  graph = new TGraphAsymmErrors();                                 \
for (int p=0; p<hist->GetNbinsX(); ++p) {                                         \
  double Xmean = hist->GetBinCenter(p+1);                                 \
  double Ymean = hist->GetBinContent(p+1);                \
  double Xerr_l= hist->GetBinCenter(p+1)-hist->GetBinLowEdge(p+1);                             \
  double Xerr_h= hist->GetBinLowEdge(p+2)-hist->GetBinCenter(p+1);                           \
  double Yerr_l= hist->GetBinError(p+1);                  \
  double Yerr_h= hist->GetBinError(p+1);                  \
  graph->SetPoint(p, Xmean, Ymean);                                \
  graph->SetPointError(p,Xerr_l,Xerr_h,Yerr_l,Yerr_h);             \
}

#define TH1_TO_TGRAPH(hist, graph, points, offset_i, offset_f)         \
  graph = new TGraphAsymmErrors(points);                                 \
for (int p=0; p<points; ++p) {                                         \
  double Xmean = hist->GetBinCenter(p+1+offset_i);                                 \
  double Ymean = hist->GetBinContent(p+1+offset_i);                \
  double Xerr_l= 0;                             \
  double Xerr_h= 0;                           \
  double Yerr_l= hist->GetBinError(p+1+offset_i);                  \
  double Yerr_h= hist->GetBinError(p+1+offset_i);                  \
  graph->SetPoint(p, Xmean, Ymean);                                \
  graph->SetPointError(p,Xerr_l,Xerr_h,Yerr_l,Yerr_h);             \
}

#define TH1_TO_TGRAPH_1(hist, graph, points, hist_draw)         \
graph = new TGraphAsymmErrors(points);                                 \
for (int p=0; p<points; ++p) {                                         \
  double Xmean = hist_draw->GetBinContent(p+1);                                 \
  double Ymean = hist->GetBinContent(p+1);                \
  double Xerr_l= Xmean-(hist->GetBinLowEdge(p+1));                             \
  double Xerr_h= (hist->GetBinLowEdge(p+1)+hist->GetBinWidth(p+1))-Xmean;                           \
  double Yerr_l= hist->GetBinError(p+1);                  \
  double Yerr_h= hist->GetBinError(p+1);                  \
  graph->SetPoint(p, Xmean, Ymean);                                \
  graph->SetPointError(p,Xerr_l,Xerr_h,Yerr_l,Yerr_h);             \
}

#define _SET_BRANCH_VEC(tree, type, branch)     \
    std::vector<type>* branch = 0;              \
    tree->SetBranchStatus(#branch, 1);          \
    tree->SetBranchAddress(#branch, &branch);   \

#define _SET_BRANCH_VAR(tree, type, branch)     \
    type branch;                                \
    tree->SetBranchStatus(#branch, 1);          \
    tree->SetBranchAddress(#branch, &branch);   \

TH1D* getPullHist(TH1 *h1, TF1 *f){
  Int_t nBins = h1->GetNbinsX();
  Double_t xMin = h1->GetBinLowEdge(1);
  Double_t xMax = h1->GetBinLowEdge(nBins)+h1->GetBinWidth(nBins);
  TString xTitle = Form("%s", h1->GetXaxis()->GetTitle());
  TH1D* hp = new TH1D(Form("%s_pull",h1->GetName()), Form(";%s;Pull",xTitle.Data()), nBins,xMin,xMax);
  //TH1F* hp = new TH1F(Form("%s_pull",h1->GetName()), Form(";#frac{reco p_{T}^{#gamma}}{gen p_{T}^{#gamma}};Pull", nBins,xMin,xMax);
  for(Int_t ix=1; ix<=nBins; ++ix){
    float data = h1->GetBinContent(ix);
    float data_err = h1->GetBinError(ix);
    float data_pos = h1->GetBinLowEdge(ix)+(h1->GetBinWidth(ix))/2.;
    float fit = f->Eval(data_pos);
    float pull = (data-fit)/data_err;
    if(data==0) pull = 0;
    hp->SetBinContent(ix+1,pull);
    hp->SetBinError(ix+1,0);
    // cout << "pull hist bin " << ix << " value = " << hp->GetBinContent(ix+1) << endl;
  }
  return hp;
}

void get2DHistBinning(TH2* hist, vector<float>& xbins_vec, vector<float>& ybins_vec){
  int nBins_x = hist->GetNbinsX();
  int nBins_y = hist->GetNbinsY();
  cout << "get2DHistBinning of " << hist->GetName() << ":: nBins_x and nBins_y = " << nBins_x << ", " << nBins_y<< endl;
  TH1* tmpHist_x = hist->ProjectionX();
  for(int i=0; i<nBins_x; ++i){
    if(i==0)
      cout << "xBinning : " << tmpHist_x->GetBinLowEdge(i+1);
    else
      cout << ", " << tmpHist_x->GetBinLowEdge(i+1); 
    xbins_vec.push_back(tmpHist_x->GetBinLowEdge(i+1));
  }
  cout << ", " << tmpHist_x->GetBinLowEdge(nBins_x)+tmpHist_x->GetBinWidth(nBins_x) << endl;
  xbins_vec.push_back(tmpHist_x->GetBinLowEdge(nBins_x)+tmpHist_x->GetBinWidth(nBins_x));

  TH1* tmpHist_y = hist->ProjectionY();
  for(int i=0; i<nBins_y; ++i){
    if(i==0)
      cout << "yBinning : " << tmpHist_y->GetBinLowEdge(i+1);
    else
      cout << ", " << tmpHist_y->GetBinLowEdge(i+1); 
    ybins_vec.push_back(tmpHist_y->GetBinLowEdge(i+1));
  }
  cout << ", " << tmpHist_y->GetBinLowEdge(nBins_y)+tmpHist_y->GetBinWidth(nBins_y) << endl;
  ybins_vec.push_back(tmpHist_y->GetBinLowEdge(nBins_y)+tmpHist_y->GetBinWidth(nBins_y));
}

void getHistBinning(TH1* hist, vector<float>& bins_vec){
  int nBins = hist->GetNbinsX();
  cout << "getHistBinning nBins = " << nBins<< endl;
  for(int i=0; i<nBins; ++i){
    cout << hist->GetBinLowEdge(i+1) << endl;
    bins_vec.push_back(hist->GetBinLowEdge(i+1));
  }
  bins_vec.push_back(hist->GetBinLowEdge(nBins)+hist->GetBinWidth(nBins));
}

void AbsHist(TH1D* hist){
  int nBins = hist->GetNbinsX();
  for(int i=0; i<nBins; ++i){
    double content = hist->GetBinContent(i+1);
    double error = hist->GetBinError(i+1);
    hist->SetBinContent(i+1,abs(content));
    hist->SetBinError(i+1,abs(error));
  }
}

void divideHist(TH1D* horg, TH1D* hout[], int nDivision){
  int nSubBins = (horg->GetNbinsX()+1)/nDivision;
  double subBinning[nSubBins+1];
  for(int i=0;i<nSubBins;++i){
    subBinning[i] = horg->GetBinLowEdge(i+1);
  }
  subBinning[nSubBins] = horg->GetBinLowEdge(nSubBins+1);

  //hout[nDivision];
  for(int i=0; i<nDivision; ++i){
    hout[i] = new TH1D(Form("%s_divided_bin%d",horg->GetName(),i),Form("%s",horg->GetTitle()),nSubBins,subBinning);
    for(int j=0;j<nSubBins;++j){
      double content = horg->GetBinContent(j+i*nSubBins+1);
      double error = horg->GetBinError(j+i*nSubBins+1);
      hout[i]->SetBinContent(j+1,content);
      hout[i]->SetBinError(j+1,error);
    }
  }
  //return hout;
}

void toyStudy(TH2D* h2D_org, TH2D* h2D_toy){
  for(int ii=0; ii<h2D_org->GetNbinsX();++ii){
    for(int jj=0; jj<h2D_org->GetNbinsY();++jj){
      double tempVal = h2D_org->GetBinContent(ii,jj);
      double tempErr = h2D_org->GetBinError(ii,jj);
      h2D_toy->SetBinContent(ii,jj,tempVal);
      h2D_toy->SetBinError(ii,jj,tempErr);
    }
  }

  for(int ii=0; ii<h2D_org->GetNbinsX();++ii){
    for(int jj=0; jj<h2D_org->GetNbinsY();++jj){
      double tempVal = h2D_org->GetBinContent(ii,jj);
      double tempErr = h2D_org->GetBinError(ii,jj);
      if(ii==jj){
        //if(tempVal!=0){}
        //TF1 *tfAsyGauss = new TF1("tfAsy",assym_gaus,tempVal-tempErr*10,tempVal+tempErr*10,3);
        //tfAsyGauss->SetParameters(tempVal,tempErr,tempErr);
        double toyVal = gRandom->Gaus(tempVal,tempErr);
        cout << "Bin number x = "<< ii << ", y = " << jj << " : bin value = " << tempVal << ", bin error = " << tempErr << ", toy value = " << toyVal << ", relative difference = " << (toyVal-tempVal)/tempVal << endl;
        toyVal = gRandom->Gaus(tempVal,tempErr);
        cout << "Bin number x = "<< ii << ", y = " << jj << " : bin value = " << tempVal << ", bin error = " << tempErr << ", toy value = " << toyVal << ", relative difference = " << (toyVal-tempVal)/tempVal << endl;
        toyVal = gRandom->Gaus(tempVal,tempErr);
        cout << "Bin number x = "<< ii << ", y = " << jj << " : bin value = " << tempVal << ", bin error = " << tempErr << ", toy value = " << toyVal << ", relative difference = " << (toyVal-tempVal)/tempVal << endl;
        toyVal = gRandom->Gaus(tempVal,tempErr);
        cout << "Bin number x = "<< ii << ", y = " << jj << " : bin value = " << tempVal << ", bin error = " << tempErr << ", toy value = " << toyVal << ", relative difference = " << (toyVal-tempVal)/tempVal << endl;
        //toyVal = tfAsyGauss->GetRandom();
        //cout << "Bin number x = "<< ii << ", y = " << jj << " : bin value = " << tempVal << ", bin error = " << tempErr << ", toy value = " << toyVal << ", relative difference = " << (toyVal-tempVal)/tempVal << endl;
        //toyVal = tfAsyGauss->GetRandom();
        //cout << "Bin number x = "<< ii << ", y = " << jj << " : bin value = " << tempVal << ", bin error = " << tempErr << ", toy value = " << toyVal << ", relative difference = " << (toyVal-tempVal)/tempVal <<  endl;
        //toyVal = tfAsyGauss->GetRandom();
        //cout << "Bin number x = "<< ii << ", y = " << jj << " : bin value = " << tempVal << ", bin error = " << tempErr << ", toy value = " << toyVal << ", relative difference = " << (toyVal-tempVal)/tempVal <<  endl;
        //toyVal = tfAsyGauss->GetRandom();
        //cout << "Bin number x = "<< ii << ", y = " << jj << " : bin value = " << tempVal << ", bin error = " << tempErr << ", toy value = " << toyVal << ", relative difference = " << (toyVal-tempVal)/tempVal << endl;
        //toyVal = tfAsyGauss->GetRandom();
        h2D_toy->SetBinContent(ii,jj,TMath::Abs(toyVal));
        h2D_toy->SetBinError(ii,jj,toyVal/tempVal*tempErr);
        //double shareVal = abs(toyVal-tempVal);
        //if(toyVal>tempVal){
        //    h2D_toy->SetBinContent(ii,jj+1,h2D_org->GetBinContent(ii,jj));
        //    h2D_toy->SetBinError(ii,jj,h2D_org->GetBinError(ii,jj));
        //}
        //delete tfAsyGauss;
      }
    }
  }
}
//
//TH1D** divideHist(TH1* horg, int nDivision){
//    int nSubBins = horg->GetNbinsX()/nDivision;
//    double subBinning[nSubBins+1];
//    for(int i=0;i<nSubBins;++i){
//        subBinning[i] = horg->GetBinLowEdge(i+1);
//    }
//    subBinning[nSubBins] = horg->GetBinLowEdge(nSubBins+1);
//
//    TH1D* hout[nDivision];
//    for(int i=0; i<nDivision; ++i){
//        hout[i] = new TH1D(Form("%s_divided_bin%d",horg->GetName(),i),Form("%s",horg->GetTitle()),nSubBins,subBinning);
//        for(int j=0;j<nSubBins;++j){
//            double content = horg->GetBinContent(j+i*nSubBins+1);
//            double error = horg->GetBinError(j+i*nSubBins+1);
//            hout[i]->SetBinContent(j+1,content);
//            hout[i]->SetBinError(j+1,content);
//        }
//    }
//    return hout;
//}

float getHistHighXvalue(TH1* h){
  float xmax = 0; 
  for(int ix = 0; ix < h->GetNbinsX(); ++ix){
    if(h->GetBinContent(ix+1) !=0) xmax = h->GetBinLowEdge(ix+1) + h->GetBinWidth(ix+1);
  }
  return xmax;
}

float getHistLowXvalue(TH1* h){
  float xmin = 0; 
  for(int ix = h->GetNbinsX(); ix > 0; --ix){
    if(h->GetBinContent(ix+1) !=0) xmin = h->GetBinLowEdge(ix);
  }
  return xmin;
}

void histContentErrorSum(TH1* h){
  for(int ix = 0; ix < h->GetNbinsX(); ++ix){
    h->GetBinContent(ix+1);
    h->GetBinError(ix+1);
  }
}

void PrintHistContent(TH1* h){
  cout << " All bin contents of hist = " << h->GetName() << endl;
  string printSt = "";
  for(int ix = 0; ix < h->GetNbinsX(); ++ix){
    if(ix == h->GetNbinsX()-1) printSt += std::to_string(h->GetBinContent(ix+1));
    else printSt += std::to_string(h->GetBinContent(ix+1)) + ", ";
  }
  cout << printSt << endl;
} 

void SplitFilename (const std::string& str, std::string& path_str, std::string& file_str)
{
  std::cout << "Splitting: " << str << '\n';
  std::size_t found = str.find_last_of("/\\");
  path_str = str.substr(0,found);
  file_str = str.substr(found+1);
  std::cout << " path: " << path_str << '\n';
  std::cout << " file: " << file_str << '\n';
}

TH1D* removeHistEdgeBins(TH1* h1, int n1=1, int n2=1) {//n1 for the first bins n2 for the last bins being removed
  int nBins_org = h1->GetNbinsX();
  int nBins_new = nBins_org - n1 - n2; 
  //cout << "original and new nBins = " << nBins_org << ", " << nBins_new << endl;
  TString hname_org = h1->GetName();
  TString htitle_org = h1->GetTitle();
  double newBinning[nBins_new+1];
  for(int i=0;i<nBins_new+1;++i){
    newBinning[i] = h1->GetBinLowEdge(i+1+n1);
    //cout << "newBinning of ipt" << i << " = " << newBinning[i] << endl; 
  }

  h1->SetName(Form("%s_beforeRebin", hname_org.Data()));
  TH1D* htemp = new TH1D(Form("%s",hname_org.Data()),Form("%s",htitle_org.Data()),nBins_new,newBinning);
  for(int i=1;i<=nBins_new;++i){
    htemp->SetBinContent(i,h1->GetBinContent(i+n1));
    htemp->SetBinError(i,h1->GetBinError(i+n1));
  }

  //cout << "original hist content" << endl;
  //PrintHistContent(h1);
  //cout << "new hist content" << endl;
  //PrintHistContent(htemp);
  return htemp;
}
//TH1D* removeHistEdgeBins(TH1D* h){
//    int nbins = h->GetNbinsX();
//    double newBinning[nbins-2+1];
//    for(int i=0;i<nbins;++i){
//       newBinning[i] = h->GetBinLowEdge(i+2);
//    }
//
//    TH1D* htemp = new TH1D(Form("%s_removedUnderAndOverFlowBins",h->GetName()),"",nbins-2,newBinning);
//    for(int i=0;i<nbins-2;++i){
//        htemp->SetBinContent(i+1,h->GetBinContent(i+2));
//        htemp->SetBinError(i+1,h->GetBinError(i+2));
//    }
//    return htemp;
//}

double divisionError(double x1, double x2, double err1, double err2) {
  double y = x1/x2;
  double tempErr1 = (err1/x1)*(err1/x1);
  double tempErr2 = (err2/x2)*(err2/x2);
  return abs(y)*TMath::Sqrt(tempErr1 + tempErr2);
}

string float2string(double f)
{
  std::stringstream ss;

  ss << std::fixed << std::setprecision(10) << f;   // 122 is LARGE, but you may find that really tiny or really large numbers still don't work out... 

  std::string s = ss.str();

  std::string::size_type len = s.length();

  int zeros = 0;
  while(len > 1 && s[--len] == '0')
    zeros++;
  if (s[len] == '.')  // remove final '.' if number ends with '.'
    zeros++;
  s.resize(s.length()-zeros);


  return s;
}

/////////////////////////////////////////////////////////////////////
// Calculate Matrix for h2D genPt vs. recoPt correlation with percent : Response matrix
TH2D* getPercentResponseMatrix_genPtInX(TH2* h2){
  TH2D* h2_per = (TH2D*) h2->Clone(Form("%s_percent", h2->GetName())); 
  for(int xpt=1; xpt<=h2->GetNbinsX(); ++xpt){
    double totEvtInAbin = 0.0;
    for(int ypt=1; ypt<=h2->GetNbinsY(); ++ypt)
      totEvtInAbin += h2->GetBinContent(xpt,ypt);

    for(int ypt=1; ypt<=h2->GetNbinsY(); ++ypt){
      double evtInAbin = h2->GetBinContent(xpt,ypt);
      double percent = 0;
      percent = (double)evtInAbin/(double)totEvtInAbin;
      h2_per->SetBinContent(xpt,ypt,percent);
    }//x loop
  }//y loop
  return h2_per;
}

/////////////////////////////////////////////////////////////////////
// Calculate Matrix for h2D genPt vs. recoPt correlation with percent : Response matrix
TH2D* getPercentResponseMatrix_genPtInY(TH2* h2){
  TH2D* h2_per = (TH2D*) h2->Clone(Form("%s_percent", h2->GetName())); 
  for(int ypt=1; ypt<=h2->GetNbinsY(); ++ypt){
    double totEvtInAbin = 0.0;
    for(int xpt=1; xpt<=h2->GetNbinsX(); ++xpt)
      totEvtInAbin += h2->GetBinContent(xpt,ypt);

    for(int xpt=1; xpt<=h2->GetNbinsX(); ++xpt){
      double evtInAbin = h2->GetBinContent(xpt,ypt);
      double percent = 0;
      percent = (double)evtInAbin/(double)totEvtInAbin;
      h2_per->SetBinContent(xpt,ypt,percent);
    }//x loop
  }//y loop
  return h2_per;
}

// below is not complete
//TH1F* reBinHist(TH1* h, float reBinArray[], int ArraySize){
//    TH1D* h_org = (TH1D*) h->Clone(Form("%s_org",h->GetName()));
//    TH1D* h_xshifted = (TH1D*) h->Clone(Form("%s_xshifted",h->GetName()));
//    int TOTNBINS = h_org->GetNbinsX();
//    for(int ibin=1; ibin<TOTNBINS; ++ibin){
//        double xVal = h_org->GetBinCenter(ibin);
//        int shiftBin = h_org->FindBin(xVal-shiftVal);
//        if(shiftBin<1) continue;
//        double tempVal = h_org->GetBinContent(shiftBin);
//        double tempErr = h_org->GetBinError(shiftBin);
//        h_xshifted->SetBinContent(ibin,tempVal);
//        h_xshifted->SetBinError(ibin,tempErr);
//        if(ibin < 100) cout << "ibin = " << ibin << ", xVal = " << xVal << ", shiftBin = " << shiftBin << ", h_org_binVal = " << h_org->GetBinContent(ibin) << ", h_xshifted_val = " << tempVal << endl;
//    }
//    return h_xshifted;
//}
TH1D* xShiftHist(TH1* h, double shiftVal){
  ////////////////////////////////////////////////////////////////
  // X-shift
  TH1D* h_org = (TH1D*) h->Clone(Form("%s_org",h->GetName()));
  TH1D* h_xshifted = (TH1D*) h->Clone(Form("%s_xshifted",h->GetName()));
  int TOTNBINS = h_org->GetNbinsX();
  for(int ibin=1; ibin<TOTNBINS; ++ibin){
    double xVal = h_org->GetBinCenter(ibin);
    int shiftBin = h_org->FindBin(xVal-shiftVal);
    if(shiftBin<1) continue;
    double tempVal = h_org->GetBinContent(shiftBin);
    double tempErr = h_org->GetBinError(shiftBin);
    h_xshifted->SetBinContent(ibin,tempVal);
    h_xshifted->SetBinError(ibin,tempErr);
    if(ibin < 100) cout << "ibin = " << ibin << ", xVal = " << xVal << ", shiftBin = " << shiftBin << ", h_org_binVal = " << h_org->GetBinContent(ibin) << ", h_xshifted_val = " << tempVal << endl;
  }
  return h_xshifted;
}

double chi2(TH1* h1, TH1* h2, double rMin=-1, double rMax=-1){
  double c = 0;
  int ndof = 0;
  double tempBinMin = 1;
  double tempBinMax = h1->GetNbinsX()+1;

  if(rMin!=-1) tempBinMin = h1->GetXaxis()->FindBin(rMin);
  if(rMax!=-1) tempBinMax = h1->GetXaxis()->FindBin(rMax);
  for(int i = tempBinMin; i < tempBinMax; ++i){
    double y1 = h1->GetBinContent(i);
    double y2 = h2->GetBinContent(i);
    double e1 = h1->GetBinError(i);
    double e2 = h2->GetBinError(i);

    double dy = y1-y2;
    double de2 = e1*e1+e2*e2;
    if(de2 > 0) {
      c += dy*dy/de2;
      ndof += 1;
    }
  }

  //return c;
  return c/ndof;
}

TH1D* getRatioErrorHist_completelyCorrelated(TH1D* hratio, TH1D* hA, TH1D* hB){
  TH1D* htemp = (TH1D*) hA->Clone(Form("%s_error",hA->GetName())); 
  cout << ">>>>>>>>>> Analyzing " << hA->GetName() << endl;
  for(int ip=1; ip<=hA->GetNbinsX(); ++ip){ 
    double EA, EB, sigA, sigB, error;
    EA=hA->GetBinContent(ip);
    EB=hB->GetBinContent(ip);
    sigA=hA->GetBinError(ip);
    sigB=hB->GetBinError(ip);
    double a= sigA*sigA/(EA*EA);
    double b= sigB*sigB/(EB*EB);
    double c= 2./(EA*EB)*sigA*sigA;
    //double sqrtPart = TMath::Sqrt(abs(a + b));
    double sqrtPart = TMath::Sqrt(abs(a + b - c));
    //double sqrtPart = TMath::Sqrt(sigA*sigA/(EA*EA) + sigB*sigB/(EB*EB) - 2./(EA*EB)*sigA*sigA);
    error = EA/EB*sqrtPart;
    cout << "ptbin = " << ip << ", sigA = " << sigA << ", sigB = " << sigB <<", a= "<< a <<", b= "<< b <<", c= "<< c<< ", sqrt part = " << sqrtPart << ", error = " << error << endl; 
    //cout << "ptbin = " << ip << ", sigA = " << sigA << ", sigB = " << sigB <<", sqrt part = " << sqrtPart << ", error = " << error << endl; 
    htemp->SetBinContent(ip,error);

    hratio->SetBinError(ip,error);
  }
  return htemp;
}
void removeHistLastBins(TH1D* h, int nRej){
  int nbins = h->GetNbinsX();
  for(int i=nbins-nRej;i<nbins;++i){
    h->SetBinContent(i+1,0.);
    h->SetBinError(i+1,0.);
  }
}

TH1D* removeHistFirstAndLastBins(TH1D* h){
  int nbins = h->GetNbinsX();
  double newBinning[nbins-2+1];
  for(int i=0;i<nbins;++i){
    newBinning[i] = h->GetBinLowEdge(i+2);
  }

  TH1D* htemp = new TH1D(Form("%s_removedUnderAndOverFlowBins",h->GetName()),"",nbins-2,newBinning);
  for(int i=0;i<nbins-2;++i){
    htemp->SetBinContent(i+1,h->GetBinContent(i+2));
    htemp->SetBinError(i+1,h->GetBinError(i+2));
  }
  return htemp;
}
void removeHistError(TH1D* h){
  int nbins = h->GetNbinsX();
  for(int i=0;i<nbins;++i){
    h->SetBinError(i+1,0.);
  }
}
void setGraphStyle(TGraphAsymmErrors* g, int color = kBlack, int marker = 20, float markerSize = 1.0){
  g->SetMarkerColor(color);
  g->SetLineColor(color);
  g->SetMarkerStyle(marker);
  g->SetMarkerSize(markerSize);
}
void setHistStyle(TH1* h, int color = kBlack, int marker = 20, float markerSize = 1.0){
  h->SetMarkerColor(color);
  h->SetLineColor(color);
  h->SetMarkerStyle(marker);
  h->SetMarkerSize(markerSize);
}
void setHistMarkerAndLineStyle(TH1* h, int color = kBlack, int markerStyle = 20, float markerSize = 1.0, float lineStyle = 1, float lineSize = 1.0){
  h->SetMarkerColor(color);
  h->SetMarkerStyle(markerStyle);
  h->SetMarkerSize(markerSize);
  h->SetLineColor(color);
  h->SetLineStyle(lineStyle);
  h->SetLineWidth(lineSize);
}
void setHistLineStyle(TH1* h, int color = kBlack, float lineStyle = 1, float lineSize = 1.0){
  h->SetLineColor(color);
  h->SetLineStyle(lineStyle);
  h->SetLineWidth(lineSize);
}
void setHistMarkerStyle(TH1* h, int color = kBlack, int markerStyle = 20, float markerSize = 1.0){
  h->SetMarkerColor(color);
  h->SetMarkerStyle(markerStyle);
  h->SetMarkerSize(markerSize);
  h->SetLineColor(color);
}
void settdrStyleHist(TH1D* h, float xoffset=1.5, float yoffset=1.8){
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetTitleOffset(yoffset);
  h->GetXaxis()->SetTitleOffset(xoffset);
  h->GetXaxis()->SetTitleFont(42);
  h->GetYaxis()->SetTitleFont(42);
  h->GetXaxis()->SetTitleColor(1);
  h->GetYaxis()->SetTitleColor(1);
  h->GetXaxis()->SetLabelSize(0.04);
}

void drawLumi(TCanvas* c, TString lumiSt,double lumiTextOffset=0.2, double lumiTextSize = 0.525){
  ////float ll = gPad->GetLeftMargin();
  ////float tt = gPad->GetTopMargin();
  ////float rr = gPad->GetRightMargin();
  ////float bb = gPad->GetBottomMargin();
  //float ll = c->GetLeftMargin();
  float tt = c->GetTopMargin();
  float rr = c->GetRightMargin();
  //float bb = c->GetBottomMargin();
  //cout << ll << ", " << tt << ", " << rr << ", " << bb << endl;
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);
  latex.SetTextFont(42);
  latex.SetTextAlign(31);
  latex.SetTextSize(lumiTextSize*tt);
  latex.DrawLatex(1-rr,1-tt+lumiTextOffset*tt,Form("%s",lumiSt.Data()));
}

void drawCMS(TCanvas* c, TString extraSt, double cmsTextOffset=0.2){
  //float ll = gPad->GetLeftMargin();
  //float tt = gPad->GetTopMargin();
  //float rr = gPad->GetRightMargin();
  //float bb = gPad->GetBottomMargin();
  float ll = c->GetLeftMargin();
  float tt = c->GetTopMargin();
  //float rr = c->GetRightMargin();
  //float bb = c->GetBottomMargin();
  //cout << ll << ", " << tt << ", " << rr << ", " << bb << endl;
  double cmsTextSize =0.75;
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);
  latex.SetTextFont(62);
  latex.SetTextAlign(13);
  //latex.SetTextSize(0.025);
  latex.SetTextSize(cmsTextSize*tt);
  latex.DrawLatex(ll+cmsTextOffset*tt,1-tt-cmsTextOffset*tt,"CMS");

  float extraOverCmsTextSize  = 0.76;
  float relExtraDY = 1.2;
  TLatex latex_ex;
  latex_ex.SetNDC();
  latex_ex.SetTextAngle(0);
  latex_ex.SetTextColor(kBlack);
  latex_ex.SetTextFont(52);
  latex_ex.SetTextAlign(13);
  //latex_ex.SetTextSize(0.025);
  latex_ex.SetTextSize(cmsTextSize*tt*extraOverCmsTextSize);
  latex_ex.DrawLatex(ll+cmsTextOffset*tt,1-tt-cmsTextOffset*tt-relExtraDY*cmsTextSize*tt,Form("%s",extraSt.Data()));

}

void yjStyleRoot(){
  gStyle -> SetOptStat(0);
  TH1::SetDefaultSumw2();
}
void SetyjPadStyle(){
  gStyle->SetPaperSize(20,26);
  gStyle->SetPadTopMargin(0.07);
  gStyle->SetPadRightMargin(0.07);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
}
void SetHistTitleStyle(double titlesize=0.06, double labelsize = 0.05){
  gStyle->SetTextFont(42); 
  gStyle->SetTextSize(0.04); 
  gStyle->SetLabelFont( 42, "X" ); 
  gStyle->SetLabelFont( 42, "Y" ); 
  gStyle->SetLabelFont( 42, "Z" ); 

  gStyle->SetTitleSize( titlesize, "X" ); 
  gStyle->SetTitleSize( titlesize, "Y" ); 
  gStyle->SetTitleSize( titlesize, "Z" ); 
  gStyle->SetLabelSize( labelsize, "X" ); 
  gStyle->SetLabelSize( labelsize, "Y" ); 
  gStyle->SetLabelSize( labelsize, "Z" ); 

}
//void SetHistTitleStyle(double titlesize=0.06, double titleoffset=0.04, double labelsize = 0.05, double labeloffset=0.01){
//    gStyle->SetTitleSize( titlesize, "X" ); gStyle->SetTitleOffset(titleoffset, "X");
//   gStyle->SetTitleSize( titlesize, "Y" ); gStyle->SetTitleOffset(titleoffset, "Y");
//  gStyle->SetLabelSize( labelsize, "X" ); gStyle->SetLabelOffset(labeloffset, "X");
// gStyle->SetLabelSize( labelsize, "Y" ); gStyle->SetLabelOffset(labeloffset, "Y");
//}
void SetHistTextSize(TH1* h, double divRatio=1.0, double titlesize=17, double labelsize=14, int fontst=43){
  double titleoffset;
  if(divRatio!=1.0) titleoffset = 2.4;
  else titleoffset = 1.28;
  double labeloffset = 0.01;
  h->GetXaxis()->SetLabelFont(fontst);
  h->GetYaxis()->SetLabelFont(fontst);
  h->GetXaxis()->SetTitleFont(fontst);
  h->GetYaxis()->SetTitleFont(fontst);
  //h->GetXaxis()->SetTitleFont(63);
  //h->GetYaxis()->SetTitleFont(63);

  h->GetXaxis()->SetLabelSize(labelsize); h->SetTitleOffset(titleoffset/divRatio, "X");
  h->GetYaxis()->SetLabelSize(labelsize); h->SetTitleOffset(titleoffset/divRatio, "Y");
  h->GetXaxis()->SetTitleSize(titlesize); h->SetLabelOffset(labeloffset/divRatio, "X");
  h->GetYaxis()->SetTitleSize(titlesize); h->SetLabelOffset(labeloffset/divRatio, "Y");
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->CenterTitle();
}
void SetHistTitleOffsetStyle(TH1* h, double titlesize=0.08, double titleoffset=0.01, double labelsize=0.05,double labeloffset=0.01){
  h->SetTitleSize( titlesize, "X" ); h->SetTitleOffset(titleoffset, "X");
  h->SetTitleSize( titlesize, "Y" ); h->SetTitleOffset(titleoffset, "Y");
  h->SetLabelSize( labelsize, "X" ); h->SetLabelOffset(labeloffset, "X");
  h->SetLabelSize( labelsize, "Y" ); h->SetLabelOffset(labeloffset, "Y");
}
void thisPadStyle(){
  gPad->SetLeftMargin(0.17);
  gPad->SetRightMargin(0.08);
  gPad->SetBottomMargin(0.15);
  gPad->SetTopMargin(0.05);
}
void SetPadStyle(){
  gStyle->SetPadLeftMargin(0.14);
  gStyle->SetPadRightMargin(0.08);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadTopMargin(0.10);
}
void legStyle( TLegend *a, float margin = 0.17)
{
  a->SetBorderSize(0);
  a->SetFillStyle(0);
  a->SetTextFont(42);
  a->SetMargin(margin);
  //a->SetTextFont(63);
  //  a->SetHeader(head);
  //  a->SetTextSize(0.04);
  //  a->SetLineColor(1);
  //  a->SetLineStyle(1);
  //  a->SetLineWidth(1);
  //  a->SetFillColor(0);

}
void graphStyle(TGraph *g1=0, Int_t lstyle=1, Int_t lcolor=1, Int_t mstyle=20, Int_t mcolor=1, Int_t lwidth=1, Double_t msize=1.0)
{
  g1->SetLineStyle(lstyle);
  g1->SetLineColor(lcolor);
  g1->SetLineWidth(lwidth);
  g1->SetMarkerStyle(mstyle);
  g1->SetMarkerColor(mcolor);
  g1->SetMarkerSize(msize);
}

void hLineStyle(TH1 *h1=0, Int_t lstyle=1, Int_t lcolor=1, Int_t lwidth=1, Int_t lfst=0, Int_t lfcolor=0)
{
  h1->SetLineStyle(lstyle);
  h1->SetLineColor(lcolor);
  h1->SetLineWidth(lwidth);
  h1->SetFillStyle(lfst);
  h1->SetFillColor(lfcolor);
}

void hMarkerStyle(TH1 *h1=0, Int_t mstyle=20, Int_t mcolor=1, Double_t msize=1.0)
{
  h1->SetMarkerStyle(mstyle);
  h1->SetMarkerColor(mcolor);
  h1->SetMarkerSize(msize);
}
void drawATLAS(float xp, float yp, bool isRightAlign=0, double textSize=18, int textFont = 73){
  TLatex* latexATLAS = new TLatex();
  latexATLAS->SetTextFont(textFont);
  latexATLAS->SetTextSize(textSize);
  if(isRightAlign) latexATLAS->SetTextAlign(31);
  latexATLAS->DrawLatexNDC(xp, yp, "ATLAS");
}
void drawInternal(float xp, float yp, bool isRightAlign=0, double textSize=17, int textFont = 43){
  TLatex* latexInternal = new TLatex();
  latexInternal->SetTextFont(textFont);
  latexInternal->SetTextSize(textSize);
  if(isRightAlign) latexInternal->SetTextAlign(31);
  latexInternal->DrawLatexNDC(xp, yp, "Internal");
}
void drawPreliminary(float xp, float yp, bool isRightAlign=0, double textSize=17, int textFont = 43){
  TLatex* latexInternal = new TLatex();
  latexInternal->SetTextFont(textFont);
  latexInternal->SetTextSize(textSize);
  if(isRightAlign) latexInternal->SetTextAlign(31);
  latexInternal->DrawLatexNDC(xp, yp, "Preliminary");
}
void drawATLASInternal(float xp, float yp, double textSize=18, bool isRightAlign=0){
  TLatex* latexATLAS = new TLatex();
  latexATLAS->SetTextFont(43);
  latexATLAS->SetTextSize(textSize);
  if(isRightAlign) latexATLAS->SetTextAlign(31);
  //latexATLAS->DrawLatexNDC(xp, yp, "#font[73]{ATLAS} #bf{#bf{Internal}}");
  //latexATLAS->DrawLatexNDC(xp, yp, "#font[73]{ATLAS} #font[43]{#font[43]{Internal}}");
  latexATLAS->DrawLatexNDC(xp, yp, "#bf{#it{ATLAS}} Internal");
  //latexATLAS->DrawLatexNDC(xp, yp, "#font[73]{ATLAS} Internal");
  //latexATLAS->DrawLatexNDC(xp, yp, "#font[73]{ATLAS} #font[43]{Internal}");
}
void drawText(const char *text, float xp, float yp, bool isRightAlign=0, int textColor=kBlack, double textSize=0.04, int textFont = 42, bool isNDC=true){
  // when textfont 42, textSize=0.04
  // when textfont 43, textSize=18
  TLatex *tex = new TLatex(xp,yp,text);
  tex->SetTextFont(textFont);
  //   if(bold)tex->SetTextFont(43);
  tex->SetTextSize(textSize);
  tex->SetTextColor(textColor);
  tex->SetLineWidth(1);
  if(isNDC) tex->SetNDC();
  if(isRightAlign) tex->SetTextAlign(31);
  tex->Draw();
}
void jumSun(Double_t x1=0,Double_t y1=0,Double_t x2=1,Double_t y2=1,Int_t color=1, Double_t width=1)
{
  TLine* t1 = new TLine(x1,y1,x2,y2);
  t1->SetLineWidth(width);
  t1->SetLineStyle(7);
  t1->SetLineColor(color);
  t1->Draw();
}

void onSun(Double_t x1=0,Double_t y1=0,Double_t x2=1,Double_t y2=1,Int_t color=1, Double_t width=1)
{
  TLine* t1 = new TLine(x1,y1,x2,y2);
  t1->SetLineWidth(width);
  t1->SetLineStyle(1);
  t1->SetLineColor(color);
  t1->Draw();
}
double findCross(TH1* h1, TH1* h2, double& frac, double& effi, double& fracErr, double& effiErr){
  Int_t nBins = h1->GetNbinsX();
  double crossVal =0;
  int binAt0 = h1->FindBin(0);
  for(Int_t ix=binAt0; ix<=nBins ;ix++){
    float yy1 = h1->GetBinContent(ix);
    float yy2 = h2->GetBinContent(ix);
    if(yy2>yy1) {
      crossVal= h1->GetBinLowEdge(ix);
      break;
    }
  }
  int crossBin = h1->FindBin(crossVal);
  frac = 1 - (h2->Integral(1,crossBin) / h1->Integral(1,crossBin) );
  effi = ( h1->Integral(1,crossBin) / h1->Integral() );
  fracErr = frac * TMath::Sqrt( (1./h2->Integral(1,crossVal)) + (1./h1->Integral(1,crossVal)) );
  effiErr = ( TMath::Sqrt(h1->Integral(1,crossVal)) / h1->Integral() ) * TMath::Sqrt(1 - (h1->Integral(1,crossVal)/h1->Integral()) );

  return crossVal;
}
void ratioPanelCanvas2(TCanvas*& canv, 
    const Float_t divRatio=0.4,
    //const Float_t leftOffset=0.,
    //const Float_t bottomOffset=0.,
    const Float_t leftMargin=0.17,
    const Float_t bottomMargin=0.3,
    const Float_t edge=0.05) {
  if (canv==0) {
    //Error("makeMultiPanelCanvas","Got null canvas.");
    return;
  }
  canv->Clear();


  TPad* pad1 = new TPad("pad1","",0.0,divRatio,1.0,1.0);
  canv->cd();
  pad1->SetLeftMargin(leftMargin);
  pad1->SetRightMargin(edge);
  pad1->SetTopMargin(edge*2);
  pad1->SetBottomMargin(0.0);
  //pad1->SetBottomMargin(0.005);
  pad1->Draw();
  pad1->cd();
  pad1->SetNumber(1);

  TPad* pad2 = new TPad("pad2","",0.0,0.0,1.0,divRatio);
  canv->cd();
  pad2->SetLeftMargin(leftMargin);
  pad2->SetRightMargin(edge);
  pad2->SetTopMargin(0.0);
  //pad2->SetTopMargin(0.005);
  //pad2->SetTopMargin(edge);
  pad2->SetBottomMargin(bottomMargin);
  pad2->Draw();
  pad2->cd();
  pad2->SetNumber(2);
}

void ratioPanelCanvas(TCanvas*& canv, 
    const Float_t divRatio=0.4,
    //const Float_t leftOffset=0.,
    //const Float_t bottomOffset=0.,
    const Float_t leftMargin=0.17,
    const Float_t bottomMargin=0.3,
    const Float_t edge=0.05) {
  if (canv==0) {
    return;
  }
  canv->Clear();


  TPad* pad1 = new TPad("pad1","",0.0,divRatio,1.0,1.0);
  canv->cd();
  pad1->SetLeftMargin(leftMargin);
  pad1->SetRightMargin(edge);
  pad1->SetTopMargin(edge*2);
  pad1->SetBottomMargin(0);
  pad1->Draw();
  pad1->cd();
  pad1->SetNumber(1);

  TPad* pad2 = new TPad("pad2","",0.0,0.0,1.0,divRatio);
  canv->cd();
  pad2->SetLeftMargin(leftMargin);
  pad2->SetRightMargin(edge);
  pad2->SetTopMargin(0);
  pad2->SetBottomMargin(bottomMargin);
  pad2->Draw();
  pad2->cd();
  pad2->SetNumber(2);
}

void triplePanelCanvas(TCanvas*& canv, 
    const Float_t divRatio=0.50,
    const Float_t divRatio2=0.275,
    //const Float_t leftOffset=0.,
    //const Float_t bottomOffset=0.,
    const Float_t leftMargin=0.17,
    const Float_t bottomMargin=0.3,
    const Float_t edge=0.05) {
  if (canv==0) {
    //Error("makeMultiPanelCanvas","Got null canvas.");
    return;
  }
  canv->Clear();

  TPad* pad1 = new TPad("pad1","",0.0,divRatio,1.0,1.0);
  canv->cd();
  pad1->SetLeftMargin(leftMargin);
  pad1->SetRightMargin(edge);
  pad1->SetTopMargin(edge*2);
  pad1->SetBottomMargin(0);
  pad1->Draw();
  pad1->cd();
  pad1->SetNumber(1);

  TPad* pad2 = new TPad("pad2","",0.0,divRatio2,1.0,divRatio);
  canv->cd();
  pad2->SetLeftMargin(leftMargin);
  pad2->SetRightMargin(edge);
  pad2->SetTopMargin(0);
  pad2->SetBottomMargin(0);
  pad2->Draw();
  pad2->cd();
  pad2->SetNumber(2);

  TPad* pad3 = new TPad("pad3","",0.0,0.0,1.0,divRatio2);
  canv->cd();
  pad3->SetLeftMargin(leftMargin);
  pad3->SetRightMargin(edge);
  pad3->SetTopMargin(0);
  pad3->SetBottomMargin(bottomMargin);
  pad3->Draw();
  pad3->cd();
  pad3->SetNumber(3);
}

void makeMultiPanelCanvas(TCanvas*& canv, const Int_t columns,
    const Int_t rows, 
    const Float_t leftOffset=0.,
    const Float_t bottomOffset=0.,
    const Float_t leftMargin=0.2,
    const Float_t bottomMargin=0.2,
    const Float_t edge=0.05,
    const Float_t edge_bottom=0.,
    const Float_t edge_top=0.) {
  if (canv==0) {
    //Error("makeMultiPanelCanvas","Got null canvas.");
    return;
  }
  canv->Clear();

  TPad* pad[columns][rows];

  Float_t Xlow[columns];
  Float_t Xup[columns];
  Float_t Ylow[rows];
  Float_t Yup[rows];
  Float_t PadWidth =
    (1.0-leftOffset)/((1.0/(1.0-leftMargin)) +
        (1.0/(1.0-edge))+(Float_t)columns-2.0);
  Float_t PadHeight =
    (1.0-bottomOffset)/((1.0/(1.0-bottomMargin)) +
        (1.0/(1.0-edge))+(Float_t)rows-2.0);
  Xlow[0] = leftOffset;
  // Xlow[0] = 0;
  Xup[0] = leftOffset + PadWidth/(1.0-leftOffset);
  //Xup[0] = leftOffset + PadWidth/(1.0-leftOffset);
  //Xup[0] = leftOffset + PadWidth/(1.0-leftMargin);
  Xup[columns-1] = 1;
  Xlow[columns-1] = 1.0-PadWidth/(1.0-edge);

  Yup[0] = 1;
  Ylow[0] = 1.0-PadHeight/(1.0-edge);
  Ylow[rows-1] = bottomOffset;
  Yup[rows-1] = bottomOffset + PadHeight/(1.0-bottomMargin);

  for(Int_t i=1;i<columns-1;i++) {
    Xlow[i] = Xup[0] + (i-1)*PadWidth;
    Xup[i] = Xup[0] + (i)*PadWidth;
  }
  Int_t ct = 0;
  for(Int_t i=rows-2;i>0;i--) {
    Ylow[i] = Yup[rows-1] + ct*PadHeight;
    Yup[i] = Yup[rows-1] + (ct+1)*PadHeight;
    ct++;
  }
  TString padName;
  for(Int_t i=0;i<columns;i++) {
    for(Int_t j=0;j<rows;j++) {
      canv->cd();
      padName = Form("p_%d_%d",i,j);
      pad[i][j] = new TPad(padName.Data(),padName.Data(),
          Xlow[i],Ylow[j],Xup[i],Yup[j]);
      // if(i==0) pad[i][j]->SetLeftMargin(-leftMargin+edge);
      if(i==0) pad[i][j]->SetLeftMargin(edge);
      else pad[i][j]->SetLeftMargin(edge);
      //else pad[i][j]->SetLeftMargin(0);
      //else pad[i][j]->SetLeftMargin(0.03);

      if(i==(columns-1)) pad[i][j]->SetRightMargin(edge);
      else pad[i][j]->SetRightMargin(0);
      //else pad[i][j]->SetRightMargin(edge);
      // else pad[i][j]->SetRightMargin(0);
      //else pad[i][j]->SetRightMargin(0.03);

      // if(j==0) pad[i][j]->SetTopMargin(edge_top);
      if(j==0) pad[i][j]->SetTopMargin(bottomMargin+edge_top);
      else pad[i][j]->SetTopMargin(edge_top);
      //else pad[i][j]->SetTopMargin(edge_top);
      //else pad[i][j]->SetTopMargin(0.03);

      if(j==(rows-1)) pad[i][j]->SetBottomMargin(bottomMargin+edge_bottom);
      else pad[i][j]->SetBottomMargin(edge_bottom);
      //else pad[i][j]->SetBottomMargin(0);

      //pad[i][j]->SetFrameFillStyle(4000);
      pad[i][j]->SetFillColor(0);
      pad[i][j]->SetFillStyle(0);
      pad[i][j]->Draw();
      pad[i][j]->cd();
      pad[i][j]->SetNumber(columns*j+i+1);

    }
  }
}

Double_t getDPHI( Double_t phi1, Double_t phi2) {
  Double_t dphi = phi1 - phi2;

  //3.141592653589
  if ( dphi > TMath::Pi() )
    dphi = dphi - 2. * TMath::Pi();
  if ( dphi <= -TMath::Pi() )
    dphi = dphi + 2. * TMath::Pi();

  if ( TMath::Abs(dphi) > TMath::Pi() ) {
    std::cout << " commonUtility::getDPHI error!!! dphi is bigger than TMath::Pi() " << std::endl;
    std::cout << " " << phi1 << ", " << phi2 << ", " << dphi << std::endl;
  }

  return dphi;
}

Double_t getDETA(Double_t eta1, Double_t eta2){
  return eta1 - eta2;
}

Double_t getDR( Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2){
  Double_t theDphi = getDPHI( phi1, phi2);
  Double_t theDeta = eta1 - eta2;
  return TMath::Sqrt ( theDphi*theDphi + theDeta*theDeta);
}

Double_t cleverRange(TH1* h,Float_t fac=1.2, Float_t minY=1.e-3)
{
  Float_t maxY =  fac * h->GetBinContent(h->GetMaximumBin());
  //   cout <<" range will be set as " << minY << " ~ " << maxY << endl;
  h->SetAxisRange(minY,maxY,"Y");
  return maxY;
}

void cleverRange(TH1* h,TH1* h2, double ymin, double ymax, Float_t fac=5.0)
{
  Float_t maxY1 =  fac * h->GetBinContent(h->GetMaximumBin());
  Float_t maxY2 =  fac * h2->GetBinContent(h2->GetMaximumBin());
  Float_t minY1 =  1/fac * h->GetBinContent(h->GetMinimumBin());
  Float_t minY2 =  1/fac * h2->GetBinContent(h2->GetMinimumBin());

  ymin = min(minY1,minY2);
  ymax = max(maxY1,maxY2);
  h->SetAxisRange(ymin,ymax,"Y");
  h2->SetAxisRange(ymin,ymax,"Y");
}


Double_t getHistMinForLogy(TH1* h, int bin_i = 0, int bin_f = 0)
{
  Double_t minY = 1000000;
  for ( Int_t ibin = 1+bin_i ; ibin <= h->GetNbinsX()-bin_f ; ibin++) {
    if(h->GetBinContent(ibin)<=0) continue;
    if (minY > h->GetBinContent(ibin) )
      minY = h->GetBinContent(ibin);
  }
  return minY;
}

Double_t getCleverRange(TH1* h, int bin_i = 0, int bin_f = 0)
{
  Double_t maxY = -1000000;
  for ( Int_t ibin = 1+bin_i ; ibin <= h->GetNbinsX()-bin_f ; ibin++) {
    if (maxY < h->GetBinContent(ibin) )
      maxY = h->GetBinContent(ibin);
  }
  return maxY;
}
Double_t getCleverRangeMin(TH1* h)
{
  Double_t minY = 100000000000000;
  for ( Int_t ibin = 1 ; ibin <= h->GetNbinsX() ; ibin++) {
    if (minY > h->GetBinContent(ibin) )
      minY = h->GetBinContent(ibin);
  }
  return minY;
}

Double_t cleverRangeOnlyMax(TH1* h,TH1* h2, Float_t fac=1.2, Float_t minY=1.e-3)
{
  Float_t maxY1 =  fac * h->GetBinContent(h->GetMaximumBin());
  Float_t maxY2 =  fac * h2->GetBinContent(h2->GetMaximumBin());

  //   cout <<" range will be set as " << minY << " ~ " << maxY << endl;
  h->SetAxisRange(minY,max(maxY1,maxY2),"Y");
  h2->SetAxisRange(minY,max(maxY1,maxY2),"Y");
  return max(maxY1,maxY2);
}

Double_t cleverRange(TH1* h,TH1* h2, Float_t fac=1.2)
{
  Float_t maxY1 =  fac * h->GetBinContent(h->GetMaximumBin());
  Float_t maxY2 =  fac * h2->GetBinContent(h2->GetMaximumBin());

  Float_t minY1 =  (2.0-fac) * h->GetBinContent(h->GetMinimumBin());
  Float_t minY2 =  (2.0-fac) * h2->GetBinContent(h2->GetMinimumBin());
  //cout <<" range will be set as " << minY1 << " ~ " << minY2 << endl;
  //   cout <<" range will be set as " << minY << " ~ " << maxY << endl;
  h->SetAxisRange(min(minY1,minY2),max(maxY1,maxY2),"Y");
  h2->SetAxisRange(min(minY1,minY2),max(maxY1,maxY2),"Y");
  return min(minY1,minY2);
  //return max(maxY1,maxY2);
}

Double_t cleverRange(TH1* h,TH1* h2, TH1* h3, Float_t fac=1.2)
{
  Float_t maxY1 =  fac * h->GetBinContent(h->GetMaximumBin());
  Float_t maxY2 =  fac * h2->GetBinContent(h2->GetMaximumBin());
  Float_t maxY3 =  fac * h3->GetBinContent(h3->GetMaximumBin());

  Float_t minY1 =  (2.0-fac) * h->GetBinContent(h->GetMinimumBin());
  Float_t minY2 =  (2.0-fac) * h2->GetBinContent(h2->GetMinimumBin());
  Float_t minY3 =  (2.0-fac) * h3->GetBinContent(h3->GetMinimumBin());
  //   cout <<" range will be set as " << minY << " ~ " << maxY << endl;
  Float_t firstmin = min(minY1,minY2);
  Float_t firstmax = max(maxY1,maxY2);
  Float_t finalmin = min(firstmin,minY3);
  Float_t finalmax = max(firstmax,maxY3);
  h->SetAxisRange(finalmin,finalmax,"Y");
  h2->SetAxisRange(finalmin,finalmax,"Y");
  h3->SetAxisRange(finalmin,finalmax,"Y");
  return finalmin; 
}

TF1* cleverGaus(TH1* h, const char* title="h", Float_t c = 2.5, bool quietMode=true)
{
  if ( h->GetEntries() == 0 )
  {
    TF1 *fit0 = new TF1(title,"gaus",-1,1);
    fit0->SetParameters(0,0,0);
    return fit0;
  }

  Int_t peakBin  = h->GetMaximumBin();
  Double_t peak =  h->GetBinCenter(peakBin);
  Double_t sigma = h->GetRMS();

  TF1 *fit1 = new TF1(title,"gaus",peak-c*sigma,peak+c*sigma);
  fit1->SetParameter(1, peak);
  //fit1->SetParameter(1, 0.0005);
  //if (quietMode) h->Fit(fit1,"Q R");
  if (quietMode) h->Fit(fit1,"LL M O Q R");
  else    h->Fit(fit1,"LL M O R");
  return fit1;
}

void SetHistColor(TH1* h, Int_t color=1)
{
  h->SetMarkerColor(color);
  h->SetLineColor(color);
} 

float mean(float data[], int n)
{
  float mean=0.0;
  int i;
  for(i=0; i<n;++i)
  {
    mean+=data[i];
  }
  mean=mean/n;
  return mean;
}

float standard_deviation(float data[], int n)
{
  float mean=0.0, sum_deviation=0.0;
  int i;
  for(i=0; i<n;++i)
  {
    mean+=data[i];
  }
  mean=mean/n;
  for(i=0; i<n;++i)
    sum_deviation+=(data[i]-mean)*(data[i]-mean);
  return sqrt(sum_deviation/n);
}

void normHist(TH1* h=0, TH1* hNominal=0, double cut_i=700, double cut_f=900){
  int cutBinFrom = h->FindBin(cut_i);
  int cutBinTo = h->FindBin(cut_f);
  h->Scale(hNominal->Integral(cutBinFrom,cutBinTo)/h->Integral(cutBinFrom,cutBinTo));
}

void saveHistogramsToPicture(TH1* h, const char* fileType="pdf", const char* caption="", const char* directoryToBeSavedIn="figures", const char* text = "", int styleIndex=1, int rebin =1){
  TCanvas* c1=new TCanvas();
  if(rebin!=1)
  {
    h->Rebin(rebin);
  }

  if(styleIndex==1)
  {
    h->Draw("E");
  }
  else
  {
    h->Draw();
    if(h->InheritsFrom("TH2"))
    {
      h->Draw("COLZ TEXT");    // default plot style for TH2 histograms
    }
  }
  drawText(text,0.7,0.7);
  if(strcmp(directoryToBeSavedIn, "") == 0)   // save in the current directory if no directory is specified
  {
    c1->SaveAs(Form("%s_%s.%s" ,h->GetName(),caption, fileType));  // name of the file is the name of the histogram
  }
  else
  {
    c1->SaveAs(Form("%s/%s_%s.%s", directoryToBeSavedIn ,h->GetName(),caption, fileType));
  }
  c1->Close();
}
std::string ReplaceString(std::string subject, const std::string& search,
    const std::string& replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}

void ReplaceStringInPlace(std::string& subject, const std::string& search,
    const std::string& replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

void drawTextCollision(TString coll, int cent_i, int cent_f, float xp, float yp, float dy=0.05, double fontSize=0.04, bool isRightAlign=1, int textFont = 42){
  //float dy = 0.05;
  if(coll=="PP"){
    drawText(Form("%s","PYTHIA"),xp,yp,isRightAlign,kBlack,fontSize, textFont);
    drawText(Form("%s","#sqrt{s} = 5.02 TeV"),xp,yp-dy,isRightAlign,kBlack,fontSize, textFont);
  } else{
    drawText(Form("%s","PYTHIA+Overlay"),xp,yp,isRightAlign,kBlack,fontSize, textFont);
    drawText(Form("%s","#sqrt{s_{NN}} = 5.02 TeV"),xp,yp-dy,isRightAlign,kBlack,fontSize, textFont);
    drawText(Form("Centrality %d-%d%s",cent_i,cent_f,"%"),xp,yp-dy*2,isRightAlign,kBlack,fontSize, textFont);
  }
}

void drawTextCollisionGeneral(TString coll, bool isMC, int cent_i, int cent_f, float xp, float yp, float dy=0.05, double fontSize=0.04, bool isRightAlign=1, int textFont = 42){
  //float dy = 0.05;
  if(coll=="PP"){
    if(isMC)
      drawText(Form("%s","PYTHIA"),xp,yp,isRightAlign,kBlack,fontSize, textFont);
    else
      drawText(Form("%s","pp data"),xp,yp,isRightAlign,kBlack,fontSize, textFont);
    drawText(Form("%s","#sqrt{s} = 5.02 TeV"),xp,yp-dy,isRightAlign,kBlack,fontSize, textFont);
  } else{
    if(isMC)
      drawText(Form("%s","PYTHIA+Overlay"),xp,yp,isRightAlign,kBlack,fontSize, textFont);
    else
      drawText(Form("%s","PbPb data"),xp,yp,isRightAlign,kBlack,fontSize, textFont);
    drawText(Form("%s","#sqrt{s_{NN}} = 5.02 TeV"),xp,yp-dy,isRightAlign,kBlack,fontSize, textFont);
    drawText(Form("Centrality %d-%d%s",cent_i,cent_f,"%"),xp,yp-dy*2,isRightAlign,kBlack,fontSize, textFont);
  }
}

void drawTextCollisionGeneral2(TString coll, bool isMC, int cent_i, int cent_f, float xp, float yp, float dy=0.05, double fontSize=0.04, bool isRightAlign=1, int textFont = 42){
  //float dy = 0.05;
  if(coll=="PP"){
    if(isMC)
      drawText(Form("%s","PYTHIA"),xp,yp,isRightAlign,kBlack,fontSize, textFont);
    else
      drawText(Form("%s","pp data"),xp,yp,isRightAlign,kBlack,fontSize, textFont);
    drawText(Form("%s","#sqrt{s} = 5.02 TeV"),xp,yp-dy,isRightAlign,kBlack,fontSize, textFont);
  } else{
    if(isMC)
      drawText(Form("%s","PYTHIA+Overlay"),xp,yp,isRightAlign,kBlack,fontSize, textFont);
    else
      drawText(Form("PbPb data %d-%d%s",cent_i,cent_f,"%"),xp,yp,isRightAlign,kBlack,fontSize, textFont);
    drawText(Form("%s","#sqrt{s_{NN}} = 5.02 TeV"),xp,yp-dy,isRightAlign,kBlack,fontSize, textFont);
    // drawText(Form("Centrality %d-%d%s",cent_i,cent_f,"%"),xp,yp-dy*2,isRightAlign,kBlack,fontSize, textFont);
  }
}

TGraphAsymmErrors* scale_graph(TGraphAsymmErrors* gr=0, Float_t s=0){
    int np = gr->GetN();
    TGraphAsymmErrors* new_gr = (TGraphAsymmErrors*) gr->Clone(Form("%s_scaled",gr->GetName()));
    //TGraphAsymmErrors* new_gr = new TGraphAsymmErrors(np);
    new_gr->SetName(Form("%s_scaled",gr->GetName()));
    for (int p=0; p<np; ++p) {
        double Xmean = 0;
        double Ymean = 0;
        gr->GetPoint(p,Xmean,Ymean);
        new_gr->SetPoint(p,Xmean,Ymean*s);
        double Xerr_l= gr->GetErrorXlow(p);
        double Xerr_h= gr->GetErrorXhigh(p);
        double Yerr_l= gr->GetErrorYlow(p);
        double Yerr_h= gr->GetErrorYhigh(p);
        new_gr->SetPointError(p,Xerr_l,Xerr_h,Yerr_l*s,Yerr_h*s);
    }
    return new_gr;
}

TGraphAsymmErrors* divide_graph_by_hist(TGraphAsymmErrors* gr=0, TH1* h1=0){
    int np = gr->GetN();
    TGraphAsymmErrors* new_gr = (TGraphAsymmErrors*) gr->Clone(Form("%s_dividedBy_%s",gr->GetName(),h1->GetName()));
    //TGraphAsymmErrors* new_gr = new TGraphAsymmErrors(np);
    new_gr->SetName(Form("%s_dividedBy_%s",gr->GetName(),h1->GetName()));
    for (int p=0; p<np; ++p) {
        double Xmean = 0;
        double Ymean = 0;
        double scaleF = h1->GetBinContent(p+1);
        gr->GetPoint(p,Xmean,Ymean);
        new_gr->SetPoint(p,Xmean,Ymean/scaleF);
        double Xerr_l= gr->GetErrorXlow(p);
        double Xerr_h= gr->GetErrorXhigh(p);
        double Yerr_l= gr->GetErrorYlow(p);
        double Yerr_h= gr->GetErrorYhigh(p);
        new_gr->SetPointError(p,Xerr_l,Xerr_h,Yerr_l/scaleF,Yerr_h/scaleF);
    }
    return new_gr;
}


//TGraphAsymmErrors* divide_graph_by_graph(TGraphAsymmErrors* gr_num=0, TGraphAsymmErrors* gr_den=0){
TGraphAsymmErrors* divide_graph_by_graph(TGraph* gr_num=0, TGraph* gr_den=0){
    int np = gr_den->GetN();
    TGraphAsymmErrors* new_gr = (TGraphAsymmErrors*) gr_den->Clone(Form("%s_dividedBy_%s",gr_num->GetName(),gr_den->GetName()));
    for (int p=0; p<np; ++p) {
        double Xmean_den = 0;
        double Ymean_den = 0;
        double Xmean_num = 0;
        double Ymean_num = 0;
        //double scaleF = h1->GetBinContent(p+1);
        gr_den->GetPoint(p,Xmean_den,Ymean_den);
        gr_num->GetPoint(p,Xmean_num,Ymean_num);
        new_gr->SetPoint(p,Xmean_den,Ymean_num/Ymean_den);
        //double Xerr_l= gr->GetErrorXlow(p);
        //double Xerr_h= gr->GetErrorXhigh(p);
        //double Yerr_l= gr->GetErrorYlow(p);
        //double Yerr_h= gr->GetErrorYhigh(p);
        //new_gr->SetPointError(p,Xerr_l,Xerr_h,Yerr_l/scaleF,Yerr_h/scaleF);
    }
    return new_gr;
}

void hist_to_graph(TGraphAsymmErrors* gr=0, TH1D* h1=0, TH1D* h2=0, TH1D* h3=0, bool doSelfScale=1){
    int np = gr->GetN();
    for (int p=0; p<np; ++p) {
        double Xmean = h1->GetBinCenter(p+1);
        double bin_width = h1->GetBinLowEdge(p+2) - h1->GetBinLowEdge(p+1);
        double nominal = h1->GetBinContent(p+1);
        double sysDown = h2->GetBinContent(p+1);
        double sysUp = h3->GetBinContent(p+1);
        double dy = abs(nominal-sysDown);
        if(dy<abs(sysUp-nominal)) dy = abs(sysUp-nominal);
        //cout << " nominal = " << nominal << ", sysDown = " << sysDown << ", sysUp = " << sysUp << endl;
        double dy_up = abs(nominal-sysUp);
        double dy_down = abs(nominal-sysDown);

        if(doSelfScale){
            gr->SetPoint(p,Xmean,nominal/nominal);
            gr->SetPointError(p,(bin_width)/2.,(bin_width)/2.,dy_up/nominal,dy_down/nominal);
        } else{
            gr->SetPoint(p,Xmean,nominal);
            gr->SetPointError(p,(bin_width)/2.,(bin_width)/2.,dy_up,dy_down);
        }
    }
}

TGraphAsymmErrors* sysHist_to_graph(TH1D* h1D_nominal, TH1D* h1D_sys, int offset_i=0, int offset_f=0){

  int np = h1D_nominal->GetNbinsX()-offset_i-offset_f;
  TGraphAsymmErrors* gr = new TGraphAsymmErrors(np);
  gr->SetName(Form("gr_sys_%s",h1D_sys->GetName()));

  for (int ipt=0; ipt<=np; ++ipt) {
    double Xmean = h1D_nominal->GetBinCenter(ipt+1+offset_i);
    double Ymean = h1D_nominal->GetBinContent(ipt+1+offset_i);
    gr->SetPoint(ipt, Xmean, Ymean);
    double bin_width = (h1D_nominal->GetBinLowEdge(ipt+2+offset_i) - h1D_nominal->GetBinLowEdge(ipt+1+offset_i))/2.;
    double error = TMath::Abs(h1D_sys->GetBinContent(ipt+1+offset_i));
    double Xerr_l = bin_width;
    double Xerr_r = bin_width;
    double Yerr_l = error;
    double Yerr_h = error;
    gr->SetPointError(ipt, Xerr_l, Xerr_r, Yerr_l, Yerr_h);
  }
  return gr;
}

TCanvas* canclone(TCanvas *c1)
{
   //example showing how to copy a full canvas into a pad in another canvas

  //create second canvas, divide it and copy first canvas in first subpad
  TCanvas *c2 = new TCanvas("c2","c2",600,800);
  c2->Divide(1,2);
  c2->cd(1);
  TPad *pad = (TPad*)gPad;
  TObject *obj;
  TIter next(c1->GetListOfPrimitives());
  while ((obj=next())) {
     gROOT->SetSelectedPad(pad);
     pad->GetListOfPrimitives()->Add(obj->Clone());
  }
  gPad->Modified();
  return c2;
}
#endif
