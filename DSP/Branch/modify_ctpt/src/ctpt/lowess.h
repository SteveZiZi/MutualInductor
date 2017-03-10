
#ifndef LOWESS_H
#define LOWESS_H

#include<vector>

using namespace std;

void lowess(const vector<double> &x, const vector<double> &y, double f, long nsteps, double delta, vector<double> &ys, vector<double> &rw, vector<double> &res);

void lowess(const vector<double> &x, const vector<double> &y, double f, long nsteps, vector<double> &ys);

void lowest(const vector<double> &x, const vector<double> &y, double xs, double &ys, long nleft, long nright, vector<double> &w,bool userw,  vector<double> &rw, bool &ok);

#endif
