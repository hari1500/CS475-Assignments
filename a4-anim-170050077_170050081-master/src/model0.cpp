#include "model0.hpp"
#include "texture.hpp"

GLuint shaderProgram;
std::vector<GLuint> shaderList;

GLuint vbo[NUM_BUFFERS], vao[NUM_BUFFERS];
GLuint tex[NUM_BUFFERS];
int nVerticesPerBuffer[NUM_BUFFERS];

glm::mat4 rotation_matrix;
glm::mat4 init_rot_matrix;
glm::mat4 projection_matrix;
glm::mat4 c_rotation_matrix;
glm::mat4 lookat_matrix;

glm::mat4 model_matrix_earth, model_matrix_space;
glm::mat4 view_matrix;

glm::mat4 modelview_matrix;
glm::mat3 normal_matrix;

GLuint uModelViewMatrix;
GLuint viewMatrix;
GLuint normalMatrix;

GLuint vPosition, vNormal, vColor, texCoord;

bool wireframe=false;

//-----------------------------------------------------------------

//6 faces, 2 triangles/face, 3 vertices/triangle
glm::vec4 white(0.2, 0.7, 0.7, 1.0);

glm::vec4 v_positions[MAX_NUM_VERTICES], v_normals[MAX_NUM_VERTICES];
glm::vec2 v_tex_coords[MAX_NUM_VERTICES];

float earthRadius = 1000, spaceRadius = 4000, delta = 20;
float f_L=-1.0, f_R=1.0, f_T=1.0, f_B=-1.0, f_N=1.0, f_F=8000.0;

void bindBuffer1(int tri_idx)
{
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES) + ((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES) + ((tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES), v_positions);
  glBufferSubData( GL_ARRAY_BUFFER, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES), ((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES), v_tex_coords);
  glBufferSubData( GL_ARRAY_BUFFER, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES)+((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES), ((tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES), v_normals);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( texCoord );
  glVertexAttribPointer( texCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES)));
  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES)+((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES)));
}

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

std::vector<std::array<float, 12>> mymodel;
int oldSize = 0;
std::string filename = "./data/model0.raw";
std::ofstream output_file(filename);
std::ostream_iterator<float> output_iter(output_file, " ");

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

void initBuffersGL(void)
{
  // Load Textures 
  tex[0]=LoadTexture("data/earth.bmp",5400,2700);
  tex[1]=LoadTexturePNG("data/cloud.png");
  tex[2]=LoadTexture("data/space.bmp",1920,1200);

  glGenVertexArrays (NUM_BUFFERS, vao);
  glGenBuffers (NUM_BUFFERS, vbo);

  glBindVertexArray (vao[0]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
  int tri_idx = 0;
  sphere(earthRadius, tri_idx);
  nVerticesPerBuffer[0] = tri_idx; bindBuffer1(tri_idx); saveIntoFile1(tri_idx);

  glBindVertexArray (vao[1]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);
  tri_idx = 0;
  sphere(earthRadius+delta, tri_idx);
  nVerticesPerBuffer[1] = tri_idx; bindBuffer1(tri_idx); saveIntoFile1(tri_idx);

  glBindVertexArray (vao[2]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[2]);
  tri_idx = 0;
  sphere(spaceRadius, tri_idx);
  nVerticesPerBuffer[2] = tri_idx; bindBuffer1(tri_idx); saveIntoFile1(tri_idx);

  xrot = -90.0;
  init_rot_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  xrot = 0;
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  model_matrix_space = glm::mat4(1.0f);
  
  rotation_matrix = glm::rotate(init_rot_matrix, glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f,1.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f,0.0f,1.0f));
  model_matrix_earth = rotation_matrix;

  //Creating the lookat and the up vectors for the camera
  c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f,0.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f,1.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f,0.0f,1.0f));

  glm::vec4 c_pos = glm::vec4(c_xpos,c_ypos,c_zpos, 1.0)*c_rotation_matrix;
  glm::vec4 c_up = glm::vec4(c_up_x,c_up_y,c_up_z, 1.0)*c_rotation_matrix;
  //Creating the lookat matrix
  lookat_matrix = glm::lookAt(glm::vec3(c_pos),glm::vec3(0.0),glm::vec3(c_up));

  //creating the projection matrix
  projection_matrix = glm::frustum(f_L, f_R, f_B, f_T, f_N, f_F);

  view_matrix = projection_matrix*lookat_matrix;

  glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(view_matrix));

  // Draw the sphere
  modelview_matrix = view_matrix*model_matrix_earth;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[0]);
  glBindVertexArray (vao[0]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, nVerticesPerBuffer[0]);

  // Draw the clouds
  modelview_matrix = view_matrix*model_matrix_earth;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[1]);
  glBindVertexArray (vao[1]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, nVerticesPerBuffer[1]);

  modelview_matrix = view_matrix*model_matrix_space;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[2]);
  glBindVertexArray (vao[2]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, nVerticesPerBuffer[2]);
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //This is for MacOSX - can be omitted otherwise
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
  //We don't want the old OpenGL 
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

  //! Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(1024, 1024, "model0", NULL, NULL);
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
  std::string vertex_shader_file("./src/vshader0.glsl");
  std::string fragment_shader_file("./src/fshader0.glsl");

  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram( shaderProgram );

  // getting the attributes from the shader program
  vPosition = glGetAttribLocation( shaderProgram, "vPosition0" );
  vNormal = glGetAttribLocation( shaderProgram, "vNormal0" );
  vColor = glGetAttribLocation( shaderProgram, "vColor0" ); 
  texCoord = glGetAttribLocation( shaderProgram, "texCoord0" ); 
  uModelViewMatrix = glGetUniformLocation( shaderProgram, "uModelViewMatrix");
  normalMatrix =  glGetUniformLocation( shaderProgram, "normalMatrix");
  viewMatrix = glGetUniformLocation( shaderProgram, "viewMatrix");

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
