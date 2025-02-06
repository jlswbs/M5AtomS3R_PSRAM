// Fizzy 2D cellular automata //

#include "M5AtomS3.h"

#define WIDTH   130
#define HEIGHT  130
#define SCR     (WIDTH*HEIGHT)
#define COLN    24

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  float *CellVal = NULL;
  uint8_t Calm = 233;
  int CellIndex = 0;
  uint16_t colors[COLN];

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR);

  for(int i=0; i<COLN; i++) colors[i] = esp_random();

  CellIndex = 0;
  Calm = 16 + esp_random()%233;
  for (int i = 0; i < SCR; i++) CellVal[i] = randomf(0.0f, 128.0f);

}

void setup(void){

  srand(time(NULL));

  auto cfg = M5.config();
  AtomS3.begin(cfg);
  AtomS3.Display.setSwapBytes(1);

  col = (uint16_t*)ps_malloc(4*SCR);
  CellVal = (float*)ps_malloc(4*SCR);
  
  rndrule();

}

void loop(void){

  if (AtomS3.BtnA.wasReleased()) rndrule();
  
  for (int i = 0; i < WIDTH; i++) {
    
    for (int j = 0; j < HEIGHT; j++) {

      CellIndex = (CellIndex+1)%SCR;

      uint8_t coll = map(4.7f*CellVal[CellIndex], 0, 100, 0, COLN);
      col[i+j*WIDTH] = colors[coll%COLN];   

      int below      = (CellIndex+1)%SCR;
      int above      = (CellIndex+SCR-1)%SCR;
      int left       = (CellIndex+SCR-HEIGHT)%SCR;
      int right      = (CellIndex+HEIGHT)%SCR;
      int aboveright = ((CellIndex-1) + HEIGHT + SCR)%SCR;
      int aboveleft  = ((CellIndex-1) - HEIGHT + SCR)%SCR;
      int belowright = ((CellIndex+1) + HEIGHT + SCR)%SCR;
      int belowleft  = ((CellIndex+1) - HEIGHT + SCR)%SCR;

      float NeighbourMix = powf((CellVal[left]*CellVal[right]*CellVal[above]*CellVal[below]*CellVal[belowleft]*CellVal[belowright]*CellVal[aboveleft]*CellVal[aboveright]),0.125f);
      CellVal[CellIndex] = fmod((sqrtf(CellVal[CellIndex]*NeighbourMix))+0.5f, Calm);

    }

  }

  AtomS3.Display.pushImage(0, 0, WIDTH, HEIGHT, col);
  AtomS3.update();

}