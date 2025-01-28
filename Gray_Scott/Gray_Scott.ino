// Gray-Scott reaction diffusion //

#include "M5AtomS3.h"

#define WIDTH   130
#define HEIGHT  130
#define SCR     (WIDTH*HEIGHT)
#define COLN    22
#define ITER    36

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  float diffU = 0.16f;
  float diffV = 0.08f;
  float paramF = 0.035f;
  float paramK =  0.06f;
  float *gridU = NULL;  
  float *gridV = NULL;
  float *dU = NULL;
  float *dV = NULL;
  uint16_t colors[COLN];

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR);

  for(int i=0; i<COLN; i++) colors[i] = esp_random();

  diffU = randomf(0.0999f, 0.1999f);
  diffV = randomf(0.0749f, 0.0849f);
  paramF = randomf(0.0299f, 0.0399f);
  paramK = randomf(0.0549f, 0.0649f);

  int seed = esp_random() % (SCR>>4);

  for(int i = 0 ; i < SCR ; i++) {
    
    int px = (i % WIDTH) - (WIDTH / 2);
    int py = (i / HEIGHT) - (WIDTH / 2);

    if(px*px + py*py < seed) {
      gridU[i] = 0.5f * randomf(0.0f, 2.0f);
      gridV[i] = 0.25f * randomf(0.0f, 2.0f);
    } else {
      gridU[i] = 1.0f;
      gridV[i] = 0.0f;
    }
  
  }

}

void timestep(float F, float K, float diffU, float diffV) {

  for (int j = 1; j < HEIGHT-1; j++) {
    for (int i = 1; i < WIDTH-1; i++) {
            
      float u = gridU[i+j*WIDTH];
      float v = gridV[i+j*WIDTH];          
      float uvv = u * v * v;   
      float lapU = (gridU[(i-1)+j*WIDTH] + gridU[(i+1)+j*WIDTH] + gridU[i+(j-1)*WIDTH] + gridU[i+(j+1)*WIDTH] - 4.0f * u);
      float lapV = (gridV[(i-1)+j*WIDTH] + gridV[(i+1)+j*WIDTH] + gridV[i+(j-1)*WIDTH] + gridV[i+(j+1)*WIDTH] - 4.0f * v);
          
      dU[i+j*WIDTH] = diffU * lapU - uvv + F * (1.0f-u);
      dV[i+j*WIDTH] = diffV * lapV + uvv - (K+F) * v;
      gridU[i+j*WIDTH] += dU[i+j*WIDTH];
      gridV[i+j*WIDTH] += dV[i+j*WIDTH];      
          
    }
  }

}

void setup(void){

  srand(time(NULL));

  auto cfg = M5.config();
  AtomS3.begin(cfg);
  AtomS3.Display.setSwapBytes(1);

  gridU = (float*)ps_malloc(4*SCR);
  gridV = (float*)ps_malloc(4*SCR);
  dU = (float*)ps_malloc(4*SCR);
  dV = (float*)ps_malloc(4*SCR);
  col = (uint16_t*)ps_malloc(4*SCR);
  
  rndrule();

}

void loop(void){

  if (AtomS3.BtnA.wasReleased()) rndrule();
  
  for (int k = 0; k < ITER; k++) timestep(paramF, paramK, diffU, diffV);

  for(int i = 0; i < SCR; i++){
    
    uint8_t coll = COLN * sinf(gridU[i]);
    col[i] = colors[coll%COLN];

  }

  AtomS3.Display.pushImage(0, 0, WIDTH, HEIGHT, col);
  AtomS3.update();

}