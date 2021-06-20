#include "model1.hpp"
#include "texture.hpp"

GLuint shaderProgram1;
glm::mat4 translation;
glm::mat4 rotation_matrix;
glm::mat4 translation_matrix;
glm::mat4 ortho_matrix;
glm::mat4 modelview_matrix;

glm::mat4 model_matrix;
glm::mat4 projection_matrix;
glm::mat4 c_rotation_matrix;
glm::mat4 lookat_matrix;
glm::mat4 view_matrix;
glm::mat3 normal_matrix;

glm::mat4 tower_translation;
glm::mat4 protrusion_translation;

GLuint uModelViewMatrix1, viewMatrix1, normalMatrix1, texExist1, lx, ly, lz;

std::vector<GLuint> shaderList1;

//-----------------------------------------------------------------
GLuint vPosition1, vColor1, vNormal1, texCoord1;
// vaos and vbos
GLuint vbo[NUM_BUFFERS], vao[NUM_BUFFERS];
GLuint tex[2];
int nVerticesPerBuffer[NUM_BUFFERS];

// Translation Parameters
GLfloat xpos=0.0,ypos=0.0,zpos=0.0;
// Rotation Parameters
GLfloat xrot=0.0,yrot=0.0,zrot=0.0;
// Camera position and rotation Parameters
GLfloat c_xpos = 50.0, c_ypos = 100.0, c_zpos = 300.0;
GLfloat c_up_x = 0.0, c_up_y = 1.0, c_up_z = 0.0;
GLfloat c_xrot=0.0,c_yrot=0.0,c_zrot=0.0;
// Tower model shift
GLfloat t_shift=0.0, prot_shift=0.0;
//Running variable to toggle culling on/off
bool enable_culling=true;
//Running variable to toggle wireframe/solid modelling
bool solid=true;
//Enable/Disable perspective view
bool enable_perspective=false;
//-----------------------------------------------------------------

//points in homogenous coordinates
glm::vec4 positions[8], normals[8] = {
  glm::vec4(-1.0, -1.0, 1.0, 1.0),
  glm::vec4(-1.0, 1.0, 1.0, 1.0),
  glm::vec4(1.0, 1.0, 1.0, 1.0),
  glm::vec4(1.0, -1.0, 1.0, 1.0),
  glm::vec4(-1.0, -1.0, -1.0, 1.0),
  glm::vec4(-1.0, 1.0, -1.0, 1.0),
  glm::vec4(1.0, 1.0, -1.0, 1.0),
  glm::vec4(1.0, -1.0, -1.0, 1.0)
};

glm::vec4 v_positions[MAX_NUM_VERTICES], v_normals[MAX_NUM_VERTICES], v_colors[MAX_NUM_VERTICES];
glm::vec2 v_tex_coords[MAX_NUM_VERTICES];

float earthRadius = 3000;

void sphere(double radius, int &tri_idx)
{
  float lats,longs;
  int slices = 8, sectors = 4;
  float l;

  for (lats = 0.0; lats <= 180; lats+=sectors)  
  {
    float lats_rad = glm::radians(lats);
    if(lats+sectors>180)
        l=180;
      else
        l=lats+sectors;
    float l_rad = glm::radians(l);    

    for(longs = 0.0; longs <= 360; longs+=slices)
    {
      float longs_rads = glm::radians(longs);
      float x = radius * sin(lats_rad) * cos(longs_rads);
      float y = radius * sin(lats_rad) * sin(longs_rads);
      float z = radius * cos(lats_rad);
      glm::vec4 pt(x, y, z, 1.0);
      glm::vec4 pt1(-x, -y, -z, 1.0);
      v_positions[tri_idx] = pt; v_normals[tri_idx] = pt1;
      v_tex_coords[tri_idx] = glm::vec2(longs/360, (1-lats/180)); tri_idx++;
      
      x = radius * sin(l_rad) * cos(longs_rads);
      y = radius * sin(l_rad) * sin(longs_rads);
      z = radius * cos(l_rad);
      pt =glm::vec4(x, y, z, 1.0);
      pt1 = glm::vec4(-x, -y, -z, 1.0);
      v_positions[tri_idx] = pt; v_normals[tri_idx] = pt1;
      v_tex_coords[tri_idx] = glm::vec2(longs/360, (1-l/180)); tri_idx++;
    }
  }
}

void assignPositions(float l, float b, float d) {
  positions[0] = glm::vec4(0, 0, d, 1.0);
  positions[1] = glm::vec4(0, b, d, 1.0);
  positions[2] = glm::vec4(l, b, d, 1.0);
  positions[3] = glm::vec4(l, 0, d, 1.0);
  positions[4] = glm::vec4(0, 0, 0, 1.0);
  positions[5] = glm::vec4(0, b, 0, 1.0);
  positions[6] = glm::vec4(l, b, 0, 1.0);
  positions[7] = glm::vec4(l, 0, 0, 1.0);
}

void setColorsCuboid(glm::vec4 col, int ind)
{
  for(int i=0; i<NUM_POINTS_PER_CUBE; ++i) v_colors[i+ind] = col;
}

void setTextureCuboid(int idx)
{
  for(int i=0; i<6; ++i) {
    v_tex_coords[idx] = (glm::vec2(1.0, 0.0)); idx++;
    v_tex_coords[idx] = (glm::vec2(0.0, 0.0)); idx++;
    v_tex_coords[idx] = (glm::vec2(0.0, 1.0)); idx++;
    v_tex_coords[idx] = (glm::vec2(1.0, 0.0)); idx++;
    v_tex_coords[idx] = (glm::vec2(0.0, 1.0)); idx++;
    v_tex_coords[idx] = (glm::vec2(1.0, 1.0)); idx++;
  }
}

void cuboid(glm::vec4 offset, float l, float b, float d, int &tri_ind)
{
  assignPositions(l, b, d);
  std::vector<std::vector<int>> quads {
    { 1, 0, 3, 2 },
    { 2, 3, 7, 6 },
    { 3, 0, 4, 7 },
    { 6, 5, 1, 2 },
    { 4, 5, 6, 7 },
    { 5, 4, 0, 1 },
  };
  std::vector<glm::vec4> normals {
    glm::vec4(0.0, 0.0, 1.0, 1.0),
    glm::vec4(1.0, 0.0, 0.0, 1.0),
    glm::vec4(0.0, -1.0, 0.0, 1.0),
    glm::vec4(0.0, 1.0, 0.0, 1.0),
    glm::vec4(0.0, 0.0, -1.0, 1.0),
    glm::vec4(-1.0, 0.0, 0.0, 1.0)
  };
  for(int i=0; i<quads.size(); ++i) {
    int a = quads[i][0], b = quads[i][1], c = quads[i][2], d = quads[i][3];
    v_positions[tri_ind] = (positions[a]+offset); v_normals[tri_ind] = normals[i]; tri_ind++;
    v_positions[tri_ind] = (positions[b]+offset); v_normals[tri_ind] = normals[i]; tri_ind++;
    v_positions[tri_ind] = (positions[c]+offset); v_normals[tri_ind] = normals[i]; tri_ind++;
    v_positions[tri_ind] = (positions[a]+offset); v_normals[tri_ind] = normals[i]; tri_ind++;
    v_positions[tri_ind] = (positions[c]+offset); v_normals[tri_ind] = normals[i]; tri_ind++;
    v_positions[tri_ind] = (positions[d]+offset); v_normals[tri_ind] = normals[i]; tri_ind++;
  }
}
//-----------------------------------------------------------------
void bindBuffer0(int tri_idx)
{
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, (tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES + (tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES + (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES, NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, (tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES, v_positions);
  glBufferSubData( GL_ARRAY_BUFFER, (tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, v_colors);
  glBufferSubData( GL_ARRAY_BUFFER, (tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES+(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES, v_normals);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( vColor1 );
  glVertexAttribPointer( vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES));
  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES+(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES));
}

void bindBuffer1(int tri_idx)
{
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES) + ((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES) + ((tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES), v_positions);
  glBufferSubData( GL_ARRAY_BUFFER, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES), ((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES), v_tex_coords);
  glBufferSubData( GL_ARRAY_BUFFER, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES)+((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES), ((tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES), v_normals);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( texCoord1 );
  glVertexAttribPointer( texCoord1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES)));
  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES)+((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES)));
}

std::vector<std::array<float, 12>> mymodel;
int oldSize = 0;
std::string filename = "./data/model1.raw";
std::ofstream output_file(filename);
std::ostream_iterator<float> output_iter(output_file, " ");

void saveIntoFile0(int num_points)
{
  oldSize = mymodel.size();
  for ( int i = 0; i < num_points; ++i ) {
    mymodel.push_back({v_positions[i][0], v_positions[i][1], v_positions[i][2], v_positions[i][3], v_normals[i][0], v_normals[i][1], v_normals[i][2], v_normals[i][3], v_colors[i][0], v_colors[i][1], v_colors[i][2], v_colors[i][3]});
  }

  output_file << num_points;
  output_file << " ";
  output_file << "0\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }
}

void saveIntoFile1(int num_points)
{
  oldSize = mymodel.size();
  for ( int i = 0; i < num_points; ++i ) {
    mymodel.push_back({v_positions[i][0], v_positions[i][1], v_positions[i][2], v_positions[i][3], v_normals[i][0], v_normals[i][1], v_normals[i][2], v_normals[i][3], v_tex_coords[i][0], v_tex_coords[i][1], 0.0, 0.0});
  }

  output_file << num_points;
  output_file << " ";
  output_file << "1\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }
}

float lenGround = 6000, widGround = 1, depGround = 6000;
float lenBase = 50*5, widBase = 1, depBase = 50*5;
float lenBuilding = 4*5, widBuilding = 20*5, depBuilding = 40*5;
float lenTower = 4*5, widTower = 100, depTower = 4*5;
float lenPad = 8*5, widPad = 2*5, depPad = 8*5;
const int numProtrusions = 4;
float protrusionLength = 3*5;
float protrusions_heights[numProtrusions] = { 12, 35, 60, 80 };
float protrusioonDimensions[3] = { 3*5, 1*5, 1*5 };

void initBuffersGL(void)
{
  // Load Textures 
  tex[0]=LoadTexture("data/ground-texture.bmp",3072,2304);
  tex[1]=LoadTexture("data/blue-sky.bmp",3888,2592);

  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (NUM_BUFFERS, vao);
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (NUM_BUFFERS, vbo);

  ////////////////////////////////  base + building + pad-for-rocket-launching  ////////////////////////////////
  glBindVertexArray (vao[0]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
  int tri_idx = 0;

  cuboid(glm::vec4(0.0, 0.0, 0.0, 0.0), lenBase, widBase, depBase, tri_idx);
  setColorsCuboid(glm::vec4(0.5555  , 0.5555, 0.5555, 1.0), tri_idx-NUM_POINTS_PER_CUBE);

  cuboid(glm::vec4(0.0, widBase, (depBase-depBuilding)/2, 0.0), lenBuilding, widBuilding, depBuilding, tri_idx);
  setColorsCuboid(glm::vec4(1.0, 0.8, 0.0, 1.0), tri_idx-NUM_POINTS_PER_CUBE);

  cuboid(glm::vec4((lenBase-lenPad)/2, widBase, (depBase-depPad)/2, 0.0), lenPad, widPad, depPad, tri_idx);
  setColorsCuboid(glm::vec4(0.3, 0.7, 0.7, 1.0), tri_idx-NUM_POINTS_PER_CUBE);
  nVerticesPerBuffer[0] = tri_idx; bindBuffer0(tri_idx); saveIntoFile0(tri_idx);

  //base-tower//
  glBindVertexArray (vao[1]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);
  tri_idx = 0;
  cuboid(glm::vec4((lenBase-lenTower)/2, widBase, 0.0, 0.0), lenTower, widTower, depTower, tri_idx);
  setColorsCuboid(glm::vec4(0.3, 0.7, 0.7, 1.0), tri_idx-NUM_POINTS_PER_CUBE);
  nVerticesPerBuffer[1] = tri_idx; bindBuffer0(tri_idx); saveIntoFile0(tri_idx);

  // protrusions//
  glBindVertexArray (vao[2]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[2]);
  tri_idx = 0;
  prot_shift = 5-protrusionLength;
  for(int i=0; i<numProtrusions; ++i){
    cuboid(glm::vec4((lenBase-protrusioonDimensions[2])/2, protrusions_heights[i], depTower, 0.0), protrusioonDimensions[2], protrusioonDimensions[1], protrusioonDimensions[0], tri_idx);
    setColorsCuboid(glm::vec4(0.0, 0.0, 0.3, 1.0), tri_idx-NUM_POINTS_PER_CUBE);
  }
  nVerticesPerBuffer[2] = tri_idx; bindBuffer0(tri_idx); saveIntoFile0(tri_idx);

  //////Ground/////
  glBindVertexArray (vao[3]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[3]);
  tri_idx = 0;
  cuboid(glm::vec4(-(lenGround-lenBase)/2, -widGround, -(depGround-depBase)/2, 0.0), lenGround, widGround, depGround, tri_idx);
  setTextureCuboid(tri_idx-NUM_POINTS_PER_CUBE);
  nVerticesPerBuffer[3] = tri_idx; bindBuffer1(tri_idx); saveIntoFile1(tri_idx);

  ////////////////////////// outer ///////////////////////////
  glBindVertexArray (vao[4]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[4]);
  tri_idx = 0;
  sphere(earthRadius, tri_idx);
  nVerticesPerBuffer[4] = tri_idx; bindBuffer1(tri_idx); saveIntoFile1(tri_idx);  

  translation = glm::translate(glm::mat4(1.0f), glm::vec3(-lenBase/2, 0.0, -depBase/2));
  // projection_matrix = glm::ortho(-100.0, 100.0, -100.0, 100.0, -100.0, 1000.0);
  projection_matrix = glm::frustum(-1.5, 1.5, -1.5, 1.5, 1.0, 6000.0);
  glUniform1f(lx, 200);
  glUniform1f(ly, 200);
  glUniform1f(lz, 0);
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f,1.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f,0.0f,1.0f));
  model_matrix = rotation_matrix * translation;

  //Creating the lookat and the up vectors for the camera
  c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f,0.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f,1.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f,0.0f,1.0f));

  glm::vec4 c_pos = glm::vec4(c_xpos,c_ypos,c_zpos, 1.0)*c_rotation_matrix;
  glm::vec4 c_up = glm::vec4(c_up_x,c_up_y,c_up_z, 1.0)*c_rotation_matrix;
  //Creating the lookat matrix
  lookat_matrix = glm::lookAt(glm::vec3(c_pos),glm::vec3(0.0),glm::vec3(c_up));

  view_matrix = projection_matrix*lookat_matrix;

  glUniformMatrix4fv(viewMatrix1, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniform1f(texExist1, 0.0);

  // Draw the model
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[0]);  
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[0]);

  // Draw the tower
  tower_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, t_shift));
  modelview_matrix = view_matrix * model_matrix * tower_translation;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  // normal_matrix = glm::mat3(modelview_matrix);
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[1]);  
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[1]);

  // Draw the protrusion
  protrusion_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, prot_shift));
  modelview_matrix = view_matrix * model_matrix * tower_translation * protrusion_translation;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  // normal_matrix = glm::mat3(modelview_matrix);
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[2]);  
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[2]);

  // Ground
  glUniform1f(texExist1, 1.0);
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  // normal_matrix = glm::mat3(modelview_matrix);
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[0]);
  glBindVertexArray(vao[3]);
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[3]);

  // Outer
  modelview_matrix = view_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[1]);
  glBindVertexArray (vao[4]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, nVerticesPerBuffer[4]);
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
  window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "model1", NULL, NULL);
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

  //Keyboard Callback
  glfwSetKeyCallback(window, csX75::key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


  // Load shaders and use the resulting shader program
  std::string vertex_shader_file1("./src/vshader1.glsl");
  std::string fragment_shader_file1("./src/fshader1.glsl");

  
  shaderList1.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file1));
  shaderList1.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file1));

  shaderProgram1 = csX75::CreateProgramGL(shaderList1);
  glUseProgram( shaderProgram1 );
  
  
  vPosition1 = glGetAttribLocation( shaderProgram1, "vPosition1" );
  vColor1 = glGetAttribLocation( shaderProgram1, "vColor1" ); 
  vNormal1 = glGetAttribLocation( shaderProgram1, "vNormal1" ); 
  texCoord1 = glGetAttribLocation( shaderProgram1, "texCoord1" );
  texExist1 = glGetUniformLocation( shaderProgram1, "texExist1" );
  lx = glGetUniformLocation( shaderProgram1, "lx" );
  ly = glGetUniformLocation( shaderProgram1, "ly" );
  lz = glGetUniformLocation( shaderProgram1, "lz" );
  uModelViewMatrix1 = glGetUniformLocation( shaderProgram1, "uModelViewMatrix");
  normalMatrix1 =  glGetUniformLocation( shaderProgram1, "normalMatrix");
  viewMatrix1 = glGetUniformLocation( shaderProgram1, "viewMatrix");

  //Initialize GL state
  csX75::initGL();
  initBuffersGL();

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
  return 0;
}
