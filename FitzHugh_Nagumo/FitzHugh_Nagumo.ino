// FitzHugh-Nagumo reaction diffusion //

#include "M5AtomS3.h"

#define WIDTH   130
#define HEIGHT  130
#define SCR     (WIDTH*HEIGHT)
#define COLN    18

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  float reactionRate = 0.2f;
  float diffusionRate = 0.01f;
  float kRate = 0.4f;
  float fRate = 0.09f;  
  float *gridU = NULL;  
  float *gridV = NULL;
  float *gridNext = NULL;
  float *diffRateUYarr = NULL;
  float *diffRateUXarr = NULL;
  float *farr = NULL;
  float *karr = NULL;
  float *temp = NULL;
  uint16_t colors[COLN];

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR);

  for(int i=0; i<COLN; i++) colors[i] = esp_random();

  diffusionRate = randomf(0.01f, 0.05f);
  kRate = randomf(0.1f, 0.5f);
  fRate = randomf(0.04f, 0.09f);
  
  for(int i=0;i<SCR;++i){
      
    gridU[i] = 0.5f + randomf(-0.01f, 0.01f);
    gridV[i] = 0.25f + randomf(-0.01f, 0.01f);

  }

  setupF();
  setupK();

  for(int i=0;i<SCR;++i){
    
    diffRateUYarr[i] = randomf(0.03f, 0.05f);
    diffRateUXarr[i] = randomf(0.03f, 0.05f);
      
  }

}

void diffusionV(){
  
  for(int i=0;i<WIDTH;++i){
    for(int j=0;j<HEIGHT;++j){

      gridNext[i+j*WIDTH] = gridV[i+j*WIDTH]+diffusionRate*4.0f*(gridV[((i-1+WIDTH)%WIDTH)+j*WIDTH]+gridV[((i+1)%WIDTH)+j*WIDTH]+gridV[i+((j-1+HEIGHT)%HEIGHT)*WIDTH]+gridV[i+((j+1)%HEIGHT)*WIDTH]-4.0f*gridV[i+j*WIDTH]);
    
    }
  }
  
  memcpy(temp, gridV, 4*SCR);
  memcpy(gridV, gridNext, 4*SCR); 
  memcpy(gridNext, temp, 4*SCR);
  
}

void diffusionU(){
  
  for(int i=0;i<WIDTH;++i){
    for(int j=0;j<HEIGHT;++j){

      gridNext[i+j*WIDTH] = gridU[i+j*WIDTH]+4.0f*(diffRateUXarr[i+j*WIDTH]*(gridU[((i-1+WIDTH)%WIDTH)+j*WIDTH]+gridU[((i+1)%WIDTH)+j*WIDTH]-2.0f*gridU[i+j*WIDTH])+diffRateUYarr[i+j*WIDTH]*(gridU[i+((j-1+HEIGHT)%HEIGHT)*WIDTH]+gridU[i+((j+1)%HEIGHT)*WIDTH]-2.0f*gridU[i+j*WIDTH]));
    
    }
  }
  
  memcpy(temp, gridU, 4*SCR);
  memcpy(gridU, gridNext, 4*SCR); 
  memcpy(gridNext, temp, 4*SCR);
  
}

void reaction(){

  for(int i=0;i<SCR;++i){

    gridU[i] = gridU[i]+4.0f*(reactionRate*(gridU[i]-gridU[i]*gridU[i]*gridU[i]-gridV[i]+karr[i]));
    gridV[i] = gridV[i]+4.0f*(reactionRate*farr[i]*(gridU[i]-gridV[i]));
    
  }

}

void setupF(){
  
  for(int i=0;i<WIDTH;++i){
  
    for(int j=0;j<HEIGHT;++j) farr[i+j*WIDTH] = 0.01f + i * fRate / WIDTH;
  
  }

}

void setupK(){

  for(int i=0;i<WIDTH;++i){
  
    for(int j=0;j<HEIGHT;++j) karr[i+j*WIDTH] = 0.06f + j * kRate / HEIGHT;
    
  }

}

void setup(void){

  srand(time(NULL));

  auto cfg = M5.config();
  AtomS3.begin(cfg);
  AtomS3.Display.setSwapBytes(1);

  gridU = (float*)ps_malloc(4*SCR);
  gridV = (float*)ps_malloc(4*SCR);
  gridNext = (float*)ps_malloc(4*SCR);
  diffRateUYarr = (float*)ps_malloc(4*SCR);
  diffRateUXarr = (float*)ps_malloc(4*SCR);
  farr = (float*)ps_malloc(4*SCR);
  karr = (float*)ps_malloc(4*SCR);
  temp = (float*)ps_malloc(4*SCR);
  col = (uint16_t*)ps_malloc(4*SCR);
  
  rndrule();

}

void loop(void){

  if (AtomS3.BtnA.wasReleased()) rndrule();
  
  diffusionU();
  diffusionV();
  reaction();

  for(int i = 0; i < SCR; i++){

    uint8_t coll = COLN * gridU[i];
    col[i] = colors[coll%COLN];

  }

  AtomS3.Display.pushImage(0, 0, WIDTH, HEIGHT, col);
  AtomS3.update();

}