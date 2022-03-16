void check_times(){

  double xorigincm = 0.0;
  double yorigincm = 0.0;
  double xfinalcm = 100.0;
  double yfinalcm = 50.0;

  double xsteplengthcm = .5;
  double ysteplengthcm = .5;

  double time_per_stop_per_rep = 1./180.;
  double time_per_empty = .1/360.;
  double n_rep = 3;
  double total_time = 0;

  double yskiplengthcm = 20.0;
  double xskiplengthcm = 70.0;
  double xp = 90;
  double yp = 0;
  double s = (yp - 20.5)/(xp - 3);
  double ymiddlecm = 22.0;

  int xsteps = (xfinalcm - xorigincm)/xsteplengthcm;
  int ysteps = (yfinalcm - yorigincm)/ysteplengthcm;

  TGraph *g_in = new TGraph();
  TGraph *g_out = new TGraph();

  double xpos = xorigincm;
  double ypos = yorigincm;
  double skipoffsetcm;
  for ( int i = 0; i < xsteps; i++){
    xpos += xsteplengthcm;
    ypos = yorigincm;
    for (int j = 0; j < ysteps; j++){
       ypos += ysteplengthcm;
       skipoffsetcm = s*(xpos - xp) + yp;
       bool skip = abs(ymiddlecm - ypos) > ymiddlecm - skipoffsetcm;
       if (xpos < 3.5) skip = true;
       if (xpos > 90) skip = true;
       if (skip) {
         g_out->Set(g_out->GetN()+1);
         g_out->SetPoint(g_out->GetN()-1, xpos, ypos);
         total_time = total_time + time_per_empty;
       }
       else {
         g_in->Set(g_in->GetN()+1);
         g_in->SetPoint(g_in->GetN()-1, xpos, ypos);
         total_time = total_time + n_rep*time_per_stop_per_rep;
       }
    }
    //total_time = total_time + ysteps*time_per_empty;
  }
  cout<<"Total steps taken: "<<g_in->GetN()<<" / "<<g_in->GetN() + g_out->GetN()<<endl;
  cout<<"Total Time is "<<total_time<<" hours or "<<total_time/24.<<" days..."<<endl;
  g_in->SetMarkerColor(kGreen);
  g_in->SetMarkerSize(2);
  g_in->SetMarkerStyle(20);
  g_out->SetMarkerColor(kRed);
  g_out->SetMarkerSize(2);
  g_out->SetMarkerStyle(20);

  g_out->Draw("AP");
  g_in->Draw("P");

  return;
}
