// Mandaloid patterns //

#include "M5AtomS3.h"

#define WIDTH   130
#define HEIGHT  130
#define SCR     (WIDTH*HEIGHT)
#define COLN    32

float randomf(float minf, float maxf) {return minf + (esp_random()%(1UL << 31)) * (maxf - minf) / (1UL << 31);}

float sigmoid(float v) {

    constexpr float c1 = 0.03138777f;
    constexpr float c2 = 0.276281267f;
    constexpr float c_log2f = 1.442695022f;
    v *= c_log2f * 0.5f;
    int intPart = (int)v;
    float x = (v - intPart);
    float xx = x * x;
    float v1 = c_log2f + c2 * xx;
    float v2 = x + xx * c1 * x;
    float v3 = (v2 + v1);
    *((int*)&v3) += intPart << 24;
    float v4 = v2 - v1;
    float res = v3 / (v3 - v4);
    return res;

}

  uint16_t *col = NULL;
  float *p = NULL;
  float *v = NULL;
  float a = 0.25f;
  uint16_t colors[COLN];

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR);

  for(int i=0; i<COLN; i++) colors[i] = esp_random();

  memset(p, 0, 4*SCR);
  memset(v, 0, 4*SCR);

  a = randomf(-0.25f, 0.25f);

  float seed = randomf(-TWO_PI, TWO_PI);
  v[(WIDTH/2)+(HEIGHT/2)*WIDTH] = seed;
  v[(WIDTH/2)+((HEIGHT/2)-1)*WIDTH] = seed;
  v[((WIDTH/2)-1)+((HEIGHT/2)-1)*WIDTH] = seed;
  v[((WIDTH/2)-1)+(HEIGHT/2)*WIDTH] = seed;

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
      v[x+y*WIDTH] += sigmoid(p[(x-1)+y*WIDTH] + p[(x+1)+y*WIDTH] + p[x+(y-1)*WIDTH] + p[x+(y+1)*WIDTH]) * a - p[x+y*WIDTH];
    }
  }

  for(int i = 0; i < SCR; i++){

    p[i] += v[i];
    uint8_t coll = COLN - (COLN * p[i]);
    col[i] = colors[coll%COLN];

  }

  AtomS3.Display.pushImage(0, 0, WIDTH, HEIGHT, col);
  AtomS3.update();

}