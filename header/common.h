#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include <GLFW/glfw3.h>
#include <FreeImage.h>

using namespace std;
using namespace glm;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

/* Define a 3D point */
typedef struct {
  vec3 pos;
  vec3 color;
  vec3 v;
  float m;
} Point;

class Mesh {
public:
  // mesh data
  Assimp::Importer importer;
  const aiScene *scene;

  // opengl data
  vector<GLuint> vboVtxs, vboUvs, vboNmls;
  vector<GLuint> vaos;

  GLuint shader;
  GLuint tboBase, tboNormal;
  GLint uniModel, uniView, uniProjection;
  GLint uniLightV, uniLightP;
  GLint uniEyePoint, uniLightColor, uniLightPosition;
  GLint uniTexBase, uniTexNormal, uniTexDepth;

  // aabb
  vec3 min, max;

  mat4 model, view, projection;

  /* Constructors */
  Mesh(const string);
  ~Mesh();

  /* Member functions */
  void initBuffers();
  void initShader();
  void initUniform();
  void draw(mat4, mat4, mat4, vec3, vec3, vec3, int, int);
  void setTexture(GLuint &, int, const string, FREE_IMAGE_FORMAT);

  void translate(vec3);
  void scale(vec3);
  void rotate(vec3);
  void findAABB();
};

string readFile(const string);
void printLog(GLuint &);
GLint myGetUniformLocation(GLuint &, string);
GLuint buildShader(string, string);
GLuint compileShader(string, GLenum);
GLuint linkShader(GLuint, GLuint);
void drawBox(vec3, vec3);
void drawPoints(vector<Point> &);
