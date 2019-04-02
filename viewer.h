#ifndef VIEWER_H
#define VIEWER_H

// GLEW lib: needs to be included first!!
#include <GL/glew.h>

// OpenGL library
#include <GL/gl.h>

// OpenGL Utility library
#include <GL/glu.h>

// OpenGL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <stack>

#include "camera.h"
#include "meshLoader.h"
#include "shader.h"
#include "grid.h"

class Viewer : public QGLWidget {
 public:
  Viewer(char *filename,
	 const QGLFormat &format=QGLFormat::defaultFormat());
  ~Viewer();

 protected :
  virtual void paintGL();
  virtual void initializeGL();
  virtual void resizeGL(int width,int height);
  virtual void keyPressEvent(QKeyEvent *ke);
  virtual void mousePressEvent(QMouseEvent *me);
  virtual void mouseMoveEvent(QMouseEvent *me);

 private:
  void createVAO();
  void deleteVAO();
  void drawVAOTerrain();
  void drawVAOQuad();

  void createFBOPerlin();
  void createFBONormal();
  void deleteFBO();
  void initFBOPerlin();
  void initFBONormal();


  void createShaderPerlin();
  void createShaderNormal();
  void createShaderTest();
  void deleteShaders();
  void enableShaderPerlin();
  void enableShaderTest();
  void enableShaderNormal();
  void disableShader();
  void drawPerlin();
  void drawNormal();

  QTimer        *_timer;    // timer that controls the animation
  unsigned int   _currentshader; // current shader index

  //Mesh   *_mesh;   // the mesh
  Camera *_cam;    // the camera

  glm::vec3 _light; // light direction
  bool      _mode;  // camera motion or light motion
  bool      _showPerlinMap;

  std::vector<std::string> _vertexFilenames;   // all vertex filenames
  std::vector<std::string> _fragmentFilenames; // all fragment filenames
  std::vector<Shader *>    _shaders;           // all the shaders

  Shader *_shaderPerlin;
  Shader *_shaderNormal;
  Shader *_shaderTest;


  GLuint _buffers[3];
  GLuint _vaoTerrain;
  GLuint _vaoQuad;
  GLuint _terrain[2];
  GLuint _quad;
  Grid *_grid;
  GLuint resol=512;

  // render texture ids
  GLuint _texPerlin;
  GLuint _texNormal;

  // fbo id
  GLuint _fboPerlin;
  GLuint _fboNormal;

};

#endif // VIEWER_H