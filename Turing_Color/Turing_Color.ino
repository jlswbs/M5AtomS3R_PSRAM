// Turing patterns color //

#include "M5AtomS3.h"

#define WIDTH   130
#define HEIGHT  130
#define SCR     (WIDTH*HEIGHT)
#define SCL     4
#define COLN    32

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  float *pat = NULL;
  float *pnew = NULL;
  float *pmedian = NULL;
  float *prange = NULL;
  float *pvar = NULL;

  uint8_t lim = 128;
  uint8_t dirs = 9;
  uint16_t colors[COLN];

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR);
  memset(pat, 1.0f, 4*SCR);

  for(int i=0; i<COLN; i++) colors[i] = esp_random();

  lim = 64 + esp_random() % 128;
  dirs = 6 + esp_random() % 5;

  pat[(WIDTH/2)+(HEIGHT/2)*WIDTH] = PI;

}

void setup(void){

  srand(time(NULL));

  auto cfg = M5.config();
  AtomS3.begin(cfg);
  AtomS3.Display.setSwapBytes(1);

  pat = (float*)ps_malloc(4*SCR);
  pnew = (float*)ps_malloc(4*SCR);
  pmedian = (float*)ps_malloc(4*SCR*SCL);
  prange = (float*)ps_malloc(4*SCR*SCL);
  pvar = (float*)ps_malloc(4*SCR*SCL);
  col = (uint16_t*)ps_malloc(4*SCR);
  
  rndrule();

}

void loop(void){

  if (AtomS3.BtnA.wasReleased()) rndrule();
  
  float R = randomf(0, TWO_PI);
  
  memcpy(pnew, pat, 4*SCR);
  memset(pmedian, 0, 4*SCR*SCL);
  memset(prange, 0, 4*SCR*SCL);
  memset(pvar, 0, 4*SCR*SCL);
  
  for(int i=0; i<SCL; i++){

    float d = (2<<i);
  
    for(int j=0; j<dirs; j++){
          
      float dir = j * TWO_PI / dirs + R;
      int dx = (d * cosf(dir));
      int dy = (d * sinf(dir));
           
      for(int l=0; l<SCR; l++){
        int x1 = l%WIDTH + dx, y1 = l/WIDTH + dy;
        if(x1<0) x1 = WIDTH-1-(-x1-1); if(x1>=WIDTH) x1 = x1%WIDTH;
        if(y1<0) y1 = HEIGHT-1-(-y1-1); if(y1>=HEIGHT) y1 = y1%HEIGHT;
        pmedian[l+SCR*i] += pat[x1+y1*WIDTH] / dirs;
      }
    }

    for(int j=0; j<dirs; j++){

      float dir = j * TWO_PI / dirs + R;
      int dx = (d * cosf(dir));
      int dy = (d * sinf(dir));
    
      for(int l=0; l<SCR; l++){
        
        int x1 = l%WIDTH + dx, y1 = l/WIDTH + dy;
        if(x1<0) x1 = WIDTH-1-(-x1-1); if(x1>=WIDTH) x1 = x1%WIDTH;
        if(y1<0) y1 = HEIGHT-1-(-y1-1); if(y1>=HEIGHT) y1 = y1%HEIGHT;
        pvar[l+SCR*i] += fabs(pat[x1+y1*WIDTH] - pmedian[l+SCR*i]) / dirs;
        prange[l+SCR*i] += pat[x1+y1*WIDTH] > lim ? 1.0f : -1.0f;
      
      }
    }
  }
  
  for(int l=0; l<SCR; l++){
    
    int imin = 0, imax = SCL;
    float vmin = MAXFLOAT;
    float vmax = -MAXFLOAT;
    
    for(int i=0; i<SCL; i++){
      if(pvar[l+SCR*i] <= vmin) { vmin = pvar[l+SCR*i]; imin = i; }
      if(pvar[l+SCR*i] >= vmax) { vmax = pvar[l+SCR*i]; imax = i; }
    }

    for(int i=0; i<=imax; i++)    pnew[l] += prange[l+SCR*i];
      
  }

  float vmin = MAXFLOAT;
  float vmax = -MAXFLOAT;

  for(int i=0; i<SCR; i++){
    vmin = fmin(vmin, pnew[i]);
    vmax = fmax(vmax, pnew[i]);     
  }

  float dv = vmax - vmin;
      
  for (int i = 0; i < SCR; i++){
    
    pat[i] = (pnew[i] - vmin) * 250.0f / dv;
    uint8_t coll = map(pat[i], 0, 255, 0, COLN);
    col[i] = colors[coll];

  }

  AtomS3.Display.pushImage(0, 0, WIDTH, HEIGHT, col);
  AtomS3.update();

}