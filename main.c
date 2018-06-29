/*

Programa para carregar modelos de objetos OBJ usando OpenGL e GLM

Observações:
- câmera esta posição padrão (na origem olhando pra a direção negativa de z)
- usando fonte de luz padrão (fonte de luz distante e na direção negativa de z)

*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <GL/glew.h>
#include "util/glut_wrap.h"
#include "glm.h"
#include "util/trackball.h"
#include "util/shaderutil.h"

GLdouble obsX = 0, obsY = 0, obsZ = 0;

static void updateModels(void);

static char *Model_file = NULL;		/* nome do arquivo do objeto */
static GLMmodel *Model;             /* modelo do objeto*/

/* de cada objeto */
static char *courage_file;
static GLMmodel *courage_model;

static char *house_file;
static GLMmodel *house_model;

static char *truck_file;
static GLMmodel *truck_model;

static char *windmIll_file;
static GLMmodel *windmIll_model;

static int currentModel = 0;        /* numero do Model atual */

static GLfloat Scale = 4.0;			/* fator de escala */
static GLint WinWidth = 1024, WinHeight = 768;

typedef struct{
   // Variáveis para controles de rotação
   float rotX, rotY, rotX_ini, rotY_ini;
   int x_ini,y_ini,bot;
   float Distance;
   /* Quando o mouse está se movendo */
   GLboolean Rotating, Translating;
   GLint StartX, StartY;
   float StartDistance;
} ViewInfo;

static ViewInfo View;

static void InitViewInfo(ViewInfo *view){
   view->Rotating = GL_FALSE;
   view->Translating = GL_FALSE;
   view->Distance = 12.0;
   view->StartDistance = 0.0;
}

static void read_model(void){
   float objScale;

   /* lendo o modelo */
   courage_model = glmReadOBJ(courage_file);
   objScale = glmUnitize(courage_model);
   glmFacetNormals(courage_model);
   if (courage_model->numnormals == 0) {
      GLfloat smoothing_angle = 90.0;
      glmVertexNormals(courage_model, smoothing_angle);
   }

   glmLoadTextures(courage_model);
   glmReIndex(courage_model);
   glmMakeVBOs(courage_model);

   /* lendo o modelo */
   house_model = glmReadOBJ(house_file);
   objScale = glmUnitize(house_model);
   glmFacetNormals(house_model);
   if (house_model->numnormals == 0) {
      GLfloat smoothing_angle = 90.0;
      glmVertexNormals(house_model, smoothing_angle);
   }

   glmLoadTextures(house_model);
   glmReIndex(house_model);
   glmMakeVBOs(house_model);


   /* carregando o truck */
    truck_model = glmReadOBJ(truck_file);
   objScale = glmUnitize(truck_model);
   glmFacetNormals(truck_model);
   if (truck_model->numnormals == 0) {
      GLfloat smoothing_angle = 90.0;
      glmVertexNormals(truck_model, smoothing_angle);
   }

   glmLoadTextures(truck_model);
   glmReIndex(truck_model);
   glmMakeVBOs(truck_model);

     /* carregando o moinho */
    windmIll_model = glmReadOBJ(windmIll_file);
   objScale = glmUnitize(windmIll_model);
   glmFacetNormals(windmIll_model);
   if (windmIll_model->numnormals == 0) {
      GLfloat smoothing_angle = 90.0;
      glmVertexNormals(windmIll_model, smoothing_angle);
   }

   glmLoadTextures(windmIll_model);
   glmReIndex(windmIll_model);
   glmMakeVBOs(windmIll_model);
}

static void init(void){
   glClearColor(1.0, 1.0, 1.0, 0.0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glEnable(GL_NORMALIZE);
}

static void reshape(int width, int height) {
   float ar = 0.5 * (float) width / (float) height; //razão de aspecto
   WinWidth = width; //largura da janela
   WinHeight = height;  //atura da janela
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-ar, ar, -0.5, 0.5, 1.0, 300.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -3.0);
}

static void display(void){
   GLfloat rot[4][4];
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /// carregando o courage
   glPushMatrix();
      glTranslatef(0.0, -3.2, -View.Distance);
      glRotatef(View.rotX,1,0,0);
//	  glRotatef(View.rotY,0,1,0);
      glScalef(Scale/4, Scale/4, Scale/4);
      glmDrawVBO(courage_model);
   glPopMatrix();

   /// carregando o moinho
   glPushMatrix();
      glTranslatef(5.0, 4.0, -View.Distance+28);
      glRotatef(View.rotX,1,0,0);
//	  glRotatef(View.rotY,0,1,0);
      glScalef(2*Scale, 2*Scale, 2*Scale);
      glmDrawVBO(windmIll_model);
   glPopMatrix();

   /// carregando o carro
   glPushMatrix();
      glTranslatef(-5.0, -3.2, -View.Distance+10);
      glRotatef(View.rotX,1,0,0);
//	  glRotatef(View.rotY,0,1,0);
	  glRotatef(45, 0.0, 1.0, 0.0);
      glScalef(Scale, Scale, Scale);
      glmDrawVBO(truck_model);
   glPopMatrix();

    /// carregando a casa
   glPushMatrix();
      glTranslatef(5.0, -2.0, -View.Distance);
      glRotatef(View.rotX,1,0,0);
	  //glRotatef(View.rotY,0,1,0);
      glScalef(2.5*Scale, 2.5*Scale, 2.5*Scale );
      glmDrawVBO(house_model);
   glPopMatrix();
   glutSwapBuffers();
}

/**
 * Evento de Mouse
 */
#define SENS_ROT	15.0
static void Mouse(int button, int state, int x, int y){
    if (button == GLUT_LEFT_BUTTON) { //mouse - botão da esquera rotaciona o objeto
        if (state == GLUT_DOWN) {
            View.x_ini = x;
            View.y_ini = y;
            View.rotX_ini = View.rotX;
            View.rotY_ini = View.rotY;
            View.Rotating = GL_TRUE;
        } else if (state == GLUT_UP) {
            View.Rotating = GL_FALSE;
        }
    } else if (button == GLUT_MIDDLE_BUTTON) {  //mouse - botão do meio aproxima ou afasta o objeto (translação)
        if (state == GLUT_DOWN) {
            View.StartX = x;
            View.StartY = y;
            View.StartDistance = View.Distance;
            View.Translating = GL_TRUE;
        } else if (state == GLUT_UP) {
            View.Translating = GL_FALSE;
        }
   }
}

/**
 * Evento de movimento do mouse
 */

static void Motion(int x, int y) {
   int i;
   if (View.Rotating) {
        int deltax = View.x_ini - x;
        int deltay = View.y_ini - y;
		// E modifica ângulos
		View.rotY = View.rotY_ini - deltax/SENS_ROT;
		View.rotX = View.rotX_ini - deltay/SENS_ROT;

      glutPostRedisplay();
   } else if (View.Translating) {
      float dz = 0.02 * (y - View.StartY);
      View.Distance = View.StartDistance + dz;
      glutPostRedisplay();
   }
}


static void DoFeatureChecks(void){
   if (!GLEW_VERSION_2_0) {
      /* check for individual extensions */
      if (!GLEW_ARB_texture_cube_map) {
         printf("Sorry, GL_ARB_texture_cube_map is required.\n");
         exit(1);
      }
      if (!GLEW_ARB_vertex_shader) {
         printf("Sorry, GL_ARB_vertex_shader is required.\n");
         exit(1);
      }
      if (!GLEW_ARB_fragment_shader) {
         printf("Sorry, GL_ARB_fragment_shader is required.\n");
         exit(1);
      }
      if (!GLEW_ARB_vertex_buffer_object) {
         printf("Sorry, GL_ARB_vertex_buffer_object is required.\n");
         exit(1);
      }
   }
   if (!ShadersSupported()) {
      printf("Sorry, GLSL is required\n");
      exit(1);
   }
}

void SpecialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT :
            View.x_ini = x;
            View.y_ini = y;
            View.rotX_ini = View.rotX;
            View.rotY_ini = View.rotY;
            obsX -=10;
			break;
        case GLUT_KEY_RIGHT :
            obsX +=10;
			break;
        case GLUT_KEY_UP :
            obsY +=10;
			break;
        case GLUT_KEY_DOWN :
            obsY -=10;
            break;
        case GLUT_KEY_HOME :
            obsZ +=10;
            break;
        case GLUT_KEY_END :
            obsZ -=10;
            break;
		}

    printf("Tecla apertada");
    glLoadIdentity();
    gluLookAt(obsX,obsY,obsZ, 0,0,0, 1,0,0);
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(WinWidth, WinHeight);

    courage_file = "courage_apply.obj";
    house_file = "houseA_obj.obj";
    truck_file = "Truck_obj.obj";
    windmIll_file = "windmIll.obj";

    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("objview");
gluPerspective(70, 1.0, 1.0, 50);   /// criando a projecao perspectiva
    glewInit();

    DoFeatureChecks();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    //glutMouseFunc(Mouse);
    //glutMotionFunc(Motion);
    glutSpecialFunc(SpecialKeys);

    InitViewInfo(&View);

    read_model();

    init();
    glutMainLoop();

    return 0;
}
