#include "../api.h"
#include <cstdio>
#include <cmath>
#include <vector>
using namespace andro;
int main(int argc,char**argv){ int m=atoi(argv[1]), tr=atoi(argv[2]); float at=atof(argv[3]); static Engine E; const int sr=48000; E.init(sr); E.rng.s=777+m; E.align(m,0,true); E.PJ.songOn=1; for(int i=0;i<8;i++) E.params[P_VOL1+i]= i==tr?0.8f:0.f;
 if(argc>4){ E.params[P_ST_ON]=0; for(int i=0;i<8;i++){E.params[P_ECH1+i]=0;E.params[P_REV1+i]=0;}}
 E.play(); int N=(int)(sr*(at+0.1)); std::vector<float> L(N),R(N); for(int i=0;i<N;i+=256) E.process(&L[i],&R[i],nullptr,nullptr,std::min(256,N-i));
 int c=(int)(at*sr); for(int k=-40;k<40;k+=2) printf("%d:%.4f ",k,L[c+k]); printf("\n"); }
