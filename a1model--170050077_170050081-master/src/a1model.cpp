#include "a1model.hpp"

GLuint shaderProgram;
glm::mat4 grid_translation_1;
glm::mat4 grid_translation_2;
glm::mat4 rotation_matrix;
glm::mat4 translation_matrix;
glm::mat4 ortho_matrix;
glm::mat4 modelview_matrix;

std::vector<GLuint> shaderList;


//-----------------------------------------------------------------
GLuint uModelViewMatrix, vPosition, vColor;
// vaos and vbos
GLuint vbo[NUM_BUFFERS], vao[NUM_BUFFERS];

glm::vec4 grid_points[GRID_POINTS];
glm::vec4 points_colors[GRID_POINTS];
glm::vec4 plane_points[PLANE_POINTS];
glm::vec4 plane_colors[PLANE_POINTS];

// Translation Parameters
GLfloat xpos=0.0,ypos=0.0,zpos=0.0;
// Rotation Parameters
GLfloat xrot=0.0,yrot=0.0,zrot=0.0;
// translation parameters for cursor
GLfloat cx=0.0,cy=0.0,cz=0.0;


int toggle = 0;
bool change = false;

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


std::vector<glm::vec4> cur_points;
std::vector<glm::vec4> cur_colors;
std::vector<glm::vec4> model_points;
std::vector<glm::vec4> model_colors;

glm::vec4 LIGHT_GREY = glm::vec4(0.658824, 0.658824, 0.658824, 1.0);

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
      points_colors[2*(N_LINES*i + j)] = LIGHT_GREY;
      points_colors[2*(N_LINES*i + j)+1] = LIGHT_GREY;
      points_colors[N_LINES*N_LINES*2 + 2*(N_LINES*i + j)] = LIGHT_GREY;
      points_colors[N_LINES*N_LINES*2 + 2*(N_LINES*i + j)+1] = LIGHT_GREY;
      points_colors[N_LINES*N_LINES*4 + 2*(N_LINES*i + j)] = LIGHT_GREY;
      points_colors[N_LINES*N_LINES*4 + 2*(N_LINES*i + j)+1] = LIGHT_GREY;
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
  cur_points.clear();
  cur_colors.clear();
  cur_points = setPositionsCube(glm::vec4(0.0, 0.0, 0.0, 0.0));
  cur_colors = setColorsCube(glm::vec4(0.0, 1.0, 0.0, 1.0));
  model_points.clear();
  model_colors.clear();
  for (int i=0; i < selectedPoints.size(); ++i) {
    std::vector<glm::vec4> temp = setPositionsCube(selectedPoints[i]);
    model_points.insert(model_points.end(), temp.begin(), temp.end());
    temp = setColorsCube(selectedColors[i]);
    model_colors.insert(model_colors.end(), temp.begin(), temp.end());
  }
  
  glm::vec4 cur_points_array[cur_points.size()];
  glm::vec4 cur_colors_array[cur_colors.size()];
  glm::vec4 model_points_array[model_points.size()];
  glm::vec4 model_colors_array[model_colors.size()];
  std::copy(cur_points.begin(), cur_points.end(), cur_points_array);
  std::copy(cur_colors.begin(), cur_colors.end(), cur_colors_array);
  std::copy(model_points.begin(), model_points.end(), model_points_array);
  std::copy(model_colors.begin(), model_colors.end(), model_colors_array);

  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (NUM_BUFFERS, vao);
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (NUM_BUFFERS, vbo);
  
  //////////////////////////////////// grid /////////////////////////////////////////////////////////
  //Set it as the current array to be used by binding it
  glBindVertexArray (vao[0]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(grid_points) + sizeof(points_colors), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(grid_points), grid_points);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(grid_points), sizeof(points_colors), points_colors);

  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(grid_points)));

  ////////////////////////////////// cursorcube ///////////////////////////////////////////

  //Set it as the current array to be used by binding it
  glBindVertexArray (vao[1]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(cur_points_array) + sizeof(cur_colors_array), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cur_points_array), cur_points_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(cur_points_array), sizeof(cur_colors_array), cur_colors_array);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cur_points_array)));  

  // ////////////////////////////////  model  /////////////////////////////////////////////////
  // Set it as the current array to be used by binding it
  glBindVertexArray (vao[2]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[2]);
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

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f,1.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f,0.0f,1.0f));

  translation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(cx, cy, cz));

  ortho_matrix = glm::ortho(-1.5*GRID_SIZE, 2.5*GRID_SIZE, -1.5*GRID_SIZE, 2.5*GRID_SIZE, -1.5*GRID_SIZE, 2.5*GRID_SIZE);

  glm::mat4 orth_mul_rot = ortho_matrix * grid_translation_2 * rotation_matrix * grid_translation_1;

  ////////////////////////////////////////// grid ////////////////////////////////////////////////
  
  modelview_matrix = orth_mul_rot;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  
  glBindVertexArray (vao[0]);
  if(toggle == 0) glDrawArrays(GL_LINES, 0, GRID_POINTS);
  else glDrawArrays(GL_LINES, 0, PLANE_POINTS);

  //////////////////////////////////////// cursor cube ///////////////////////////////////////////

  modelview_matrix = orth_mul_rot * translation_matrix;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  
  glBindVertexArray(vao[1]);
  glDrawArrays(GL_TRIANGLES, 0, cur_points.size());

  /////////////////////////////////////// model //////////////////////////////////////////////////

  modelview_matrix = orth_mul_rot;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));

  glBindVertexArray(vao[2]);

  if(change){
    model_points.clear();
    model_colors.clear();
    for (int i=0; i < selectedPoints.size(); ++i) {
      std::vector<glm::vec4> temp = setPositionsCube(selectedPoints[i]);
      model_points.insert(model_points.end(), temp.begin(), temp.end());
      temp = setColorsCube(selectedColors[i]);
      model_colors.insert(model_colors.end(), temp.begin(), temp.end());
    }

    glm::vec4 model_points_array[model_points.size()];
    glm::vec4 model_colors_array[model_colors.size()];
    std::copy(model_points.begin(), model_points.end(), model_points_array);
    std::copy(model_colors.begin(), model_colors.end(), model_colors_array);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    //Copy the points into the current buffer
    glBufferData (GL_ARRAY_BUFFER, sizeof(model_points_array) + sizeof(model_colors_array), NULL, GL_STATIC_DRAW);
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(model_points_array), model_points_array);
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(model_points_array), sizeof(model_colors_array), model_colors_array);

    // set up vertex arrays
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(model_points_array)));
    change = false;
  }
  
  glDrawArrays(GL_TRIANGLES, 0, model_points.size());
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
  window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "a1model", NULL, NULL);
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
  std::cout<<"Current mode is INSPECTION mode\n";

  //Keyboard Callback
  glfwSetKeyCallback(window, csX75::key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


  // Load shaders and use the resulting shader program
  std::string vertex_shader_file("./src/a1model_vshader.glsl");
  std::string fragment_shader_file("./src/a1model_fshader.glsl");

  
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
