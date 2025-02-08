// Chladni patterns - color //

#include "M5AtomS3.h"

#define WIDTH   130
#define HEIGHT  130
#define SCR     (WIDTH*HEIGHT)
#define COLN    18

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

  uint16_t *col = NULL;
  float *p = NULL;
  float *v = NULL;
  float a, b, c, d;
  uint16_t colors[COLN];

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR);

  for(int i=0; i<COLN; i++) colors[i] = esp_random();

  a = randomf(0.005f, 0.05f);
  b = randomf(0.005f, 0.05f);
  c = randomf(0.005f, 0.05f);
  d = randomf(1.0f, 12.0f);

  memset(p, 0, 4*SCR);
  memset(v, 0, 4*SCR);

}

void setup(void){

  srand(time(NULL));

  auto cfg = M5.config();
  AtomS3.begin(cfg);
  AtomS3.Display.setSwapBytes(1);

  col = (uint16_t*)ps_malloc(4*SCR);
  p = (float*)ps_malloc(4*SCR);
  v = (float*)ps_malloc(4*SCR);
  
  rndrule();

}

void loop(void){

  if (AtomS3.BtnA.wasReleased()) rndrule();
  
  int frame = millis() / 8;

  v[(WIDTH/2)+(HEIGHT/2)*WIDTH] = 0;
  p[(WIDTH/2)+(HEIGHT/2)*WIDTH] = (sinf(frame * a) + sinf(frame * b) + sinf(frame * c)) * d;
  
  for (int y = 1; y < HEIGHT-1; y++) {
    for (int x = 1; x < WIDTH-1; x++) {
      v[x+y*WIDTH] += (p[(x-1)+y*WIDTH] + p[(x+1)+y*WIDTH] + p[x+(y-1)*WIDTH] + p[x+(y+1)*WIDTH]) * 0.25f - p[x+y*WIDTH];
    }
  }

  for(int i = 0; i < SCR; i++){

    p[i] += v[i];
    uint8_t coll = COLN * fabs(constrain(p[i], -1.0f, 1.0f));
    col[i] = colors[coll%COLN];

  }

  AtomS3.Display.pushImage(0, 0, WIDTH, HEIGHT, col);
  AtomS3.update();

}