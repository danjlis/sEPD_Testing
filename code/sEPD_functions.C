// read out and make histograms of the dark current for each channel.
// #include "AtlasUtils.C"
// If you don't have AtlasUtils just delete the include statement and change the text writing commands on the last few lines.
// when calling this function, the first is the tile you are reading,
//then the x, y location in cm of the base of the sector (where the fiber bundle inserts into the secotr)
// then you can alter the rotation if you want, but it is fixed at 3 pi / 2 since that is the orientation in the cell.
// Maybe the odd and even sides are mixed up but thats an easy change. Let me know if anything needs fixing.

// void isSourceOnTopOfTheTile(const int tile, const double xorigin, const double yorigin, const double rot); //to_be_updated
double GetTimeHours(double time){
  double r = (time/100. - floor(time/100.))/0.6;
  double new_time = r + floor(time/100.);
  return new_time;
}

void ParseFileName( string fname, char *date, char *test, int &sector, char *addon){

  std::string delimiter = "/";

  size_t pos = 0;
  std::string token;
  while ((pos = fname.find(delimiter)) != std::string::npos) {
      token = fname.substr(0, pos);
      //std::cout << token << std::endl;
      fname.erase(0, pos + delimiter.length());
  }
  size_t pos2 = 0;
  int n = 0;
  std::string delimiter2 = "_";
  cout<<fname<<endl;
  while ((pos2 = fname.find(delimiter2)) != std::string::npos) {
      token = fname.substr(0, pos2);
      std::cout << token << std::endl;
      cout<<"IF"<<endl;
      if ( n == 0 ) strcpy(date, token.c_str());
      else if ( n == 1 ) strcpy(test, token.c_str());
      else if ( n == 2 ) sector = stoi(token);
      n++;
      fname.erase(0, pos2 + delimiter2.length());
  }

}
void draw_ex(int tile = 7, double norm = 1, Option_t *option = "hist"){

  string file = Form("EX_tile%d/Tile%d_data.csv", tile, tile);
  ifstream input_file(Form("../data/old_star/%s", file.c_str()));

  input_file.is_open();

  string line, word;
  int val;
  vector<double> x = {};
  vector<double> y = {};
  while (getline(input_file, line)){

    if (line.empty()) continue;
    if (!isdigit(line.at(0))) continue;

    std::stringstream ss(line);
    vector<double> num = {};
    while(getline(ss, word, ',')){
      num.push_back(stof(word));
    }
    x.push_back(num.at(0));
    y.push_back(num.at(1));
  }
  const int size = x.size();
  double xx[size];
  double yy[size];
  for (int i = 0; i < size; i++){
    xx[i] = x.at(i);
    yy[i] = y.at(i)*norm;
  }
  TGraph *gg = new TGraph(size, xx, yy);
  gg->SetLineColor(kRed);
  gg->SetLineWidth(2);


  gg->Draw(option);

}

class Tile {
	public:
		int tile;
		int row;
		int ieo;
		double xc;
		double yc;
		double xr;
		double yr;
		double xm;
};

int TileInfo(Tile *arr_tile[]){
	int t;
	int r[31] = {1,
2,
2,
3,
3,
4,
4,
5,
5,
6,
6,
7,
7,
8,
8,
9,
9,
10,
10,
11,
11,
12,
12,
13,
13,
14,
14,
15,
15,
16,
16};
	int ieo[31] = {0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0,
1,
0};
	double xc[31] = {5.785440678,
10.17601994,
10.17601994,
14.5665992,
14.5665992,
19.52096876,
19.52096876,
25.0391286,
25.0391286,
30.55728845,
30.55728845,
36.07544829,
36.07544829,
41.59360814,
41.59360814,
47.11176798,
47.11176798,
52.62992783,
52.62992783,
58.15307697,
58.15307697,
63.67622611,
63.67622611,
69.19438596,
69.19438596,
74.7125458,
74.7125458,
80.23070565,
80.23070565,
85.74886549,
85.74886549};
	double yc[31] = {22,
22.73251505,
21.26748495,
23.02028882,
20.97971118,
23.34501535,
20.65498465,
23.70669466,
20.29330534,
24.06837396,
19.93162604,
24.43005327,
19.56994673,
24.79173257,
19.20826743,
25.15341188,
18.84658812,
25.51509118,
18.48490882,
25.8770975,
18.1229025,
26.23910382,
17.76089618,
26.60078313,
17.39921687,
26.96246243,
17.03753757,
27.32414173,
16.67585827,
27.68582104,
16.31417896};
	double xm[31] = {5.8,
10.2,
10.2,
14.6,
14.6,
19.565,
19.565,
25.095,
25.095,
30.625,
30.625,
36.155,
36.155,
41.685,
41.685,
47.215,
47.215,
52.745,
52.745,
58.28,
58.28,
63.815,
63.815,
69.345,
69.345,
74.875,
74.875,
80.405,
80.405,
85.935,
85.935
	};
	double xr[31] = {3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3,
3};
	double yr[31] = {2,
2,
2,
2,
2,
3,
3,
3,
3,
4,
4,
4,
4,
5,
5,
5,
5,
6,
6,
6,
6,
7,
7,
7,
7,
8,
8,
8,
8,
9,
9};

	for (t = 0; t < 31; t++){
		arr_tile[t] = new Tile;
		arr_tile[t]->tile = t + 1;
		arr_tile[t]->row = r[t];
		arr_tile[t]->ieo = ieo[t];
		arr_tile[t]->xc = xc[t];
		arr_tile[t]->yc = yc[t];
		arr_tile[t]->xm = xm[t];
	}

	return 1;
}
int InitializeTiles(){

	Tile *arr_tile[31];
	int cheese = TileInfo(arr_tile);
	for(int i = 0; i < 31; i++){
		cout<<"tile : "<<arr_tile[i]->tile<<endl;
	}
	return 1;
}

void draw_scan(const int tile = 8, const double xorigin = 96., const double yorigin = 25., const double rot = 3*TMath::Pi()/2,  const double scale_x = 23., const double scale_y = 86.) {
  double x, y;

  double point_x[53] = {0, 0.0443, -0.0443,      0,  0.0887, -0.0887,      0, 0.142,-0.142,     0., 0.189, -0.189,      0, 0.2512, -0.2512,    0.,  .3103, -.3103,      0, 0.3744, -0.3744,    0., 0.4384, -0.4384,     0., 0.5025, -0.5025,     0., 0.5616, -0.5616,     0., 0.6256, -0.6256,     0., 0.6847, -0.6847,    0., 0.7438, -0.7438,     0.,  0.8128, -0.8128,     0., 0.8768, -0.8768,     0., 0.936, -0.936,      0, 0.0394, -0.0394,      1,     -1} ;
  double point_y[53] = {1,      1,       1, 0.9507,  0.9533,  0.9533, 0.8987,  0.904,  0.904, 0.8467,  0.852,   0.852, 0.7813, 0.7907,  0.7907, 0.716, 0.7267, 0.7267, 0.6493,  0.664,   0.664, 0.584,    0.6,     0.6, 0.5187, 0.5373,  0.5373, 0.4547, 0.4747,  0.4747, 0.3893, 0.4093,  0.4093, 0.3227, 0.3467,  0.3467, 0.2587,  0.284,   0.284, 0.192,   0.22,    0.22, 0.1267,  0.1573,  0.1573, 0.0613, 0.0947,  0.0947,     0,     0,      0, 0.0307, 0.0307};
  // initializing the line matrix and index with other things too.
  int line_matrix[53][53] = {0};
  int line_index = 1;
  std::vector<int> line_red[31];
  line_red[0].push_back(1);
  line_red[0].push_back(2);
  line_red[0].push_back(3);
  line_red[0].push_back(4);
  line_red[0].push_back(5);
  line_red[0].push_back(6);
  int prev_even = 5;
  int prev_odd = 6;
  // drawing out which lines correspond to which tile, these will be colored red;
  for (int i = 1; i <31; i++){
    if(i == 29){
      line_red[i].push_back(76);
      line_red[i].push_back(77);
      line_red[i].push_back(79);
      line_red[i].push_back(81);
      line_red[i].push_back(82);
    }
    else if(i == 30){
      line_red[i].push_back(75);
      line_red[i].push_back(77);
      line_red[i].push_back(78);
      line_red[i].push_back(80);
      line_red[i].push_back(83);
    }
    else{
      if (i%2 == 1){
        line_red[i].push_back(prev_odd);
        line_red[i].push_back(prev_odd+3);
        line_red[i].push_back(prev_odd+5);
        line_red[i].push_back(prev_odd+1);
        prev_odd = prev_odd + 5;
      }
      else{
        line_red[i].push_back(prev_even);
        line_red[i].push_back(prev_even+3);
        line_red[i].push_back(prev_even+5);
        line_red[i].push_back(prev_even+2);
        prev_even = prev_even + 5;
      }
    }
  }
  // making the line matrix, integer greater than 0 means there's a line between the two points, and a 0 means no line.
  // lines are indexed by the order of their creation.
  for(int i = 0; i < 53; i++){
    if (i == 0){
      line_matrix[i][1] = line_index;
      line_index++;
      line_matrix[i][2] = line_index;
      line_index++;
      continue;
    }
    else{
      for(int j = i + 1; j < 53; j++){
        if ( point_x[i] == 0 ){
          if (j == i + 1 || j == i + 2){
            line_matrix[i][j] = line_index;
            line_index++;
          }
          if (point_x[j] == 0 && j == i + 3){
            line_matrix[i][j] = line_index;
            line_index++;
          }
        }
        else{
          if ((j == i + 3 || (j == i + 5 && j > 20)) && abs(point_x[i] - point_x[j]) < 0.07){
            line_matrix[i][j] = line_index;
            line_index++;
          }
        }
      }
    }
  }
  line_matrix[50][52] = line_index;
  line_index++;
  line_matrix[49][51] = line_index;
  line_index++;

  // scaling, rotating, and translating the sector
  for ( int i = 0; i < 53; i++){
    point_x[i] = point_x[i]*scale_x;
    point_y[i] = point_y[i]*scale_y;
  }
  for ( int i = 0; i < 53; i++){
    x = point_x[i];
    y = point_y[i];
    point_x[i] = x*cos(rot) + y*sin(rot);
    point_y[i] = -1*x*sin(rot) + y*cos(rot);
    // cout<<point_x[i]<<", "<<point_y[i]<<endl;
  }
  for (int i = 0; i < 53; i++){
    point_x[i] += xorigin;
    point_y[i] += yorigin;
  }
  /*
     TGraph *tg = new TGraph(53, point_x, point_y);
     tg->SetMarkerStyle(1);
     tg->SetMarkerColor(kBlack);
     tg->Draw("AP");
     */
  int s = line_red[tile].size();
  std::cout<<"size: "<< s <<std::endl;
  bool innit = false;
  TLine *line;
  //drawing the lines, (red if the tile is chosen)
  for ( int i = 0; i < 53; i++){
    for ( int j = 0; j < 53; j++ ){
      if(line_matrix[i][j] > 0 ){
        line = new TLine(point_x[i], point_y[i], point_x[j], point_y[j]);
        line->SetLineWidth(2);
        for (std::vector<int>::iterator k = line_red[tile - 1].begin(); k != line_red[tile - 1].end(); ++k){
          if (line_matrix[i][j] == *k) innit = true;
        }
        if (innit) line->SetLineColor(kRed);
        else line->SetLineColor(kBlack);
        innit = false;
        line->Draw();
      }
    }
  }
}

void GetParameters(vector<string> &paramNames, vector<string> &params, TString inputFileName = "20220131-1540_middlescan.txt", TString cap=""){

  ifstream infile(Form("../data/%s", inputFileName.Data()));

  /////////////////////////////////////
  // IMPORT NUMBERS FROM THE TXT FILE
  Long64_t nLines = 0;
  Bool_t isDiffPosition = false;
  Float_t xpos_temp = 0;
  Float_t ypos_temp = 0;
  int ll = 0;


  while (infile)
  {
    string s;
    if (!getline( infile, s )) break;
    if (s.empty()) continue;
    if (isdigit(s.at(0))) continue;
    istringstream ss( s );
    vector<string> vec_str;

    while (ss)
    {
      string stemp;
      if (!getline( ss, stemp, '=' )) break;
      vec_str.push_back( stemp );
    }
    if(vec_str.size() < 2) continue;

    istringstream sN(vec_str.at(0));
    istringstream sP(vec_str.at(1));

    while (sN){
      string stemp;
      if (!getline( sN, stemp, ',')) break;
      paramNames.push_back( stemp );
    }
    while (sP){
      string stemp;
      if (!getline( sP, stemp, ',')) break;
      params.push_back( stemp );
    }
  }
}

// void isSourceOnTopOfTheTile(const int tile = 8, const double xorigin = 96., const double yorigin = 25., const double rot = 3*TMath::Pi()/2){
// }

void make_sEPD_rootfile_v3(TString inputFileName = "20220113-1447_TEST_OUTPUT.txt", TString cap="", int ttt = 32){

  cout << "RUNNING: make_sEPD_rootfile_v3 for " << inputFileName << " input file" << endl;

  ifstream infile(Form("../data/%s", inputFileName.Data()));
  Float_t xpos, ypos;
  vector<Int_t> device;
  vector<Int_t> ch;
  vector<Int_t> tile;
  vector<Float_t> imon, rmon, vcomp;

  /////////////////////////////////////
  // DEFINE TREE AND OUTPUT ROOT FILE
  TString fname = Form("../data/sEPD%s.root",cap.Data());
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
      xpos_temp = stof(vec_str[4]);
      ypos_temp = stof(vec_str[5]);
      if (!(xpos_temp == xpos && ypos_temp == ypos)){
        tr->Fill();
        device.clear();
        ch.clear();
        tile.clear();
        imon.clear();
        rmon.clear();
        vcomp.clear();
        xpos = xpos_temp;
        ypos = ypos_temp;
      }
      else {
        xpos = xpos_temp;
        ypos = ypos_temp;
      }
      device.push_back(stoi(vec_str[0]));
      ch.push_back(stoi(vec_str[1]));
      tile.push_back(stoi(vec_str[2]));
      imon.push_back(stof(vec_str[6]));
      rmon.push_back(stof(vec_str[7]));
      vcomp.push_back(stof(vec_str[8]));
      nLines++;
    }

    // if(tile.size() == ttt){
    //   tr->Fill();
    //   device.clear();
    //   ch.clear();
    //   tile.clear();
    //   imon.clear();
    //   rmon.clear();
    //   vcomp.clear();
    // }
  }

  cout << "total numer of lines analyzed = " << nLines << endl;
  fout->cd();
  tr->Write("", TObject::kOverwrite);
  delete tr;
  fout->Close();
  cout << "DONE: " << fname << " has been created" << endl;
}

void make_sEPD_rootfile_v1(TString inputFileName = "20220113-1447_TEST_OUTPUT.txt", TString cap="", int ttt = 32){

  cout << "RUNNING: make_sEPD_rootfile_v1 for " << inputFileName << " input file" << endl;

  ifstream infile(Form("../data/%s", inputFileName.Data()));
  Float_t xpos, ypos;
  vector<Int_t> device;
  vector<Int_t> ch;
  vector<Int_t> tile;
  vector<Float_t> imon, rmon, vcomp;

  /////////////////////////////////////
  // DEFINE TREE AND OUTPUT ROOT FILE
  TString fname = Form("../data/sEPD%s.root",cap.Data());
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

    if(tile.size() == ttt){
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
void make_sEPD_rootfile_v2(TString inputFileName = "20220113-1447_TEST_OUTPUT.txt", TString cap=""){

  cout << "RUNNING: make_sEPD_rootfile_v2 for " << inputFileName << " input file" << endl;

  ifstream infile(Form("../data/%s", inputFileName.Data()));
  Float_t xpos, ypos;
  vector<Int_t> device;
  vector<Int_t> ch;
  vector<Int_t> tile;
  vector<Float_t> time;
  vector<Float_t> imon, rmon, vcomp;

  /////////////////////////////////////
  // DEFINE TREE AND OUTPUT ROOT FILE
  TString fname = Form("../data/sEPD%s.root",cap.Data());
  TFile* fout = new TFile(Form("%s",fname.Data()), "recreate");
  TTree* tr = new TTree("sEPDTree", "");
  tr->Branch("xpos", &xpos, "xpos/F");
  tr->Branch("ypos", &ypos, "xpos/F");
  tr->Branch("time", &time);
  tr->Branch("device", &device);
  tr->Branch("channel", &ch);
  tr->Branch("tile", &tile);
  tr->Branch("imon", &imon);

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
    //printf(" %s \n", ss);
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
      xpos = stof(vec_str[5]);
      ypos = stof(vec_str[6]);
      time.push_back(stof(vec_str[7]));
      imon.push_back(stof(vec_str[8]));
      nLines++;
    }
    if (xpos == 0 && ypos == 0){
      if(tile.size()==32){
        tr->Fill();
        device.clear();
        ch.clear();
        tile.clear();
        imon.clear();
        time.clear();
      }
    }
    else{
      if(tile.size()==1){
        tr->Fill();
        device.clear();
        ch.clear();
        tile.clear();
        imon.clear();
        time.clear();
      }
    }
  }

  cout << "total numer of lines analyzed = " << nLines << endl;
  fout->cd();
  tr->Write("", TObject::kOverwrite);
  delete tr;
  fout->Close();
  cout << "DONE: " << fname << " has been created" << endl;
}
