#include "model0.hpp"
#include "texture.hpp"

GLuint shaderProgram;
GLuint vbo[3], vao[3];
GLuint tex[3];

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

bool wireframe=false;

//-----------------------------------------------------------------

//6 faces, 2 triangles/face, 3 vertices/triangle
const int num_vertices = 20000;
glm::vec4 white(0.2, 0.7, 0.7, 1.0);

glm::vec4 v_positions[num_vertices];
glm::vec4 v_normals[num_vertices];
glm::vec2 tex_coords[num_vertices];

float earthRadius = 1000, spaceRadius = 4000, delta = 20;
float f_L=-1.0, f_R=1.0, f_T=1.0, f_B=-1.0, f_N=1.0, f_F=8000.0;

void sphere(double radius)
{
  float lats,longs;

  int slices = 8; 
  int sectors = 4;

  float l;

  int tri_idx = 0;
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

      v_positions[tri_idx] = pt; 
      v_normals[tri_idx] = pt;
      tex_coords[tri_idx] = glm::vec2(longs/360, (1-lats/180));
      tri_idx++;
      
      x = radius * sin(l_rad) * cos(longs_rads);
      y = radius * sin(l_rad) * sin(longs_rads);
      z = radius * cos(l_rad);
      pt =glm::vec4(x, y, z, 1.0);
      
      v_positions[tri_idx] = pt; 
      v_normals[tri_idx] = pt;
      tex_coords[tri_idx] = glm::vec2(longs/360, (1-l/180));
      tri_idx++;
    }
  }
}

void initBuffersGL(void)
{

  std::vector<std::array<float, 12>> mymodel;
  int oldSize = 0;
  std::string filename = "./data/model0.raw";
  std::ofstream output_file(filename);
  std::ostream_iterator<float> output_iter(output_file, " ");


  // Load shaders and use the resulting shader program
  std::string vertex_shader_file("./src/vshader0.glsl");
  std::string fragment_shader_file("./src/fshader0.glsl");

  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram( shaderProgram );

  // getting the attributes from the shader program
  GLuint vPosition = glGetAttribLocation( shaderProgram, "vPosition0" );
  GLuint vNormal = glGetAttribLocation( shaderProgram, "vNormal0" );
  GLuint vColor = glGetAttribLocation( shaderProgram, "vColor0" ); 
  GLuint texCoord = glGetAttribLocation( shaderProgram, "texCoord0" ); 
  uModelViewMatrix = glGetUniformLocation( shaderProgram, "uModelViewMatrix");
  normalMatrix =  glGetUniformLocation( shaderProgram, "normalMatrix");
  viewMatrix = glGetUniformLocation( shaderProgram, "viewMatrix");

  // Load Textures 
  tex[0]=LoadTexture("data/earth.bmp",5400,2700);
  tex[1]=LoadTexturePNG("data/cloud.png");
  tex[2]=LoadTexture("data/space.bmp",1920,1200);
  // tex[0]=LoadTextureAll("data/earth.bmp");

  //Ask GL for two Vertex Attribute Objects (vao) , one for the sphere and one for the wireframe
  glGenVertexArrays (3, vao);
  //Ask GL for two Vertex Buffer Object (vbo)
  glGenBuffers (3, vbo);

  //Set 0 as the current array to be used by binding it
  glBindVertexArray (vao[0]);
  //Set 0 as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);

  sphere(earthRadius);

  oldSize = mymodel.size();
  for ( int i = 0; i < num_vertices; ++i ) {
    mymodel.push_back({v_positions[i][0], v_positions[i][1], v_positions[i][2], v_positions[i][3], v_normals[i][0], v_normals[i][1], v_normals[i][2], v_normals[i][3], tex_coords[i][0], tex_coords[i][1], 0.0, 0.0});
  }

  output_file << num_vertices;
  output_file << " ";
  output_file << "1\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof (v_positions) + sizeof(tex_coords) + sizeof(v_normals), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(v_positions), v_positions );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(v_positions), sizeof(tex_coords), tex_coords);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(tex_coords)+sizeof(v_positions), sizeof(v_normals), v_normals );
  // set up vertex array
  //Position
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  //Textures
  glEnableVertexAttribArray( texCoord );
  glVertexAttribPointer( texCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions)) );
  //Normal
  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions)+sizeof(tex_coords)) );

  //Set 1 as the current array to be used by binding it
  glBindVertexArray (vao[1]);
  //Set 1 as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);

  sphere(earthRadius+delta);

  oldSize = mymodel.size();
  for ( int i = 0; i < num_vertices; ++i ) {
    mymodel.push_back({v_positions[i][0], v_positions[i][1], v_positions[i][2], v_positions[i][3], v_normals[i][0], v_normals[i][1], v_normals[i][2], v_normals[i][3], tex_coords[i][0], tex_coords[i][1], 0.0, 0.0});
  }

  output_file << num_vertices;
  output_file << " ";
  output_file << "1\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof (v_positions) + sizeof(tex_coords) + sizeof(v_normals), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(v_positions), v_positions );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(v_positions), sizeof(tex_coords), tex_coords);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(tex_coords)+sizeof(v_positions), sizeof(v_normals), v_normals );
  // set up vertex array
  //Position
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  //Textures
  glEnableVertexAttribArray( texCoord );
  glVertexAttribPointer( texCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions)) );
  //Normal
  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions)+sizeof(tex_coords)) );

  //Set 2 as the current array to be used by binding it
  glBindVertexArray (vao[2]);
  //Set 2 as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[2]);

  sphere(spaceRadius);

  oldSize = mymodel.size();
  for ( int i = 0; i < num_vertices; ++i ) {
    mymodel.push_back({v_positions[i][0], v_positions[i][1], v_positions[i][2], v_positions[i][3], v_normals[i][0], v_normals[i][1], v_normals[i][2], v_normals[i][3], tex_coords[i][0], tex_coords[i][1], 0.0, 0.0});
  }

  output_file << num_vertices;
  output_file << " ";
  output_file << "1\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof (v_positions) + sizeof(tex_coords) + sizeof(v_normals), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(v_positions), v_positions );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(v_positions), sizeof(tex_coords), tex_coords);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(tex_coords)+sizeof(v_positions), sizeof(v_normals), v_normals );
  // glBufferSubData( GL_ARRAY_BUFFER, sizeof(tex_coords)+sizeof(v_positions)+sizeof(v_normals), sizeof(v_colors), v_colors );
  // set up vertex array
  //Position
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  //Textures
  glEnableVertexAttribArray( texCoord );
  glVertexAttribPointer( texCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions)) );
  //Normal
  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions)+sizeof(tex_coords)) );
  // //Color
  // glEnableVertexAttribArray( vColor );
  // glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions)+sizeof(tex_coords)+sizeof(v_normals)) );
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
  //  glBindTexture(GL_TEXTURE_2D, tex);
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, tex[0]);
  glBindVertexArray (vao[0]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, num_vertices);

   // Draw the clouds
  modelview_matrix = view_matrix*model_matrix_earth;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  //  glBindTexture(GL_TEXTURE_2D, tex);
  glBindTexture(GL_TEXTURE_2D, tex[1]);
  glBindVertexArray (vao[1]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, num_vertices);

  modelview_matrix = view_matrix*model_matrix_space;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[2]);
  glBindVertexArray (vao[2]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, num_vertices);
  
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

//-------------------------------------------------------------------------

