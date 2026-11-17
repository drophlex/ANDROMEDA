#include "../api.h"
#include <cstdio>
#include <cmath>
#include <vector>
using namespace andro;
int main(){ static Engine E; const int sr=48000; E.init(sr); E.rng.s=1234+3; E.align(3,0,true); E.PJ.songOn=0; E.switchScene(2);
 for(int i=0;i<8;i++) E.params[P_VOL1+i]= i==4?0.8f:0.f; E.params[P_ECH5]=0; E.params[P_REV5]=0; E.params[P_ST_ON]=0;
 E.play(); int N=sr*6; std::vector<float> L(N),R(N); for(int i=0;i<N;i+=256) E.process(&L[i],&R[i],nullptr,nullptr,std::min(256,N-i));
 int shown=0; for(int i=2;i<N&&shown<6;i++){ float d1=L[i]-L[i-1],d0=L[i-1]-L[i-2]; if(fabsf(d1-d0)>0.25f){ printf("at %d (%.3fs): ",i,i/48000.0); for(int k=-6;k<6;k++) printf("%.3f ",L[i+k]); printf("\n"); shown++; i+=2000; } }
 FILE*f=fopen("bell.raw","wb"); fwrite(L.data(),4,N,f); fclose(f); }
