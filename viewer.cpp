#include "viewer.h"

#include <math.h>
#include <iostream>
#include "meshLoader.h"
#include <QTime>
#include "grid.h"

using namespace std;


Viewer::Viewer(char *filename,const QGLFormat &format)
  : QGLWidget(format),
    _timer(new QTimer(this)),
    _currentshader(0),
    _showPerlinMap(false),
    _light(glm::vec3(0,0,1)),
    _mode(false) {

  setlocale(LC_ALL,"C");

  // load a mesh into the CPU memory
  //_mesh = new Mesh(filename);

  // create a camera (automatically modify model/view matrices according to user interactions)
  //cam  = new Camera(_mesh->radius,glm::vec3(_mesh->center[0],_mesh->center[1],_mesh->center[2]));
  _cam = new Camera();
  _timer->setInterval(10);
  connect(_timer,SIGNAL(timeout()),this,SLOT(updateGL()));
}

Viewer::~Viewer() {
  delete _timer;
  //delete _mesh;
  delete _cam;
  delete _grid;
  deleteShaders();

  deleteVAO();
}


void Viewer::createVAO() {
  //the variable _grid should be an instance of Grid
  _grid = new Grid();
  //the .h file should contain the following VAO/buffer ids
  //GLuint _vaoTerrain;
  //GLuint _vaoQuad;
  //GLuint _terrain[2];
  //GLuint _quad;

  const GLfloat quadData[] = {
    -1.0f,-1.0f,0.0f, 1.0f,-1.0f,0.0f, -1.0f,1.0f,0.0f, -1.0f,1.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,1.0f,0.0f };

  glGenBuffers(2,_terrain);
  glGenBuffers(1,&_quad);
  glGenVertexArrays(1,&_vaoTerrain);
  glGenVertexArrays(1,&_vaoQuad);

  // create the VBO associated with the grid (the terrain)
  glBindVertexArray(_vaoTerrain);
  glBindBuffer(GL_ARRAY_BUFFER,_terrain[0]); // vertices
  glBufferData(GL_ARRAY_BUFFER,_grid->nbVertices()*3*sizeof(float),_grid->vertices(),GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_terrain[1]); // indices
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,_grid->nbFaces()*3*sizeof(int),_grid->faces(),GL_STATIC_DRAW);

  // create the VBO associated with the screen quad
  glBindVertexArray(_vaoQuad);
  glBindBuffer(GL_ARRAY_BUFFER,_quad); // vertices
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadData),quadData,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
  glEnableVertexAttribArray(0);
}

void Viewer::deleteVAO() {
  glDeleteBuffers(2,_terrain);
  glDeleteBuffers(1,&_quad);
  glDeleteVertexArrays(1,&_vaoTerrain);
  glDeleteVertexArrays(1,&_vaoQuad);

}



void Viewer::drawVAOTerrain() {
  // activate the VAO, draw the associated triangles and desactivate the VAO
  glBindVertexArray(_vaoTerrain);
  glDrawArrays(GL_TRIANGLES,0,6);
  //glDrawElements(GL_TRIANGLES,3*_mesh->nb_faces,GL_UNSIGNED_INT,(void *)0);
  glBindVertexArray(0);
}
void Viewer::drawVAOQuad() {
  // activate the VAO, draw the associated triangles and desactivate the VAO
  glBindVertexArray(_vaoQuad);
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindVertexArray(0);
}

void Viewer::createFBOPerlin(){
    // Ids needed for the FBO and associated textures
glGenFramebuffers(1,&_fboPerlin);
glGenTextures(1,&_texPerlin);


}

void Viewer::createFBONormal(){
    // Ids needed for the FBO and associated textures
glGenFramebuffers(1,&_fboNormal);
glGenTextures(1,&_texNormal);

}

void Viewer::initFBOPerlin() {

  // create the texture for rendering depth values
  glBindTexture(GL_TEXTURE_2D,_texPerlin);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,resol,resol,0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // attach textures to framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER,_fboPerlin);
  glBindTexture(GL_TEXTURE_2D,_texPerlin);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_texPerlin,0);

    // test if everything is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      cout << "Warning: FBO not complete!" << endl;


  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Viewer::initFBONormal() {

  // create the texture for rendering depth values
  glBindTexture(GL_TEXTURE_2D,_texNormal);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,resol,resol,0,GL_RGBA,GL_FLOAT,NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // attach textures to framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER,_fboNormal);
  glBindTexture(GL_TEXTURE_2D,_texNormal);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,_texNormal,0);

    // test if everything is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      cout << "Warning: FBO not complete!" << endl;


  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Viewer::deleteFBO() {
  // delete all FBO Ids
  glDeleteFramebuffers(1,&_fboPerlin);
  glDeleteTextures(1,&_texPerlin);
  // delete all FBO Ids
  glDeleteFramebuffers(1,&_fboNormal);
  glDeleteTextures(1,&_texNormal);
}




void Viewer::createShaderPerlin() {
  _shaderPerlin = new Shader();

    _shaderPerlin->load("shaders/noise.vert","shaders/noise.frag");
  // ***************************
}
void Viewer::createShaderNormal() {
    _shaderNormal = new Shader();
    _shaderNormal->load("shaders/normal.vert","shaders/normal.frag");

}
void Viewer::createShaderShowNormal() {
    _shaderShowNormal = new Shader();
    _shaderShowNormal->load("shaders/shownormal.vert","shaders/shownormal.frag");

}
void Viewer::createShaderTest() {
    _shaderTest = new Shader();

    _shaderTest->load("shaders/test.vert","shaders/test.frag");

}

void Viewer::deleteShaders(){
  delete _shaderNormal;
  delete _shaderShowNormal;
  delete _shaderPerlin;
  delete _shaderTest;

}





void Viewer::enableShaderNormal() {
  // current shader ID
  GLuint id = _shaderNormal->id();
  // activate the current shader
  glUseProgram(id);

}
void Viewer::enableShaderShowNormal() {
  // current shader ID
  GLuint id = _shaderShowNormal->id();
  // activate the current shader
  glUseProgram(id);

}
void Viewer::enableShaderPerlin() {
  // current shader ID
  GLuint id = _shaderPerlin->id();
  // activate the current shader
  glUseProgram(id);

}

void Viewer::enableShaderTest() {
  // current shader ID
  GLuint id = _shaderTest->id();
  // activate the current shader
  glUseProgram(id);

}

void Viewer::disableShader() {
  // desactivate all shaders
  glUseProgram(0);
}



void Viewer::paintGL() {
  // draw Perlin noise in FBO
 glBindFramebuffer(GL_FRAMEBUFFER,_fboPerlin);
glViewport(0,0,resol,resol);
  // tell the GPU to use this specified shader and send custom variables (matrices and others)
  enableShaderPerlin();
    // clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // actually draw the scene
  drawVAOQuad();
  // tell the GPU to stop using this shader
  disableShader();
 glBindFramebuffer(GL_FRAMEBUFFER,0);


 glBindFramebuffer(GL_FRAMEBUFFER,_fboNormal);
  glViewport(0,0,resol,resol);
  // activate the rendering shader
  enableShaderNormal();

  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawNormal();
  disableShader();
  glBindFramebuffer(GL_FRAMEBUFFER,0);


glViewport(0,0,width(),height());
  enableShaderShowNormal();
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawShowNormal();

  if(_showPerlinMap) {
    enableShaderTest();
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //-> créer le shader qui affiche le contenu du FBO
    // on envoie le bruit de perlin au shader test
    drawPerlin();
    //drawNormal();
  }
  disableShader();



}

void Viewer::drawPerlin() {
  // send depth texture

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texPerlin);
  glUniform1i(glGetUniformLocation(_shaderTest->id(),"depthmap"),0);
// draw the quad
  drawVAOQuad();
}

void Viewer::drawNormal(){
  // send depth texture

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texPerlin);
  glUniform1i(glGetUniformLocation(_shaderNormal->id(),"heightmap"),0);
// draw the quad
  drawVAOQuad();
}
void Viewer::drawShowNormal(){
  // send depth texture

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_texNormal);
  glUniform1i(glGetUniformLocation(_shaderShowNormal->id(),"heightmap"),0);
// draw the quad
  drawVAOQuad();
  // disable VAO
  glBindVertexArray(0);
}

void Viewer::resizeGL(int width,int height) {
  _cam->initialize(width,height,false);
  glViewport(0,0,width,height);
  updateGL();
}

void Viewer::mousePressEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(me->button()==Qt::LeftButton) {
    _cam->initRotation(p);
    _mode = false;
  } else if(me->button()==Qt::MidButton) {
    _cam->initMoveZ(p);
    _mode = false;
  } else if(me->button()==Qt::RightButton) {
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
    _mode = true;
  }

  updateGL();
}

void Viewer::mouseMoveEvent(QMouseEvent *me) {
  const glm::vec2 p((float)me->x(),(float)(height()-me->y()));

  if(_mode) {
    // light mode
    _light[0] = (p[0]-(float)(width()/2))/((float)(width()/2));
    _light[1] = (p[1]-(float)(height()/2))/((float)(height()/2));
    _light[2] = 1.0f-std::max(fabs(_light[0]),fabs(_light[1]));
    _light = glm::normalize(_light);
  } else {
    // camera mode
    _cam->move(p);
  }

  updateGL();
}

void Viewer::keyPressEvent(QKeyEvent *ke) {

  // key a: play/stop animation
  if(ke->key()==Qt::Key_A) {
    if(_timer->isActive())
      _timer->stop();
    else
      _timer->start();
  }

  // key i: init camera
  if(ke->key()==Qt::Key_I) {
    _cam->initialize(width(),height(),true);
  }

  // key f: compute FPS
  if(ke->key()==Qt::Key_F) {
    int elapsed;
    QTime timer;
    timer.start();
    unsigned int nb = 500;
    for(unsigned int i=0;i<nb;++i) {
      paintGL();
    }
    elapsed = timer.elapsed();
    double t = (double)nb/((double)elapsed);
    cout << "FPS : " << t*1000.0 << endl;
  }

  // key r: reload shaders
  if(ke->key()==Qt::Key_R) {
    _shaderPerlin->reload("shaders/noise.vert","shaders/noise.frag");
    _shaderNormal->reload("shaders/normal.vert","shaders/normal.frag");
    _shaderShowNormal->reload("shaders/shownormal.vert","shaders/shownormal.frag");
    _shaderTest->reload("shaders/test.vert","shaders/test.frag");

  }

  // space: next shader
  if(ke->key()==Qt::Key_Space) {
    _currentshader = (_currentshader+1)%_shaders.size();
  }

  // key S: show the perlin map
  if(ke->key()==Qt::Key_S) {
    _showPerlinMap = !_showPerlinMap;
  }



  updateGL();
}

void Viewer::initializeGL() {
  // make this window the current one
  makeCurrent();

  // init and chack glew
  glewExperimental = GL_TRUE;

  if(glewInit()!=GLEW_OK) {
    cerr << "Warning: glewInit failed!" << endl;
  }


  // init OpenGL settings
  glClearColor(0.0,0.0,0.0,1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glViewport(0,0,width(),height());

  // initialize camera
  _cam->initialize(width(),height(),true);

  // load shader files
  createShaderPerlin();
 createShaderNormal();
 createShaderShowNormal();
  createShaderTest();


  // VAO creation
  createVAO();

// create/init FBO Perlin
  createFBOPerlin();
  initFBOPerlin();
// create/init FBO Normal
 createFBONormal();
  initFBONormal();

  // starts the timer
  _timer->start();
}
