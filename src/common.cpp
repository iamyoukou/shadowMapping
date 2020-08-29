#include "common.h"

std::string readFile(const std::string fileName) {
  std::ifstream in;
  in.open(fileName.c_str());
  std::stringstream ss;
  ss << in.rdbuf();
  std::string sOut = ss.str();
  in.close();

  return sOut;
}

// return a shader executable
GLuint buildShader(string vsDir, string fsDir) {
  GLuint vs, fs;
  GLint linkOk;
  GLuint exeShader;

  // compile
  vs = compileShader(vsDir, GL_VERTEX_SHADER);
  fs = compileShader(fsDir, GL_FRAGMENT_SHADER);

  // link
  exeShader = linkShader(vs, fs);

  return exeShader;
}

GLuint compileShader(string fileName, GLenum type) {
  /* read source code */
  string sTemp = readFile(fileName);
  string info;
  const GLchar *source = sTemp.c_str();

  switch (type) {
  case GL_VERTEX_SHADER:
    info = "Vertex";
    break;
  case GL_FRAGMENT_SHADER:
    info = "Fragment";
    break;
  }

  if (source == NULL) {
    std::cout << info << " Shader : Can't read shader source file."
              << std::endl;
    return 0;
  }

  const GLchar *sources[] = {source};
  GLuint objShader = glCreateShader(type);
  glShaderSource(objShader, 1, sources, NULL);
  glCompileShader(objShader);

  GLint compile_ok;
  glGetShaderiv(objShader, GL_COMPILE_STATUS, &compile_ok);
  if (compile_ok == GL_FALSE) {
    std::cout << info << " Shader : Fail to compile." << std::endl;
    printLog(objShader);
    glDeleteShader(objShader);
    return 0;
  }

  return objShader;
}

GLuint linkShader(GLuint vsObj, GLuint fsObj) {
  GLuint exe;
  GLint linkOk;

  exe = glCreateProgram();
  glAttachShader(exe, vsObj);
  glAttachShader(exe, fsObj);
  glLinkProgram(exe);

  // check result
  glGetProgramiv(exe, GL_LINK_STATUS, &linkOk);

  if (linkOk == GL_FALSE) {
    std::cout << "Failed to link shader program." << std::endl;
    printLog(exe);
    glDeleteProgram(exe);

    return 0;
  }

  return exe;
}

void printLog(GLuint &object) {
  GLint log_length = 0;
  if (glIsShader(object)) {
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  } else if (glIsProgram(object)) {
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  } else {
    cerr << "printlog: Not a shader or a program" << endl;
    return;
  }

  char *log = (char *)malloc(log_length);

  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);

  cerr << log << endl;
  free(log);
}

GLint myGetUniformLocation(GLuint &prog, string name) {
  GLint location;
  location = glGetUniformLocation(prog, name.c_str());
  if (location == -1) {
    cerr << "Could not bind uniform : " << name << ". "
         << "Did you set the right name? "
         << "Or is " << name << " not used?" << endl;
  }

  return location;
}

void drawBox(vec3 min, vec3 max) {
  // 8 corners
  GLfloat aVtxs[]{
      min.x, max.y, min.z, // 0
      min.x, min.y, min.z, // 1
      max.x, min.y, min.z, // 2
      max.x, max.y, min.z, // 3
      min.x, max.y, max.z, // 4
      min.x, min.y, max.z, // 5
      max.x, min.y, max.z, // 6
      max.x, max.y, max.z  // 7
  };

  // vertex color
  // GLfloat colorArray[] = {color.x, color.y, color.z, color.x, color.y,
  // color.z,
  //                         color.x, color.y, color.z, color.x, color.y,
  //                         color.z, color.x, color.y, color.z, color.x,
  //                         color.y, color.z, color.x, color.y, color.z,
  //                         color.x, color.y, color.z};

  // vertex index
  GLushort aIdxs[] = {
      0, 1, 2, 3, // front face
      4, 7, 6, 5, // back
      4, 0, 3, 7, // up
      5, 6, 2, 1, // down
      0, 4, 5, 1, // left
      3, 2, 6, 7  // right
  };

  // prepare buffers to draw
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vboVtx;
  glGenBuffers(1, &vboVtx);
  glBindBuffer(GL_ARRAY_BUFFER, vboVtx);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8 * 3, aVtxs, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // GLuint vboColor;
  // glGenBuffers(1, &vboColor);
  // glBindBuffer(GL_ARRAY_BUFFER, vboColor);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(colorArray), colorArray,
  // GL_STATIC_DRAW); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  // glEnableVertexAttribArray(1);

  GLuint ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(aIdxs), aIdxs, GL_STATIC_DRAW);

  // draw box
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  for (size_t i = 0; i < 6; i++) {
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT,
                   (GLvoid *)(sizeof(GLushort) * 4 * i));
  }

  glDeleteBuffers(1, &vboVtx);
  // glDeleteBuffers(1, &vboColor);
  glDeleteBuffers(1, &ibo);
  glDeleteVertexArrays(1, &vao);
}

void drawPoints(vector<Point> &pts) { // array data
  int nOfPs = pts.size();

  GLfloat *aPos = new GLfloat[nOfPs * 3];
  GLfloat *aColor = new GLfloat[nOfPs * 3];

  // implant data
  for (size_t i = 0; i < nOfPs; i++) {
    // positions
    Point &p = pts[i];
    aPos[i * 3 + 0] = p.pos.x;
    aPos[i * 3 + 1] = p.pos.y;
    aPos[i * 3 + 2] = p.pos.z;

    // colors
    aColor[i * 3 + 0] = p.color.r;
    aColor[i * 3 + 1] = p.color.g;
    aColor[i * 3 + 2] = p.color.b;
  }

  // selete vao
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // position
  GLuint vboPos;
  glGenBuffers(1, &vboPos);
  glBindBuffer(GL_ARRAY_BUFFER, vboPos);
  glBufferData(GL_ARRAY_BUFFER, nOfPs * 3 * sizeof(GLfloat), aPos,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // color
  GLuint vboColor;
  glGenBuffers(1, &vboColor);
  glBindBuffer(GL_ARRAY_BUFFER, vboColor);
  glBufferData(GL_ARRAY_BUFFER, nOfPs * 3 * sizeof(GLfloat), aColor,
               GL_STREAM_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  glDrawArrays(GL_POINTS, 0, nOfPs);

  // release
  delete[] aPos;
  delete[] aColor;
  glDeleteBuffers(1, &vboPos);
  glDeleteBuffers(1, &vboColor);
  glDeleteVertexArrays(1, &vao);
}

/* Mesh class */
Mesh::Mesh(const string fileName) {
  // import mesh
  scene = importer.ReadFile(fileName, aiProcess_CalcTangentSpace);

  initBuffers();
  initShader();
  initUniform();
}

Mesh::~Mesh() {
  for (size_t i = 0; i < scene->mNumMeshes; i++) {
    glDeleteBuffers(1, &vboVtxs[i]);
    glDeleteBuffers(1, &vboUvs[i]);
    glDeleteBuffers(1, &vboNmls[i]);
    glDeleteVertexArrays(1, &vaos[i]);
  }
}

void Mesh::initShader() {
  shader = buildShader("./shader/vsScene.glsl", "./shader/fsScene.glsl");
}

void Mesh::initUniform() {
  uniModel = myGetUniformLocation(shader, "M");
  uniView = myGetUniformLocation(shader, "V");
  uniProjection = myGetUniformLocation(shader, "P");
  uniEyePoint = myGetUniformLocation(shader, "eyePoint");
  uniLightColor = myGetUniformLocation(shader, "lightColor");
  uniLightPosition = myGetUniformLocation(shader, "lightPosition");
  uniTexDepth = myGetUniformLocation(shader, "texDepth");
}

void Mesh::initBuffers() {
  // for each mesh
  for (size_t i = 0; i < scene->mNumMeshes; i++) {
    const aiMesh *mesh = scene->mMeshes[i];
    int numVtxs = mesh->mNumVertices;

    // numVertices * numComponents
    GLfloat *aVtxCoords = new GLfloat[numVtxs * 3];
    GLfloat *aUvs = new GLfloat[numVtxs * 2];
    GLfloat *aNormals = new GLfloat[numVtxs * 3];

    for (size_t j = 0; j < numVtxs; j++) {
      aiVector3D &vtx = mesh->mVertices[j];
      aVtxCoords[j * 3 + 0] = vtx.x;
      aVtxCoords[j * 3 + 1] = vtx.y;
      aVtxCoords[j * 3 + 2] = vtx.z;

      aiVector3D &nml = mesh->mNormals[j];
      aNormals[j * 3 + 0] = nml.x;
      aNormals[j * 3 + 1] = nml.y;
      aNormals[j * 3 + 2] = nml.z;

      aiVector3D &uv = mesh->mTextureCoords[0][j];
      aUvs[j * 2 + 0] = uv.x;
      aUvs[j * 2 + 1] = uv.y;
    }

    // vao
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    vaos.push_back(vao);

    // vbo for vertex
    GLuint vboVtx;
    glGenBuffers(1, &vboVtx);
    glBindBuffer(GL_ARRAY_BUFFER, vboVtx);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVtxs * 3, aVtxCoords,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    vboVtxs.push_back(vboVtx);

    // vbo for uv
    GLuint vboUv;
    glGenBuffers(1, &vboUv);
    glBindBuffer(GL_ARRAY_BUFFER, vboUv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVtxs * 2, aUvs,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    vboUvs.push_back(vboUv);

    // vbo for normal
    GLuint vboNml;
    glGenBuffers(1, &vboNml);
    glBindBuffer(GL_ARRAY_BUFFER, vboNml);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVtxs * 3, aNormals,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    vboNmls.push_back(vboNml);

    // delete client data
    delete[] aVtxCoords;
    delete[] aUvs;
    delete[] aNormals;
  } // end for each mesh
}

void Mesh::setTexture(GLuint &tbo, int texUnit, const string texDir,
                      FREE_IMAGE_FORMAT imgType) {
  glActiveTexture(GL_TEXTURE0 + texUnit);

  FIBITMAP *texImage =
      FreeImage_ConvertTo24Bits(FreeImage_Load(imgType, texDir.c_str()));

  glGenTextures(1, &tbo);
  glBindTexture(GL_TEXTURE_2D, tbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(texImage),
               FreeImage_GetHeight(texImage), 0, GL_BGR, GL_UNSIGNED_BYTE,
               (void *)FreeImage_GetBits(texImage));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // release
  FreeImage_Unload(texImage);
}

void Mesh::draw(mat4 M, mat4 V, mat4 P, vec3 eye, vec3 lightColor,
                vec3 lightPosition, int unitBaseColor, int unitNormal) {
  glUseProgram(shader);

  glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(M));
  glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(V));
  glUniformMatrix4fv(uniProjection, 1, GL_FALSE, value_ptr(P));

  glUniform3fv(uniEyePoint, 1, value_ptr(eye));

  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  glUniform3fv(uniLightPosition, 1, value_ptr(lightPosition));
  glUniform1i(uniTexDepth, 3);

  for (size_t i = 0; i < scene->mNumMeshes; i++) {
    int numVtxs = scene->mMeshes[i]->mNumVertices;

    glBindVertexArray(vaos[i]);
    glDrawArrays(GL_TRIANGLES, 0, numVtxs);
  }
}

// void Mesh::translate(glm::vec3 xyz) {
//   // move each vertex with xyz
//   for (size_t i = 0; i < vertices.size(); i++) {
//     vertices[i] += xyz;
//   }
//
//   // update aabb
//   min += xyz;
//   max += xyz;
// }

// void Mesh::scale(glm::vec3 xyz) {
//   // scale each vertex with xyz
//   for (size_t i = 0; i < vertices.size(); i++) {
//     vertices[i].x *= xyz.x;
//     vertices[i].y *= xyz.y;
//     vertices[i].z *= xyz.z;
//   }
//
//   // update aabb
//   min.x *= xyz.x;
//   min.y *= xyz.y;
//   min.z *= xyz.z;
//
//   max.x *= xyz.x;
//   max.y *= xyz.y;
//   max.z *= xyz.z;
// }

// rotate mesh along x, y, z axes
// xyz specifies the rotated angle along each axis
// void Mesh::rotate(glm::vec3 xyz) {}

// void Mesh::findAABB() {
//   int nOfVtxs = vertices.size();
//   vec3 min(0, 0, 0), max(0, 0, 0);
//
//   for (size_t i = 0; i < nOfVtxs; i++) {
//     vec3 vtx = vertices[i];
//
//     // x
//     if (vtx.x > max.x) {
//       max.x = vtx.x;
//     }
//     if (vtx.x < min.x) {
//       min.x = vtx.x;
//     }
//     // y
//     if (vtx.y > max.y) {
//       max.y = vtx.y;
//     }
//     if (vtx.y < min.y) {
//       min.y = vtx.y;
//     }
//     // z
//     if (vtx.z > max.z) {
//       max.z = vtx.z;
//     }
//     if (vtx.z < min.z) {
//       min.z = vtx.z;
//     }
//   }
//
//   min = min;
//   max = max;
// }
