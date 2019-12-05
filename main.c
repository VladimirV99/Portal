#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <GL/glut.h>

#include "shared.h"
#include "util.h"
#include "input.h"
#include "light.h"
#include "portal.h"

// Player Position
static double x = 0;
static double y = 1.0f;
static double z = 0;

// Camera Rotation
static double yaw = 0;
static double pitch = 0;

// Look Vector
static double look_x = 1;
static double look_y = 0;
static double look_z = 0;

// Lights
GLfloat main_light_position[] = { 0, 5, 0, 1 };

// GLUT Event Handlers
static void on_display(void);
static void on_reshape(int width, int height);
static void on_mouse_click(int button, int state, int m_x, int m_y);
static void on_timer(int data);

portal portals[4];

portal portal1 = { { 8, 1, 0 }, { -1, 0, 0 }, 6, 4, NULL };
portal portal2 = { { 0, 1, -8 }, { 0, 0, 1 }, 6, 4, NULL };
portal portal3 = { { 0, 1, 8 }, { 0, 0, -1 }, 6, 4, NULL };
portal portal4 = { { 6, 1, 6 }, { -1, 0, -1 }, 3, 4, NULL };

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL | GLUT_DOUBLE);

  glutInitWindowSize(window_width, window_height);
  glutInitWindowPosition(300, 300);
  glutCreateWindow("Non-Euclidean Space");

  glutDisplayFunc(on_display);
  glutReshapeFunc(on_reshape);
  glutKeyboardFunc(on_keyboard);
  glutKeyboardUpFunc(on_keyboard_up);
  glutMouseFunc(on_mouse_click);
  glutPassiveMotionFunc(on_mouse_move);

  glutSetCursor(GLUT_CURSOR_NONE);
  glutWarpPointer(window_width/2, window_height/2);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_NORMALIZE);

  glClearColor(0, 0, 0, 0);

  glutTimerFunc(20, on_timer, 0);

  // Initialize portals array
  portals[0] = portal1;
  portals[1] = portal2;
  portals[2] = portal3;
  portals[3] = portal4;
  normalize3v(portals[3].normal);

  // Link portals to show each other's view
  (void)link_portals(&portals[0], &portals[3]);
  (void)link_portals(&portals[1], &portals[2]);

  // (void)link_portals(&portals[0], &portals[1]);

  glutMainLoop();

  return 0;
}

void update_camera() {
  look_x = cos(to_radians(pitch))*cos(to_radians(yaw));
  look_y = -sin(to_radians(pitch));
  look_z = cos(to_radians(pitch))*sin(to_radians(yaw));
}

static void on_timer(int data) {
  if(data != 0)
    return;

  yaw = clamp_yaw(yaw + mouse_dx * MOUSE_SENSITIVITY);
  pitch = clamp_pitch(pitch + mouse_dy * MOUSE_SENSITIVITY);

  update_mouse();
  update_camera();

  if(is_forward_pressed) {
    x += look_x * PLAYER_SPEED;
    z += look_z * PLAYER_SPEED;
  }
  if(is_backward_pressed) {
    x -= look_x * PLAYER_SPEED;
    z -= look_z * PLAYER_SPEED;
  }
  if(is_left_pressed) {
    x += cos(to_radians(yaw)-PI/2) * PLAYER_SPEED;
    z += sin(to_radians(yaw)-PI/2) * PLAYER_SPEED;
  }
  if(is_right_pressed) {
    x += cos(to_radians(yaw)+PI/2) * PLAYER_SPEED;
    z += sin(to_radians(yaw)+PI/2) * PLAYER_SPEED;
  }

  glutPostRedisplay();

  glutTimerFunc(20, on_timer, 0);
}

static void on_mouse_click(int button, int state, int m_x, int m_y) {
  (void) button;
  (void) state;
  (void) m_x;
  (void) m_y;
}

static void draw_world() {
  glEnable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);

  // Draw the inside of the skybox sphere
  glCullFace(GL_FRONT);

  // Skybox
  glColor3f(0.2f, 0.8f, 1);
  glutSolidSphere(50, 10, 10);

  glEnable(GL_LIGHTING);

  // Draw the front faces of the walls
  glCullFace(GL_BACK);

  init_light(GL_LIGHT0, main_light_position, light_basic);

  glBegin(GL_QUADS);
    // Floor
    set_material(material_concrete_white);
    glNormal3f(0, 1, 0);

    glVertex3f(-8.0f, 0, -8.0f);
    glVertex3f(-8.0f, 0, 8.0f);
    glVertex3f(8.0f, 0, 8.0f);
    glVertex3f(8.0f, 0, -8.0f);

    // Back Wall
    set_material(material_concrete_red);
    glNormal3f(1, 0, 0);

    glVertex3f(-8.0f, 0, 8.0f);
    glVertex3f(-8.0f, 0, -8.0f);
    glVertex3f(-8.0f, 4.0f, -8.0f);
    glVertex3f(-8.0f, 4.0f, 8.0f);

    // Front Wall
    set_material(material_concrete_green);
    glNormal3f(-1, 0, 0);

    glVertex3f(8.0f, 0, -8.0f);
    glVertex3f(8.0f, 0, 8.0f);
    glVertex3f(8.0f, 4.0f, 8.0f);
    glVertex3f(8.0f, 4.0f, -8.0f);

    // Right Wall
    set_material(material_concrete_blue);
    glNormal3f(0, 0, -1);

    glVertex3f(8.0f, 0, 8.0f);
    glVertex3f(-8.0f, 0, 8.0f);
    glVertex3f(-8.0f, 4.0f, 8.0f);
    glVertex3f(8.0f, 4.0f, 8.0f);

    // Left Wall
    set_material(material_concrete_yellow);
    glNormal3f(0, 0, 1);

    glVertex3f(-8.0f, 0, -8.0f);
    glVertex3f(8.0f, 0, -8.0f);
    glVertex3f(8.0f, 4.0f, -8.0f);
    glVertex3f(-8.0f, 4.0f, -8.0f);
  glEnd();

  glDisable(GL_CULL_FACE);

  glPushMatrix();
    glTranslatef(-3, 1, 0);
    glutSolidTeapot(1);
  glPopMatrix();
}

void draw_scene() {
  for(unsigned int i = 0; i < sizeof(portals)/sizeof(portal); i++) {
    // If the portal is linked draw the link's view
    // Otherwise only draw the frame
    if(is_linked(&portals[i])) {
      glEnable(GL_STENCIL_TEST);

      // Create a mask for the portal in the stencil buffer
      // Disable drawing to the color and depth buffer
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);
      glDisable(GL_DEPTH_TEST);

      // Fail the stencil test on every drawn pixel
      // Increment the stencil buffer value for every failed test
      glStencilFunc(GL_NEVER, 0, 0xFF);
      glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);
      // Alternatively always pass the stencil test and increment on success
      // glStencilFunc(GL_ALWAYS, 0, 0xFF);
      // glStencilOp(GL_KEEP, GL_KEEP, GL_INCREMENT);

      // Clearing the stencil buffer depends on the mask value
      glStencilMask(0xFF);
      glClear(GL_STENCIL_BUFFER_BIT);

      // Draw the portal frame in the stencil buffer
      draw_portal_frame(&portals[i]);

      // Enable drawing to the color and depth buffers only where the portal was
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);
      glEnable(GL_DEPTH_TEST);

      // Disable drawing to the stencil buffer
      glStencilMask(0x00);

      // Draw where the stencil value is 1
      glStencilFunc(GL_EQUAL, 1, 0xFF);

      // Render a flipped mirror perspective from the portal
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

        // Make sure normal vectors are normalized
        float angle = to_degrees(acos(dot_prod3v(portals[i].normal, portals[i].link->normal)));
        // float angle = to_degrees(acos(dot_prod3v(portals[i].normal, portals[i].link->normal)/norm3v(portals[i].link->normal)/norm3v(portals[i].normal)));
        int orientation = sgn(orientationy3f(portals[i].normal, portals[i].link->normal));

        // Flip the camera
        glRotatef(180, 0, 1, 0);
        // Find the transformation between the portals
        // We multiply by 0.99 so it doesn't look disconnected
        // because the portal is offset from the wall a few pixels
        glTranslatef(-0.99f*portals[i].position[0], portals[i].position[1], -0.99f*portals[i].position[2]);
        glRotatef(orientation==0 ? angle : orientation*angle, 0, 1, 0);
        glTranslatef(-0.99f*portals[i].link->position[0], -portals[i].link->position[1], -0.99f*portals[i].link->position[2]);

        draw_world();

      glPopMatrix();

      glDisable(GL_STENCIL_TEST);
    } else {
      draw_portal_frame(&portals[i]);
    }
  }

  // Run a depth test to find objects closer to the camera than the portals
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);

  for(unsigned int i = 0; i < sizeof(portals)/sizeof(portal); i++) {
    draw_portal_frame(&portals[i]);
  }

  // Render the world from the players perspective
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	draw_world();
}

static void on_display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(x, y, z, x + look_x, y + look_y, z + look_z, 0.0f, 1.0f, 0.0f);

  draw_scene();

  glutSwapBuffers();
}

static void on_reshape(int width, int height) {
  window_width = width;
  window_height = height;

  glViewport(0, 0, (GLsizei)width, (GLsizei)height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective((GLdouble)60, (GLdouble)width / (GLdouble)height, (GLdouble)0.1, (GLdouble)100.0);

  glutPostRedisplay();
}
