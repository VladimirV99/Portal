#ifndef PORTAL_H_
#define PORTAL_H_

#include <GL/glut.h>

#include "light.h"

typedef struct portal {
  GLfloat position[3];
  GLfloat normal[3];
  GLfloat width;
  GLfloat height;
  struct portal *link;
} portal;

extern void draw_portal_frame(portal p);

#endif
