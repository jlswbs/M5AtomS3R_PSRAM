// Plasma color //

#include "M5AtomS3.h"

#define WIDTH   130
#define HEIGHT  130
#define SCR     (WIDTH*HEIGHT)
#define COLN    20

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  float times, alpha;
  int offx, offy;
  uint16_t colors[COLN];

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR);

  for(int i=0; i<COLN; i++) colors[i] = esp_random();
  alpha = randomf(8.0f, 40.0f);
  offx = esp_random()%WIDTH;
  offy = esp_random()%HEIGHT;
  times = 0;

}

void setup(void){

  srand(time(NULL));

  auto cfg = M5.config();
  AtomS3.begin(cfg);
  AtomS3.Display.setSwapBytes(1);

  col = (uint16_t*)ps_malloc(4*SCR);
  
  rndrule();

}

void loop(void){

  if (AtomS3.BtnA.wasReleased()) rndrule();
  
  for (int j=0;j<HEIGHT;j++){
    
    for (int i=0;i<WIDTH;i++) {
      
      float value1 = sinf((i*sinf(times/2.0f)+j*cosf(times/3.0f))/alpha+times);
      float distance = sqrtf(((offx-i)*(offx-i))+((offy-j)*(offy-j)));   
      float value2 = sinf((distance/alpha) + times);
            
      uint8_t coll = COLN * fabs(sinf((value1+value2) * PI));
      col[i+j*WIDTH] = colors[coll%COLN];
      
    }
    
  }

  times += 0.1f;

  AtomS3.Display.pushImage(0, 0, WIDTH, HEIGHT, col);
  AtomS3.update();

}