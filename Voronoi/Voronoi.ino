// Voronoi distribution (cell noise) //

#include "M5AtomS3.h"

#define WIDTH   130
#define HEIGHT  130
#define SCR     (WIDTH*HEIGHT)
#define COLN    24
#define PARTICLES 12

float distance(int x1, int y1, int x2, int y2) { return sqrtf(powf(x2 - x1, 2.0f) + powf(y2 - y1, 2.0f)); }

  uint16_t *col = NULL;
  float mindist;
  int x[PARTICLES];
  int y[PARTICLES];
  int dx[PARTICLES];
  int dy[PARTICLES];
  uint16_t colors[COLN];

void rndrule(){
  
  memset((uint16_t *) col, 0, 4*SCR);

  for(int i=0; i<COLN; i++) colors[i] = esp_random();

  for (int i=0; i<PARTICLES; i++) {
    
    x[i] = esp_random()%WIDTH;
    y[i] = esp_random()%HEIGHT;
    dx[i] = esp_random()%8;
    dy[i] = esp_random()%8;
    
  }

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
  
  for (int j=0; j<HEIGHT; j++) {
    
    for (int i=0; i<WIDTH; i++) {
      
      mindist = WIDTH;
      
      for (int p=0; p<PARTICLES; p++) {
        if (distance(x[p], y[p], i, j) < mindist) mindist = distance(x[p], y[p], i, j);
        if (distance(x[p]+WIDTH, y[p], i, j) < mindist) mindist = distance(x[p]+WIDTH, y[p], i, j);
        if (distance(x[p]-WIDTH, y[p], i, j) < mindist) mindist = distance(x[p]-WIDTH, y[p], i, j);
      }
      
      uint8_t coll = constrain(mindist, 0, COLN);
      col[i+j*WIDTH] = colors[coll%COLN];

    }
  }
  
  for (int p=0; p<PARTICLES; p++) {
    x[p] += dx[p];
    y[p] += dy[p];
    x[p] = x[p] % WIDTH;
    y[p] = y[p] % HEIGHT;
  }

  AtomS3.Display.pushImage(0, 0, WIDTH, HEIGHT, col);
  AtomS3.update();

}