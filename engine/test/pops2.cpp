#include "../api.h"
#include <cstdio>
#include <cmath>
#include <vector>
using namespace andro;
// a pop = a step in the waveform: shows up in the LOW band (HF timbre does not)
int count(const std::vector<float>& L, std::vector<int>* where=nullptr){ float a=1-expf(-2*3.14159f*3000/48000.f), y1=0,y2=0, p1=0,p2=0, ms=1e-6f; int c=0, last=-99999;
 for(size_t i=0;i<L.size();i++){ y1+=(L[i]-y1)*a; y2+=(y1-y2)*a; float d2=y2-2*p1+p2; p2=p1; p1=y2; float e=d2*d2; if(i>4800 && e>ms*100.f && fabsf(d2)>1.5e-3f && (int)i-last>2400){ c++; last=i; if(where) where->push_back(i);} ms+=(e-ms)*0.001f; } return c; }
int main_all(){ static Engine E; const int sr=48000;
 for(int m=0;m<NMOODS;m++){ E.init(sr); E.rng.s=777+m; E.align(m,0,true); E.PJ.songOn=1; if(getenv("NODRUM")) E.params[P_VOL1]=0; E.play(); int N=sr*40; std::vector<float> L(N),R(N);
  for(int i=0;i<N;i+=256) E.process(&L[i],&R[i],nullptr,nullptr,std::min(256,N-i));
  printf("%-8s pops %d\n",MOODS[m].name,count(L)); } }

int main(int argc,char**argv){ int m=atoi(argv[1]); static Engine E; const int sr=48000;
 for(int solo=0;solo<8;solo++){ E.init(sr); E.rng.s=777+m; E.align(m,0,true); E.PJ.songOn=1; for(int i=0;i<8;i++) E.params[P_VOL1+i]= i==solo?0.8f:0.f; E.play(); int N=sr*40; std::vector<float> L(N),R(N);
  for(int i=0;i<N;i+=256) E.process(&L[i],&R[i],nullptr,nullptr,std::min(256,N-i)); std::vector<int> w; int c=count(L,&w); printf("track %d pops %d",solo,c); for(int k=0;k<c&&k<4;k++) printf(" %.2fs",w[k]/48000.0); printf("\n"); } }
