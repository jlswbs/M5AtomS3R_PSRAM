// Water drops patterns //

#include "M5AtomS3.h"

#define WIDTH   130
#define HEIGHT  130
#define SCR     (WIDTH*HEIGHT)
#define NUM     10

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}
uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) { return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3); }

  uint16_t *col = NULL;
  float *p = NULL;
  float *v = NULL;

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR);

  memset(p, 0, 4*SCR);
  memset(v, 0, 4*SCR);

  for (int i = 0; i < NUM; i++) v[(4+esp_random()%(WIDTH-8))+(4+esp_random()%(HEIGHT-8))*WIDTH] = randomf(-TWO_PI, TWO_PI);

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
  
  for (int y = 1; y < HEIGHT-1; y++) {
    for (int x = 1; x < WIDTH-1; x++) {
      v[x+y*WIDTH] += (p[(x-1)+y*WIDTH] + p[(x+1)+y*WIDTH] + p[x+(y-1)*WIDTH] + p[x+(y+1)*WIDTH]) * 0.25f - p[x+y*WIDTH];
    }
  }

  for(int i = 0; i < SCR; i++){

    p[i] += v[i];
    uint8_t coll = 255.0f * fabs(p[i]);
    col[i] = color565(coll, coll, coll);

  }

  AtomS3.Display.pushImage(0, 0, WIDTH, HEIGHT, col);
  AtomS3.update();

}