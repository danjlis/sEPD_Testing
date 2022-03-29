// read out and make histograms of the dark current for each channel.
// #include "AtlasUtils.C"
// If you don't have AtlasUtils just delete the include statement and change the text writing commands on the last few lines.
// when calling this function, the first is the tile you are reading,
//then the x, y location in cm of the base of the sector (where the fiber bundle inserts into the secotr)
// then you can alter the rotation if you want, but it is fixed at 3 pi / 2 since that is the orientation in the cell.
// Maybe the odd and even sides are mixed up but thats an easy change. Let me know if anything needs fixing.

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

void draw_scan(const int tile = 8, const double xorigin = 96., const double yorigin = 25., const double rot = 3*TMath::Pi()/2,  const double scale_x = 23., const double scale_y = 86.){
  // initial points as from PlotDigitizer
  double point_x[53] = {0, 0.0443, -0.0443,      0,  0.0887, -0.0887,      0, 0.142,-0.142,     0., 0.189, -0.189,      0, 0.2512, -0.2512,    0.,  .3103, -.3103,      0, 0.3744, -0.3744,    0., 0.4384, -0.4384,     0., 0.5025, -0.5025,     0., 0.5616, -0.5616,     0., 0.6256, -0.6256,     0., 0.6847, -0.6847,    0., 0.7438, -0.7438,     0.,  0.8128, -0.8128,     0., 0.8768, -0.8768,     0., 0.936, -0.936,      0, 0.0394, -0.0394,      1,     -1};
  double point_y[53] = {1,      1,       1, 0.9507,  0.9533,  0.9533, 0.8987,  0.904,  0.904, 0.8467,  0.852,   0.852, 0.7813, 0.7907,  0.7907, 0.716, 0.7267, 0.7267, 0.6493,  0.664,   0.664, 0.584,    0.6,     0.6, 0.5187, 0.5373,  0.5373, 0.4547, 0.4747,  0.4747, 0.3893, 0.4093,  0.4093, 0.3227, 0.3467,  0.3467, 0.2587,  0.284,   0.284, 0.192,   0.22,    0.22, 0.1267,  0.1573,  0.1573, 0.0613, 0.0947,  0.0947,     0,     0,      0, 0.0307, 0.0307};
  double x, y;
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
  // here is where you put the text. Change if you'd  like
  // myText(0.15, 0.86, kBlack, "sPHENIX sEPD Scan", 0.03);
  // myText(0.15, 0.82, kBlack, Form("Tile %d w/ source", tile), 0.03);
  // myText(0.15, 0.78, kBlack, "[Your text here!]", 0.03);
}
