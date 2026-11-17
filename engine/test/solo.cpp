#include "../api.h"
#include <cstdio>
#include <cmath>
#include <vector>
using namespace andro;
int main(int argc,char**argv){ int m=atoi(argv[1]); static Engine E; const int sr=48000;
 for(int solo=-1;solo<8;solo++){ E.init(sr); E.rng.s=1234+m; E.align(m,0,true); E.PJ.songOn=0; E.switchScene(2);
  if(solo>=0) for(int i=0;i<8;i++) E.params[P_VOL1+i]= i==solo?0.8f:0.f;
  // dry only to isolate
  E.play(); int N=sr*15; std::vector<float> L(N),R(N); for(int i=0;i<N;i+=256) E.process(&L[i],&R[i],nullptr,nullptr,std::min(256,N-i));
  int c=0; float pk=0; for(int i=2;i<N;i++){ float d1=L[i]-L[i-1],d0=L[i-1]-L[i-2]; if(fabsf(d1-d0)>0.25f) c++; pk=fmaxf(pk,fabsf(L[i]));}
  printf("solo %d clicks %d peak %.2f\n",solo,c,pk);} }
