
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include "GL/glut.h"
#include "GL/glu.h"
#endif

#include <string.h>
#include <time.h>
#include <math.h>

using namespace std;

#define PI 3.14159265  
inline float sqr(float x) { return x*x; }



class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};


// Global Variables

Viewport	viewport;
float specpow = 0.0;
float rgb_ka[3];
float rgb_kd[3];
float rgb_ks[3];
float directedLxyz[15];
float directedLrgb[15];
float pointLxyz[15];
float pointLrgb[15];
int numPointL = 0;
int numDirectedL = 0;




void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}



void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);
}



void circle(float centerX, float centerY, float radius) {
  // Draw inner circle
  glBegin(GL_POINTS);

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));


  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {
        
        float totambient_R = 0;
        float totambient_G = 0;
        float totambient_B = 0;
        float totdiffuse_R = 0;
        float totdiffuse_G = 0;
        float totdiffuse_B = 0;
        float totspecular_R = 0;
        float totspecular_G = 0;
        float totspecular_B = 0;


      // Location of the center of pixel relative to center of sphere
      float x = (i+0.5-centerX);
      float y = (j+0.5-centerY);

      float dist = sqrt(sqr(x) + sqr(y));

      if (dist<=radius) {
          

        //front-facing Z coordinate
        float z = sqrt(radius*radius-dist*dist);
          
          //normalize pixel vector
          float PIXmagnitude = sqrt((x*x) + (y*y) + (z*z));
          float PIXnormXYZ[] = {x/PIXmagnitude, y/PIXmagnitude, z/PIXmagnitude};
          
          
          //****************DIRECTIONAL LIGHTS****************
          
          for (int num = 0; num < numDirectedL; num++) {
              
              float DLxyz[] = {-directedLxyz[3*num], -directedLxyz[(3*num)+1], -directedLxyz[(3*num)+2]};
              float DLrgb[] = {directedLrgb[3*num], directedLrgb[(3*num)+1], directedLrgb[(3*num)+2]};
              
              //AMBIENT
              
              totambient_R += DLrgb[0] * rgb_ka[0];
              totambient_G += DLrgb[1] * rgb_ka[1];
              totambient_B += DLrgb[2] * rgb_ka[2];
              
              
              //DIFFUSE
              
              //normalize Directional light xyz vector
              float DLmagnitude = sqrt(sqr(DLxyz[0]) + sqr(DLxyz[1]) + sqr(DLxyz[2]));
              float DLnormXYZ[] = {DLxyz[0]/DLmagnitude, DLxyz[1]/DLmagnitude, DLxyz[2]/DLmagnitude};
              
              //vectorMultiply diffuse Kd and Directional light RGB
              float diffuseDL[] = {rgb_kd[0]*DLrgb[0], rgb_kd[1]*DLrgb[1], rgb_kd[2]*DLrgb[2]};
              
              //dotProduct (normalized Directional light, normalized Pixel)
              float dotProdDL = (DLnormXYZ[0]*PIXnormXYZ[0]) + (DLnormXYZ[1]*PIXnormXYZ[1]) + (DLnormXYZ[2]*PIXnormXYZ[2]);
              
              float DLscalar = max(dotProdDL, 0.0f);
              
              totdiffuse_R += diffuseDL[0]*DLscalar;
              totdiffuse_G += diffuseDL[1]*DLscalar;
              totdiffuse_B += diffuseDL[2]*DLscalar;
              
              
              //SPECULAR
              
              //vectorMultiply specular Ks and Directional light RGB
              float specularDL[] = {rgb_ks[0]*DLrgb[0], rgb_ks[1]*DLrgb[1], rgb_ks[2]*DLrgb[2]};
              
              //rNorm -- don't need whole thing since viewer at (0, 0, 1)
              //float rNorm[] = {(-DLnormXYZ[0]+2*dotProdDL*PIXnormXYZ[0]), (-DLnormXYZ[1]+2*dotProdDL*PIXnormXYZ[1]), (-DLnormXYZ[2]+2*dotProdDL*PIXnormXYZ[2])};
              
              //dotproduct (rNorm, normalized viewer (0, 0, 1))
              float dotProdrNorm = (-DLnormXYZ[2]+2*dotProdDL*PIXnormXYZ[2]);  //rNorm[2]
              
              float specScalarDL = pow(max(dotProdrNorm, 0.0f), specpow);
              
              totspecular_R += specularDL[0]*specScalarDL;
              totspecular_G += specularDL[1]*specScalarDL;
              totspecular_B += specularDL[2]*specScalarDL;
              
          }
          
          
          
          //****************POINT LIGHTS****************
          
          for (int num = 0; num < numPointL; num++) {
              
              float PLxyz[] = {pointLxyz[3*num]*radius-x, pointLxyz[(3*num)+1]*radius-y, pointLxyz[(3*num)+2]*radius-z};
              float PLrgb[] = {pointLrgb[3*num], pointLrgb[(3*num)+1], pointLrgb[(3*num)+2]};
              
              
              //AMBIENT
              
              totambient_R += PLrgb[0] * rgb_ka[0];
              totambient_G += PLrgb[1] * rgb_ka[1];
              totambient_B += PLrgb[2] * rgb_ka[2];
              
              
              //DIFFUSE
              
              //normalize xyz Point light vector
              float PLmagnitude = sqrt(sqr(PLxyz[0]) + sqr(PLxyz[1]) + sqr(PLxyz[2]));
              float PLnormXYZ[] = {PLxyz[0]/PLmagnitude, PLxyz[1]/PLmagnitude, PLxyz[2]/PLmagnitude};
              
              //vectorMultiply
              float diffusePL[] = {rgb_kd[0]*PLrgb[0], rgb_kd[1]*PLrgb[1], rgb_kd[2]*PLrgb[2]};
              
              //dotProduct (normalized Point light, normalized Pixel)
              float dotProdPL = (PLnormXYZ[0]*PIXnormXYZ[0]) + (PLnormXYZ[1]*PIXnormXYZ[1]) + (PLnormXYZ[2]*PIXnormXYZ[2]);
              
              float scalar = max(dotProdPL, 0.0f);
              
              totdiffuse_R += scalar * diffusePL[0];
              totdiffuse_G += scalar * diffusePL[1];
              totdiffuse_B += scalar * diffusePL[2];
              
              
              //SPECULAR
              
              //vectorMultiply specular Ks and Point light rgb
              float specularPL[] = {rgb_ks[0]*PLrgb[0], rgb_ks[1]*PLrgb[1], rgb_ks[2]*PLrgb[2]};
              
              //rNorm -- don't need whole thing since viewer at (0, 0, 1)
              //float rNormPL[] = {(-PLnormXYZ[0]+(2*dotProdPL*PIXnormXYZ[0])), (-PLnormXYZ[1]+(2*dotProdPL*PIXnormXYZ[1])), (-PLnormXYZ[2]+(2*dotProdPL*PIXnormXYZ[2]))};
              
              //dotproduct (rNorm, normalized viewer (0, 0, 1)
              float dotProdrNormPL = -PLnormXYZ[2]+(2*dotProdPL*PIXnormXYZ[2]);
              
              float specScalar = pow(max(dotProdrNormPL, 0.0f), specpow);
              
              totspecular_R += specScalar * specularPL[0];
              totspecular_G += specScalar * specularPL[1];
              totspecular_B += specScalar * specularPL[2];

          }

          float tot_R = (totambient_R + totdiffuse_R + totspecular_R);
          float tot_G = (totambient_G + totdiffuse_G + totspecular_G);
          float tot_B = (totambient_B + totdiffuse_B + totspecular_B);


          setPixel(i,j, tot_R, tot_G, tot_B);

      }
    }

  }

  glEnd();
}


//****************************************************
// display rendered scene
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();


  // Start drawing
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 2.5);

  glFlush();
  glutSwapBuffers();
}

//Used to break from the glutMainLoop at a Spacebar press
void idleInput (unsigned char key, int xmouse, int ymouse) {
    switch (key)
    {
        case ' ':
            exit(0);
        default:
            break;
    }
}






//****************************************************
// main function
//****************************************************
int main(int argc, char *argv[]) {
  
    glutInit(&argc, argv);
    
	for (int i = 1; i < argc; i) {

		if (strcmp(argv[i], "-ka") == 0) { //AMBIENT with rgb---------------
			rgb_ka[0] = atof(argv[i+1]);
			rgb_ka[1] = atof(argv[i+2]);
			rgb_ka[2] = atof(argv[i+3]);
			cout << rgb_ka[0] << rgb_ka[1] << rgb_ka[2] << endl;
			i = i+4;
		} else if (strcmp(argv[i], "-kd") == 0) { //DIFFUSE with rgb--------
			rgb_kd[0] = atof(argv[i+1]);
			rgb_kd[1] = atof(argv[i+2]);
			rgb_kd[2] = atof(argv[i+3]);
			cout << rgb_kd[0] << rgb_kd[1] << rgb_kd[2] << endl;
			i = i+4;
		} else if (strcmp(argv[i], "-ks") == 0) { //SPECULAR with rgb-------
			rgb_ks[0] = atof(argv[i+1]);
			rgb_ks[1] = atof(argv[i+2]);
			rgb_ks[2] = atof(argv[i+3]);
			cout << rgb_ks[0] << rgb_ks[1] << rgb_ks[2] << endl;
			i = i+4;
		} else if (strcmp(argv[i], "-sp") == 0) { //ASSIGN SPECULAR POWER---
			specpow = atof(argv[i+1]);
			cout << specpow << endl;
			i = i+2;
		} else if (strcmp(argv[i], "-pl") == 0) { //POINT LIGHT-------------
            pointLxyz[numPointL*3] = atof(argv[i+1]);
            pointLxyz[numPointL*3+1] = atof(argv[i+2]);
            pointLxyz[numPointL*3+2] = atof(argv[i+3]);
            pointLrgb[numPointL*3] = atof(argv[i+4]);
            pointLrgb[numPointL*3+1] = atof(argv[i+5]);
            pointLrgb[numPointL*3+2] = atof(argv[i+6]);
            numPointL++;
			i = i+7;
		} else  if (strcmp(argv[i], "-dl") == 0) { //DIRECTION LIGHT---------
            directedLxyz[numDirectedL*3] = atof(argv[i+1]);
            directedLxyz[numDirectedL*3+1] = atof(argv[i+2]);
            directedLxyz[numDirectedL*3+2] = atof(argv[i+3]);
            directedLrgb[numDirectedL*3] = atof(argv[i+4]);
            directedLrgb[numDirectedL*3+1] = atof(argv[i+5]);
            directedLrgb[numDirectedL*3+2] = atof(argv[i+6]);
            numDirectedL++;
			i = i+7;
		}
	}
	


  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // viewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);
  


  glutDisplayFunc(myDisplay);
  glutReshapeFunc(myReshape);
  glutKeyboardFunc(idleInput);
  glutMainLoop();


  return 0;
}

