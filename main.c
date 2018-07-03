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

float distancia_aviao = -300;
float angulo_aviao = 0;

float z_covarde = -0.8;
float x_covarde = 0;
float angulo_covarde = 0;

float z_car = -5;
float x_car = 10;
float angulo_car = -90;

int objeto = 0;

static void updateModels(void);

static char *Model_file = NULL;		/* nome do arquivo do objeto */
static GLMmodel *Model;             /* modelo do objeto*/

/* de cada objeto */
static char *courage_file;
static GLMmodel *courage_model;

static char *chao_file;
static GLMmodel *chao_model;

static char *lua_file;
static GLMmodel *lua_model;

static char *windmIll_file;
static GLMmodel *windmIll_model;

static char *casa_file;
static GLMmodel *casa_model;

static char *car_file;
static GLMmodel *car_model;

static char *aviao_file;
static GLMmodel *aviao_model;

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

void criaLua(){
    //limpa o buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    //define material da superfície
    float kd[4] = {0.65f, 0.65f, 0.0f, 1.0f};
    float ks[4] = {0.9f, 0.9f, 0.9f, 1.0f};
    float ns = 5.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT, GL_SHININESS, ns);

    //define que a matrix é a a model view
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(-4, 8, -20);
    glutSolidSphere(1.5, 40, 40);
}

static void read_model(void){
   float objScale;

   // lendo o modelo
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

     /// carregando o moinho
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

    /// carregando a pedra
   chao_model = glmReadOBJ(chao_file);
   objScale = glmUnitize(chao_model);
   glmFacetNormals(chao_model);
   if (chao_model->numnormals == 0) {
      GLfloat smoothing_angle = 90.0;
      glmVertexNormals(chao_model, smoothing_angle);
   }

   glmLoadTextures(chao_model);
   glmReIndex(chao_model);
   glmMakeVBOs(chao_model);


    /// carregando o aviao
   aviao_model = glmReadOBJ(aviao_file);
   objScale = glmUnitize(aviao_model);
   glmFacetNormals(aviao_model);
   if (aviao_model->numnormals == 0) {
      GLfloat smoothing_angle = 90.0;
      glmVertexNormals(aviao_model, smoothing_angle);
   }

   glmLoadTextures(aviao_model);
   glmReIndex(aviao_model);
   glmMakeVBOs(aviao_model);

    /// carregando a casa
   casa_model = glmReadOBJ(casa_file);
   objScale = glmUnitize(casa_model);
   glmFacetNormals(casa_model);
   if (casa_model->numnormals == 0) {
      GLfloat smoothing_angle = 90.0;
      glmVertexNormals(casa_model, smoothing_angle);
   }

   glmLoadTextures(casa_model);
   glmReIndex(casa_model);
   glmMakeVBOs(casa_model);

    /// carregando a car
   car_model = glmReadOBJ(car_file);
   objScale = glmUnitize(car_model);
   glmFacetNormals(car_model);
   if (car_model->numnormals == 0) {
      GLfloat smoothing_angle = 90.0;
      glmVertexNormals(car_model, smoothing_angle);
   }

   glmLoadTextures(car_model);
   glmReIndex(car_model);
   glmMakeVBOs(car_model);
}

static void init(void){
   glClearColor(1.0, 1.0, 0.6, 0.0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glEnable(GL_NORMALIZE);

    lighting();
}

void lighting(){
    GLfloat luzlua[4] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat black[4] = {0.0f, 0.0f, 0.0f, 1.0f};

    glLightfv(GL_LIGHT0,GL_POSITION,luzlua);
    glLightfv(GL_LIGHT0,GL_AMBIENT,black);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,white);
    glLightfv(GL_LIGHT0,GL_SPECULAR,white);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

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


    criaLua();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /// carregando a chao
   glPushMatrix();
      glTranslatef(1.0, -2.5, -View.Distance+5);
      glRotatef(View.rotX,1,0,0);
//	  glRotatef(View.rotY,0,1,0);
      glScalef(Scale, Scale/2, 17*Scale);
      glmDrawVBO(chao_model);
   glPopMatrix();

    /// carregando o courage
   glPushMatrix();
      glTranslatef(x_covarde, -0.8, -View.Distance+z_covarde);
      glRotatef(angulo_covarde, 0.0, 1.0, 0.0);
      //glRotatef(View.rotX,1,0,0);
//	  glRotatef(View.rotY,0,1,0);
      glScalef(Scale/4, Scale/4, Scale/4);
      glmDrawVBO(courage_model);
   glPopMatrix();

   /// carregando o moinho
   glPushMatrix();
      glTranslatef(10.0, 4.0, -View.Distance-30);
//      glRotatef(View.rotX,1,0,0);
//	  glRotatef(View.rotY,0,1,0);
      glScalef(2*Scale, 2*Scale, 2*Scale);
      glmDrawVBO(windmIll_model);
   glPopMatrix();

   /// carregando o aviao
   glPushMatrix();
      glTranslatef(-10.0, 14.0, -View.Distance+distancia_aviao);
      glRotatef(angulo_aviao, 0.0, 0.0, 1.0);
//    glRotatef(View.rotX,1,0,0);
//	  glRotatef(View.rotY,0,1,0);
      glScalef(Scale, Scale, Scale);
      glmDrawVBO(aviao_model);
   glPopMatrix();

   /// carregando a casa
   glPushMatrix();
      glTranslatef(-8.0, 0.0, -View.Distance-5);
      //glRotatef(View.rotX,0,1,0);
	  //glRotatef(View.rotY,0,1,0);
      glScalef(Scale, Scale, Scale);
      glmDrawVBO(casa_model);
   glPopMatrix();

   /// carregando o carro
   glPushMatrix();
      glTranslatef(x_car, 0.0, -View.Distance+z_car);
      glRotatef(angulo_car, 0.0, 1.0, 0.0);        /// rotacionando em 45 graus no eixo x
      //glRotatef(View.rotX,0,1,0);
	  //glRotatef(View.rotY,0,1,0);
      glScalef(Scale, Scale, Scale);
      glmDrawVBO(car_model);
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
    // covarde
    if(objeto == 0){
        switch (key) {
            case GLUT_KEY_LEFT :
                x_covarde -= 0.5;
                angulo_covarde = 270;
                break;
            case GLUT_KEY_RIGHT :
                x_covarde += 0.5;
                angulo_covarde = 90;
                break;
            case GLUT_KEY_UP :
                z_covarde -= 0.5;
                angulo_covarde = 180;
                break;
            case GLUT_KEY_DOWN :
                z_covarde += 0.5;
                angulo_covarde = 0;
                break;
		}
    } else if (objeto == 1){
        switch (key) {
            case GLUT_KEY_LEFT :
                x_car -= 0.5;
                angulo_car = 180;
                break;
            case GLUT_KEY_RIGHT :
                x_car += 0.5;
                angulo_car = 0;
                break;
            case GLUT_KEY_UP :
                z_car -= 0.5;
                angulo_car = 90;
                break;
            case GLUT_KEY_DOWN :
                z_car += 0.5;
                angulo_car = 270;
                break;
		}
    }

    glutPostRedisplay();
}

void opcao(int key, int x, int y){
    switch(key){
        case 'c':
            objeto = 0; // covarde
            break;
        case 'b':
            objeto = 1; // bobcat
            break;
    }
}

void spinDisplay(void){
   //playSound("Airplane.wav");
   distancia_aviao = distancia_aviao + 1;
   if (distancia_aviao == 0){
      distancia_aviao = -300;
      angulo_aviao = 0;
    }

    if(distancia_aviao > -180)
        angulo_aviao += 1;

   glutPostRedisplay();
}

int playSound(char *filename) {
    char command[256];
    int status;

    /* create command to execute */
    sprintf(command, "aplay -c 1 -q -t wav %s", filename);

    /* play sound */
    status = system(command);
    return status;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(WinWidth, WinHeight);

    courage_file = "courage_apply.obj";
    windmIll_file = "windmIll.obj";
    chao_file = "rock.obj";
    casa_file = "houseA_obj.obj";
    car_file = "bobcat.obj";
    aviao_file = "plane.obj";

    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("objview");
    glewInit();

    DoFeatureChecks();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(opcao);            // captura as teclas 'R' e 'T'
    //glutMouseFunc(Mouse);
    //glutMotionFunc(Motion);
    glutIdleFunc(spinDisplay);
    glutSpecialFunc(SpecialKeys);

    InitViewInfo(&View);

    read_model();

    init();
    glutMainLoop();

    return 0;
}
