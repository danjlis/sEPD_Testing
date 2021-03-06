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

int oneback(Tile *arr_tile[]){
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
	int cheese = oneback(arr_tile);
	for(int i = 0; i < 31; i++){
		cout<<"tile : "<<arr_tile[i]->tile<<endl;
	}
	return 1;
}
