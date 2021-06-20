#include "a2view.hpp"

GLuint shaderProgram;
glm::mat4 grid_translation_1, grid_translation_11;
glm::mat4 grid_translation_2, grid_translation_21;
glm::mat4 ortho_matrix;
glm::mat4 ortho_matrix0;
glm::mat4 ortho_matrix1;
glm::mat4 ortho_matrix2;
glm::mat4 modelview_matrix;
int NUM_MODELS = 0;
std::vector<GLuint> shaderList;


//-----------------------------------------------------------------
GLuint uModelViewMatrix, vPosition, vColor;
// vaos and vbos
GLuint gridvao, gridvbo, frustumvao, frustumvbo;

GLuint* modelvaos;
GLuint* modelvbos;

// per model transformation using S R T values.
std::vector<glm::mat4> SRT_matrices;

// per model points and colors.
std::vector<std::vector<glm::vec4>> scene_points;
std::vector<std::vector<glm::vec4>> scene_colors;

// VCS setup
glm::vec3 eye, lookat, upvector;
GLfloat L, R, T, B, N, F;
glm::vec3 U_vec, V_vec, N_vec;

glm::mat4 VCStoWCS;
glm::mat4 WCStoVCS;
glm::mat4 VCStoCCS;
glm::mat4 CCStoNDCS;
glm::mat4 NDCStoDCS;
glm::vec4 grid_points[GRID_POINTS];
glm::vec4 grid_colors[GRID_POINTS];
glm::vec4 plane_points[PLANE_POINTS];
glm::vec4 plane_colors[PLANE_POINTS];
glm::vec4 frustrum_points[36];
glm::vec4 frustrum_colors[36];

// Rotation Parameters
GLfloat xrot=0.0,yrot=0.0,zrot=0.0;

int mode = 0;

int toggle = 0;

// default color
glm::vec4 defaultColor=glm::vec4(1.0, 1.0, 1.0, 1.0);
// current color
glm::vec4 currColor=defaultColor;
// storing selected points, colors
std::vector<glm::vec4> selectedPoints;
std::vector<glm::vec4> selectedColors;

//Running variable to toggle culling on/off
bool enable_culling=true;
//Running variable to toggle wireframe/solid modelling
bool solid=true;
//-----------------------------------------------------------------

//points in homogenous coordinates
glm::vec4 positions[8] = 
{
  glm::vec4(0.0, 0.0, 1.0, 1.0),
  glm::vec4(0.0, 1.0, 1.0, 1.0),
  glm::vec4(1.0, 1.0, 1.0, 1.0),
  glm::vec4(1.0, 0.0, 1.0, 1.0),
  glm::vec4(0.0, 0.0, 0.0, 1.0),
  glm::vec4(0.0, 1.0, 0.0, 1.0),
  glm::vec4(1.0, 1.0, 0.0, 1.0),
  glm::vec4(1.0, 0.0, 0.0, 1.0)
};

glm::vec4 LIGHT_GREY = glm::vec4(0.658824, 0.658824, 0.658824, 1.0);
glm::vec4 CYAN = glm::vec4(0.0, 1.0, 1.0, 1.0);
glm::vec4 MAGENTA = glm::vec4(1.0, 0.0, 1.0, 1.0);
glm::vec4 RED = glm::vec4(1.0, 0.0, 0.0, 1.0);

void createGrid(void)
{
  int N_LINES = GRID_SIZE+1;
  for(int i=0;i<=GRID_SIZE;i++){
    for(int j=0;j<=GRID_SIZE;j++){
      // parallel to x axis
      grid_points[2*(N_LINES*i + j)] = glm::vec4(0.0, j*1.0, i*1.0, 1.0);
      grid_points[2*(N_LINES*i + j)+1] = glm::vec4(GRID_SIZE*1.0, j*1.0, i*1.0, 1.0);
      // parallel to y axis
      grid_points[N_LINES*N_LINES*2 + 2*(N_LINES*i + j)] = glm::vec4(j*1.0, 0.0, i*1.0, 1.0);
      grid_points[N_LINES*N_LINES*2 + 2*(N_LINES*i + j)+1] = glm::vec4(j*1.0, GRID_SIZE*1.0, i*1.0, 1.0);
      // parallel to z axis
      grid_points[N_LINES*N_LINES*4 + 2*(N_LINES*i + j)] = glm::vec4(i*1.0, j*1.0, 0.0, 1.0);
      grid_points[N_LINES*N_LINES*4 + 2*(N_LINES*i + j)+1] = glm::vec4(i*1.0, j*1.0, GRID_SIZE*1.0, 1.0);

      // assign light grey colors to the points
      grid_colors[2*(N_LINES*i + j)] = LIGHT_GREY;
      grid_colors[2*(N_LINES*i + j)+1] = LIGHT_GREY;
      grid_colors[N_LINES*N_LINES*2 + 2*(N_LINES*i + j)] = LIGHT_GREY;
      grid_colors[N_LINES*N_LINES*2 + 2*(N_LINES*i + j)+1] = LIGHT_GREY;
      grid_colors[N_LINES*N_LINES*4 + 2*(N_LINES*i + j)] = LIGHT_GREY;
      grid_colors[N_LINES*N_LINES*4 + 2*(N_LINES*i + j)+1] = LIGHT_GREY;
    }
  }
}

void createPlanes(void)
{
  int N_LINES = GRID_SIZE+1;
  for(int i=0;i<=GRID_SIZE;i++){
    plane_points[4*i] = glm::vec4(0.0, i*1.0, 0.0, 1.0);
    plane_points[4*i + 1] = glm::vec4(GRID_SIZE*1.0, i*1.0, 0.0, 1.0);
    plane_points[4*i + 2] = glm::vec4(i*1.0, 0.0, 0.0, 1.0);
    plane_points[4*i + 3] = glm::vec4(i*1.0, GRID_SIZE*1.0, 0.0, 1.0);
    
    plane_points[4*N_LINES + 4*i] = glm::vec4(0.0, 0.0, i*1.0, 1.0);
    plane_points[4*N_LINES + 4*i + 1] = glm::vec4(0.0, GRID_SIZE*1.0, i*1.0, 1.0);
    plane_points[4*N_LINES + 4*i + 2] = glm::vec4(0.0, i*1.0, 0.0, 1.0);
    plane_points[4*N_LINES + 4*i + 3] = glm::vec4(0.0, i*1.0, GRID_SIZE*1.0, 1.0);
    
    plane_points[8*N_LINES + 4*i] = glm::vec4(0.0, 0.0, i*1.0, 1.0);
    plane_points[8*N_LINES + 4*i + 1] = glm::vec4(GRID_SIZE*1.0, 0.0, i*1.0, 1.0);
    plane_points[8*N_LINES + 4*i + 2] = glm::vec4(i*1.0, 0.0, 0.0, 1.0);
    plane_points[8*N_LINES + 4*i + 3] = glm::vec4(i*1.0, 0.0, GRID_SIZE*1.0, 1.0);
    
    // assign light grey colors to the points
    plane_colors[4*i] = LIGHT_GREY;
    plane_colors[4*i + 1] = LIGHT_GREY;
    plane_colors[4*i + 2] = LIGHT_GREY;
    plane_colors[4*i + 3] = LIGHT_GREY;
     
    plane_colors[4*N_LINES + 4*i] = LIGHT_GREY;
    plane_colors[4*N_LINES + 4*i + 1] = LIGHT_GREY;
    plane_colors[4*N_LINES + 4*i + 2] = LIGHT_GREY;
    plane_colors[4*N_LINES + 4*i + 3] = LIGHT_GREY;

    plane_colors[8*N_LINES + 4*i] = LIGHT_GREY;
    plane_colors[8*N_LINES + 4*i + 1] = LIGHT_GREY;
    plane_colors[8*N_LINES + 4*i + 2] = LIGHT_GREY;
    plane_colors[8*N_LINES + 4*i + 3] = LIGHT_GREY;
  }
}

void loadScene(){
  std::string filename = "./data/myscene.scn";
  std::fstream file;
        
  file.open(filename.c_str());
  bool readVCS = false;
  for(std::string line; std::getline(file, line);){
    if(line[0] == '\0' || line[0] == '#' || line[0] == '\n') continue;
    else if(line.substr(line.find_last_of(".")+1) == "raw"){
      NUM_MODELS++;
      std::fstream modelfile;
      std::string modelfilename = "./data/" + line;
      modelfile.open(modelfilename.c_str());
      std::vector<glm::vec4> model_points, model_colors;
      while(true){
        std::string cx, cy, cz, r, g, b;
        if(!(modelfile>>cx && modelfile>>cy && modelfile>>cz && modelfile>>r && modelfile>>g && modelfile>>b)) break;  
        model_points.push_back(glm::vec4(std::stof(cx),std::stof(cy),std::stof(cz), 0.0));
        model_colors.push_back(glm::vec4(std::stof(r),std::stof(g),std::stof(b), 1.0));
      }
      modelfile.close();
      scene_points.push_back(model_points);
      scene_colors.push_back(model_colors);
      std::string sx, sy, sz, rx, ry, rz, tx, ty, tz;
      file>>sx; file>>sy; file>>sz;
      file>>rx; file>>ry; file>>rz;
      file>>tx; file>>ty; file>>tz;
      glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(stof(rx)), glm::vec3(1.0f,0.0f,0.0f));
      rotation_matrix = glm::rotate(rotation_matrix, glm::radians(stof(ry)), glm::vec3(0.0f,1.0f,0.0f));
      rotation_matrix = glm::rotate(rotation_matrix, glm::radians(stof(rz)), glm::vec3(0.0f,0.0f,1.0f));
      glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(std::stof(tx),std::stof(ty),std::stof(tz)));
      SRT_matrices.push_back(translation_matrix * glm::scale(rotation_matrix, glm::vec3(std::stof(sx),std::stof(sy),std::stof(sz))));   
      //scale first then rotate then translate
    }
    else if(!readVCS){
      float x, y, z;
      sscanf(line.c_str(), "%f %f %f", &x,&y,&z);
      eye = glm::vec3(x,y,z);
      std::getline(file, line);
      sscanf(line.c_str(), "%f %f %f", &x,&y,&z);
      lookat = glm::vec3(x,y,z);
      std::getline(file, line);
      sscanf(line.c_str(), "%f %f %f", &x,&y,&z);
      upvector = glm::vec3(x,y,z);
      readVCS = true;
    }
    else{
      sscanf(line.c_str(), "%f %f %f %f", &L,&R,&T,&B);
      std::getline(file, line);
      sscanf(line.c_str(), "%f %f", &N,&F);  
    }
  }
  file.close();
}

void makefrustrum(){
  glm::vec4 frust_points[8];
  frust_points[0] = VCStoWCS * glm::vec4(L,T,-1.0f*N,1.0f);
  frust_points[1] = VCStoWCS * glm::vec4(R,T,-1.0f*N,1.0f);
  frust_points[3] = VCStoWCS * glm::vec4(L,B,-1.0f*N,1.0f);
  frust_points[2] = VCStoWCS * glm::vec4(R,B,-1.0f*N,1.0f);
  
  frust_points[4] = VCStoWCS * glm::vec4(L*(F/N),T*(F/N),-1.0f*F,1.0f);
  frust_points[5] = VCStoWCS * glm::vec4(R*(F/N),T*(F/N),-1.0f*F,1.0f);
  frust_points[7] = VCStoWCS * glm::vec4(L*(F/N),B*(F/N),-1.0f*F,1.0f);
  frust_points[6] = VCStoWCS * glm::vec4(R*(F/N),B*(F/N),-1.0f*F,1.0f);

  for(int i=0;i<4;i++){
    frustrum_points[2*i+0] = glm::vec4(eye, 1.0f);
    frustrum_points[2*i+1] = frust_points[i];
    frustrum_colors[2*i+0] = MAGENTA;
    frustrum_colors[2*i+1] = MAGENTA;

    frustrum_points[8+2*i+0] = frust_points[i];
    frustrum_points[8+2*i+1] = frust_points[4+i];
    frustrum_colors[8+2*i+0] = CYAN;
    frustrum_colors[8+2*i+1] = CYAN;

    frustrum_points[16+2*i+0] = frust_points[i];
    frustrum_points[16+2*i+1] = frust_points[(i+1)%4];
    frustrum_colors[16+2*i+0] = CYAN;
    frustrum_colors[16+2*i+1] = CYAN;

    frustrum_points[24+2*i+0] = frust_points[4+i];
    frustrum_points[24+2*i+1] = frust_points[4+(i+1)%4];
    frustrum_colors[24+2*i+0] = CYAN;
    frustrum_colors[24+2*i+1] = CYAN;

    frustrum_colors[32+i] = RED;
  }
  frustrum_points[32] = VCStoWCS * glm::vec4(-2.0, 0.0, 0.0, 1.0f);
  frustrum_points[33] = VCStoWCS * glm::vec4(2.0, 0.0, 0.0,1.0f);
  frustrum_points[34] = VCStoWCS * glm::vec4(0.0, -2.0, 0.0, 1.0f);
  frustrum_points[35] = VCStoWCS * glm::vec4(0.0, 2.0, 0.0,1.0f); 
}

std::vector<glm::vec4> setColorsCube(glm::vec4 col)
{
  std::vector<glm::vec4> out_colors;
  for(int i=0; i<NUM_POINTS_PER_CUBE; ++i) out_colors.push_back(col);
  return out_colors;
}

std::vector<glm::vec4> setPositionsCube(glm::vec4 offset)
{
  std::vector<glm::vec4> out_points;
  std::vector<std::vector<int>> quads {
    { 1, 0, 3, 2 },
    { 2, 3, 7, 6 },
    { 3, 0, 4, 7 },
    { 6, 5, 1, 2 },
    { 4, 5, 6, 7 },
    { 5, 4, 0, 1 },
  };
  for(int i=0; i<quads.size(); ++i) {
    int a = quads[i][0], b = quads[i][1], c = quads[i][2], d = quads[i][3];
    out_points.push_back(positions[a]+offset);
    out_points.push_back(positions[b]+offset);
    out_points.push_back(positions[c]+offset);
    out_points.push_back(positions[a]+offset);
    out_points.push_back(positions[c]+offset);
    out_points.push_back(positions[d]+offset);
  }
  return out_points;
}

//-----------------------------------------------------------------

void initBuffersGL(void)
{
  createGrid();
  createPlanes();
  loadScene();

  modelvaos = new GLuint[NUM_MODELS];
  modelvbos = new GLuint[NUM_MODELS]; 
    
  //////////////////////////////////// grid /////////////////////////////////////////////////////////
  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (1, &gridvao);
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (1, &gridvbo);
  //Set it as the current array to be used by binding it
  glBindVertexArray (gridvao);
  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, gridvbo);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(grid_points) + sizeof(grid_colors), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(grid_points), grid_points);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(grid_points), sizeof(grid_colors), grid_colors);
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(grid_points)));


  // ////////////////////////////////// frustum ///////////////////////////////////////////
  N_vec = glm::normalize(eye-lookat); 
  U_vec = glm::normalize(glm::cross(upvector, N_vec));
  V_vec = glm::normalize(glm::cross(N_vec, U_vec));

  VCStoWCS[0] = glm::vec4(U_vec, 0.0f);
  VCStoWCS[1] = glm::vec4(V_vec, 0.0f);
  VCStoWCS[2] = glm::vec4(N_vec, 0.0f);
  VCStoWCS[3] = glm::vec4(eye, 1.0f);

  makefrustrum();

  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (1, &frustumvao);
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (1, &frustumvbo);
  //Set it as the current array to be used by binding it
  glBindVertexArray (frustumvao);
  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, frustumvbo);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(frustrum_points) + sizeof(frustrum_colors), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(frustrum_points), frustrum_points);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(frustrum_points), sizeof(frustrum_colors), frustrum_colors);
  // set up vertex arrays
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(frustrum_points)));  


  // ////////////////////////////////  models  /////////////////////////////////////////////////
  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (NUM_MODELS, modelvaos);
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (NUM_MODELS, modelvbos);
  
  for(int i=0;i<NUM_MODELS;++i){
    std::vector<glm::vec4> model_points, model_colors;
    for (int j=0; j < scene_points[i].size(); ++j) {
      std::vector<glm::vec4> temp = setPositionsCube(scene_points[i][j]);
      model_points.insert(model_points.end(), temp.begin(), temp.end());
      temp = setColorsCube(scene_colors[i][j]);
      model_colors.insert(model_colors.end(), temp.begin(), temp.end());
    }
    glm::vec4 model_points_array[model_points.size()];
    glm::vec4 model_colors_array[model_colors.size()];
    std::copy(model_points.begin(), model_points.end(), model_points_array);
    std::copy(model_colors.begin(), model_colors.end(), model_colors_array);
    // Set it as the current array to be used by binding it
    glBindVertexArray (modelvaos[i]);
    //Set it as the current buffer to be used by binding it
    glBindBuffer (GL_ARRAY_BUFFER, modelvbos[i]);
    //Copy the points into the current buffer
    glBufferData (GL_ARRAY_BUFFER, sizeof(model_points_array) + sizeof(model_colors_array), NULL, GL_STATIC_DRAW);
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(model_points_array), model_points_array);
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(model_points_array), sizeof(model_colors_array), model_colors_array);

    // set up vertex arrays
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(model_points_array)));  
  }
  ortho_matrix = glm::ortho(-1.5*GRID_SIZE, 2.5*GRID_SIZE, -1.5*GRID_SIZE, 2.5*GRID_SIZE, -1.5*GRID_SIZE, 2.5*GRID_SIZE);
  ortho_matrix0 = glm::ortho(-2.0*GRID_SIZE, 2.0*GRID_SIZE, -2.0*GRID_SIZE, 2.0*GRID_SIZE, -2.0*GRID_SIZE, 2.0*GRID_SIZE);
  ortho_matrix1 = glm::ortho(-1.5, 1.5, -1.5, 1.5, -1.5, 1.5);
  ortho_matrix2 = glm::ortho(-1.0, 1.0, -1.0, 1.0, -1.5, 1.5);
  
  WCStoVCS[0][0] = U_vec.x;
  WCStoVCS[1][0] = U_vec.y;
  WCStoVCS[2][0] = U_vec.z;
  WCStoVCS[3][0] = -1.0*glm::dot(U_vec, eye);
  WCStoVCS[0][1] = V_vec.x;
  WCStoVCS[1][1] = V_vec.y;
  WCStoVCS[2][1] = V_vec.z;
  WCStoVCS[3][1] = -1.0*glm::dot(V_vec, eye);
  WCStoVCS[0][2] = N_vec.x;
  WCStoVCS[1][2] = N_vec.y;
  WCStoVCS[2][2] = N_vec.z;
  WCStoVCS[3][2] = -1.0*glm::dot(N_vec, eye);
  WCStoVCS[0][3] = 0;
  WCStoVCS[1][3] = 0;
  WCStoVCS[2][3] = 0;
  WCStoVCS[3][3] = 1;
  
  VCStoCCS[0][0] = 2.0*N/(R-L);
  VCStoCCS[1][0] = 0.0;
  VCStoCCS[2][0] = (R+L)/(R-L);
  VCStoCCS[3][0] = 0.0;
  VCStoCCS[0][1] = 0.0;
  VCStoCCS[1][1] = 2.0*N/(T-B);
  VCStoCCS[2][1] = (T+B)/(T-B);
  VCStoCCS[3][1] = 0.0;
  VCStoCCS[0][2] = 0.0;
  VCStoCCS[1][2] = 0.0;
  VCStoCCS[2][2] = 1.0*(F+N)/(F-N);
  VCStoCCS[3][2] = 2.0*F*N/(F-N);
  VCStoCCS[0][3] = 0;
  VCStoCCS[1][3] = 0;
  VCStoCCS[2][3] = -1;
  VCStoCCS[3][3] = 0;
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f,1.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f,0.0f,1.0f));

  glm::mat4 orth_mul_rot = ortho_matrix * grid_translation_2 * rotation_matrix * grid_translation_1;
  glm::mat4 orth_mul_rot0 = ortho_matrix0 * grid_translation_2 * rotation_matrix * grid_translation_1;
  glm::mat4 orth_mul_rot1 = ortho_matrix1 * grid_translation_21 * rotation_matrix * grid_translation_11;
  glm::mat4 orth_mul_rot2 = ortho_matrix2 * grid_translation_21 * rotation_matrix * grid_translation_11;
  ////////////////////////////////////////// grid ////////////////////////////////////////////////
  if(mode == 0) modelview_matrix = orth_mul_rot;
  else if(mode == 1) modelview_matrix = orth_mul_rot0 * WCStoVCS;
  else if(mode == 2 || mode == 3) modelview_matrix = orth_mul_rot1 * VCStoCCS * WCStoVCS;
  else modelview_matrix = orth_mul_rot2 * VCStoCCS * WCStoVCS;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  
  glBindVertexArray (gridvao);
  if(toggle == 0) glDrawArrays(GL_LINES, 0, GRID_POINTS);
  else glDrawArrays(GL_LINES, 0, PLANE_POINTS);

  //////////////////////////////////////// frustrum ///////////////////////////////////////////

  if(mode == 0) modelview_matrix = orth_mul_rot;
  else if(mode == 1) modelview_matrix = orth_mul_rot0 * WCStoVCS;
  else if(mode == 2 || mode == 3) modelview_matrix = orth_mul_rot1 * VCStoCCS * WCStoVCS;
  else modelview_matrix = orth_mul_rot2 * VCStoCCS * WCStoVCS;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  
  glBindVertexArray(frustumvao);
  glDrawArrays(GL_LINES, 0, sizeof(frustrum_points));

  /////////////////////////////////////// models //////////////////////////////////////////////////

  for(int i=0;i<NUM_MODELS;i++){
    if(mode == 0) modelview_matrix = orth_mul_rot * SRT_matrices[i];
    else if(mode == 1) modelview_matrix = orth_mul_rot0 * WCStoVCS * SRT_matrices[i];
    else if(mode == 2 || mode == 3) modelview_matrix = orth_mul_rot1 * VCStoCCS * WCStoVCS * SRT_matrices[i];
    else modelview_matrix = orth_mul_rot2 * VCStoCCS * WCStoVCS * SRT_matrices[i];

    glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
    glBindVertexArray(modelvaos[i]);
    glDrawArrays(GL_TRIANGLES, 0, 36*scene_points[i].size());  
  }
}

int main(int argc, char** argv)
{
  //! The pointer to the GLFW window
  GLFWwindow* window;

  //! Setting up the GLFW Error callback
  glfwSetErrorCallback(csX75::error_callback);

  //! Initialize GLFW
  if (!glfwInit())
    return -1;

  //We want OpenGL 4.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  //This is for MacOSX - can be omitted otherwise
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
  //We don't want the old OpenGL 
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

  //! Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "a2view", NULL, NULL);
  if (!window)
    {
      glfwTerminate();
      return -1;
    }
  
  //! Make the window's context current 
  glfwMakeContextCurrent(window);

  //Initialize GLEW
  //Turn this on to get Shader based OpenGL
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      //Problem: glewInit failed, something is seriously wrong.
      std::cerr<<"GLEW Init Failed : %s"<<std::endl;
    }

  //Print and see what context got enabled
  std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
  std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
  std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
  std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;
  
  //Keyboard Callback
  glfwSetKeyCallback(window, csX75::key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


  // Load shaders and use the resulting shader program
  std::string vertex_shader_file("./src/a2view_vshader.glsl");
  std::string fragment_shader_file("./src/a2view_fshader.glsl");

  
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram( shaderProgram );
  
  vPosition = glGetAttribLocation( shaderProgram, "vPosition" );
  vColor = glGetAttribLocation( shaderProgram, "vColor" ); 
  
  uModelViewMatrix = glGetUniformLocation( shaderProgram, "uModelViewMatrix");

  // Initialize translation matrices for the grid center.
  grid_translation_1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0*GRID_SIZE/2.0, -1.0*GRID_SIZE/2.0, -1.0*GRID_SIZE/2.0));
  grid_translation_2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0*GRID_SIZE/2.0, 1.0*GRID_SIZE/2.0, 1.0*GRID_SIZE/2.0));
  grid_translation_11 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0/2.0, -1.0/2.0, -1.0/2.0));
  grid_translation_21 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0/2.0, 1.0/2.0, 1.0/2.0));
  //Initialize GL state
  csX75::initGL();
  initBuffersGL();
  std::cout<<"Current mode is WCS mode\n";
  // Loop until the user closes the window
  while (glfwWindowShouldClose(window) == 0)
    {
       
      // Render here
      renderGL();

      // Swap front and back buffers
      glfwSwapBuffers(window);
      
      // Poll for and process events
      glfwPollEvents();
    }
  
  glfwTerminate();
  delete[] modelvaos;
  delete[] modelvbos;
  return 0;
}
