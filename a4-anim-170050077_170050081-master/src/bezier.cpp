#include "bezier.hpp"

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

glm::vec4 planePoints[PLANE_POINTS];
//glm::vec4 plane_colors[PLANE_POINTS];
glm::vec4 planeColor = glm::vec4(1.0,1.0,1.0,1.0);

// Translation Parameters
GLfloat xpos=0.0,ypos=0.0,zpos=0.0;
// Rotation Parameters
GLfloat xrot=0.0,yrot=0.0,zrot=0.0;
// translation parameters for cursor
GLfloat cx=0.0,cy=0.0,cz=0.0;

bool change = false;


// cursor color
glm::vec4 cursorColor=glm::vec4(0.0, 1.0, 0.0, 1.0);;

// storing control points , there is more than one trajectories. Hence vector of vectors.
std::vector<std::vector<glm::vec4>> controlPoints;
//std::vector<glm::vec4> controlColors;
glm::vec4 curveColor = glm::vec4(0.0, 1.0, 1.0, 1.0);
glm::vec4 CURVECOLORS[5] = {glm::vec4(0.0, 1.0, 1.0, 1.0), glm::vec4(1.0, 0.6, 0.8, 1.0), glm::vec4(0.6, 1.0, 0.4, 1.0), glm::vec4(1.0, 1.0, 0.4, 1.0), glm::vec4(1.0, 0.6, 1.0, 1.0)}; 
glm::vec4 controlColor = glm::vec4(0.0, 0.0, 1.0, 1.0);
float interval = 0.1;
int num_points_curve = (1+(1/interval));

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


std::vector<glm::vec4> cursorPoints;
//std::vector<glm::vec4> control_points;
int NUMCONTROLPOINTS = 0;
std::vector<int> offsets0;
std::vector<int> sizes0;
//std::vector<glm::vec4> model_colors;

glm::vec4 LIGHT_GREY = glm::vec4(0.658824, 0.658824, 0.658824, 1.0);

void createPlanes(void)
{
  int N_LINES = GRID_SIZE+1;
  for(int i=0;i<=GRID_SIZE;i++){
    planePoints[4*i] = glm::vec4(0.0, i*1.0, 0.0, 1.0);
    planePoints[4*i + 1] = glm::vec4(GRID_SIZE*1.0, i*1.0, 0.0, 1.0);
    planePoints[4*i + 2] = glm::vec4(i*1.0, 0.0, 0.0, 1.0);
    planePoints[4*i + 3] = glm::vec4(i*1.0, GRID_SIZE*1.0, 0.0, 1.0);
    
    planePoints[4*N_LINES + 4*i] = glm::vec4(0.0, 0.0, i*1.0, 1.0);
    planePoints[4*N_LINES + 4*i + 1] = glm::vec4(0.0, GRID_SIZE*1.0, i*1.0, 1.0);
    planePoints[4*N_LINES + 4*i + 2] = glm::vec4(0.0, i*1.0, 0.0, 1.0);
    planePoints[4*N_LINES + 4*i + 3] = glm::vec4(0.0, i*1.0, GRID_SIZE*1.0, 1.0);
    
    planePoints[8*N_LINES + 4*i] = glm::vec4(0.0, 0.0, i*1.0, 1.0);
    planePoints[8*N_LINES + 4*i + 1] = glm::vec4(GRID_SIZE*1.0, 0.0, i*1.0, 1.0);
    planePoints[8*N_LINES + 4*i + 2] = glm::vec4(i*1.0, 0.0, 0.0, 1.0);
    planePoints[8*N_LINES + 4*i + 3] = glm::vec4(i*1.0, 0.0, GRID_SIZE*1.0, 1.0);
    
    // assign light grey colors to the points
    // plane_colors[4*i] = LIGHT_GREY;
    // plane_colors[4*i + 1] = LIGHT_GREY;
    // plane_colors[4*i + 2] = LIGHT_GREY;
    // plane_colors[4*i + 3] = LIGHT_GREY;
     
    // plane_colors[4*N_LINES + 4*i] = LIGHT_GREY;
    // plane_colors[4*N_LINES + 4*i + 1] = LIGHT_GREY;
    // plane_colors[4*N_LINES + 4*i + 2] = LIGHT_GREY;
    // plane_colors[4*N_LINES + 4*i + 3] = LIGHT_GREY;

    // plane_colors[8*N_LINES + 4*i] = LIGHT_GREY;
    // plane_colors[8*N_LINES + 4*i + 1] = LIGHT_GREY;
    // plane_colors[8*N_LINES + 4*i + 2] = LIGHT_GREY;
    // plane_colors[8*N_LINES + 4*i + 3] = LIGHT_GREY;
  }
}


// std::vector<glm::vec4> setColorsCube(glm::vec4 col)
// {
//   std::vector<glm::vec4> out_colors;
//   for(int i=0; i<NUM_POINTS_PER_CUBE; ++i) out_colors.push_back(col);
//   return out_colors;
// }

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

int factorial(int n){
  if(n == 0 || n == 1) return 1;
  else return n*factorial(n-1);
}

int nCr(int n, int r){
  if(n-r < r) return nCr(n, n-r);
  int prod = 1;
  for(int i=n;i>=(n-r+1);--i){
    prod *= i;
  }
  return prod/factorial(r);
}

//-----------------------------------------------------------------

void initBuffersGL(void)
{
  createPlanes();
  
  cursorPoints.clear();
  cursorPoints = setPositionsCube(glm::vec4(0.0, 0.0, 0.0, 0.0));  
  glm::vec4 cur_points_array[cursorPoints.size()];
  std::copy(cursorPoints.begin(), cursorPoints.end(), cur_points_array);

  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (NUM_BUFFERS, vao);
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (NUM_BUFFERS, vbo);
  
  //////////////////////////////////// grid planes /////////////////////////////////////////////////////////
  //Set it as the current array to be used by binding it
  glBindVertexArray (vao[0]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(planePoints), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(planePoints), planePoints);
  //glBufferSubData( GL_ARRAY_BUFFER, sizeof(grid_points), sizeof(points_colors), points_colors);

  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  ////////////////////////////////// cursorcube ///////////////////////////////////////////

  //Set it as the current array to be used by binding it
  glBindVertexArray (vao[1]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(cur_points_array), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(cur_points_array), cur_points_array);
  //glBufferSubData( GL_ARRAY_BUFFER, sizeof(cur_points_array), sizeof(cur_colors_array), cur_colors_array);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  grid_translation_1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0*GRID_SIZE/2.0, -1.0*GRID_SIZE/2.0, -1.0*GRID_SIZE/2.0));
  grid_translation_2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0*GRID_SIZE/2.0, 1.0*GRID_SIZE/2.0, 1.0*GRID_SIZE/2.0));
  
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f,1.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f,0.0f,1.0f));

  translation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(cx, cy, cz));

  ortho_matrix = glm::ortho(-1.0*GRID_SIZE, 2.0*GRID_SIZE, -1.0*GRID_SIZE, 2.0*GRID_SIZE, -1.5*GRID_SIZE, 2.5*GRID_SIZE);

  glm::mat4 orth_mul_rot = ortho_matrix * grid_translation_2 * rotation_matrix * grid_translation_1;

  ////////////////////////////////////////// grid ////////////////////////////////////////////////
  
  modelview_matrix = orth_mul_rot;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  glUniform4f(vColor, planeColor[0], planeColor[1], planeColor[2], planeColor[3]);

  glBindVertexArray (vao[0]);
  glDrawArrays(GL_LINES, 0, PLANE_POINTS);

  //////////////////////////////////////// cursor cube ///////////////////////////////////////////

  modelview_matrix = orth_mul_rot * translation_matrix;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  glUniform4f(vColor, cursorColor[0], cursorColor[1], cursorColor[2], cursorColor[3]);

  glBindVertexArray(vao[1]);
  glDrawArrays(GL_TRIANGLES, 0, cursorPoints.size());

  /////////////////////////////////////// controlPoints //////////////////////////////////////////////////

  modelview_matrix = orth_mul_rot;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  glUniform4f(vColor, controlColor[0], controlColor[1], controlColor[2], controlColor[3]);  
  glBindVertexArray(vao[2]);

  if(change){
    offsets0.clear();
	  sizes0.clear();
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
  	glBufferData (GL_ARRAY_BUFFER, sizeof(glm::vec4)*NUMCONTROLPOINTS, NULL, GL_STATIC_DRAW);
	  int offset = 0;
	  for (int i=0; i < controlPoints.size(); ++i) {
      std::vector<glm::vec4> lastCP = controlPoints[i];
      int s = lastCP.size();
      glm::vec4 control_points_array[s];
		  for(int j=0;j<s;++j){
        control_points_array[j] = lastCP[j] + glm::vec4(0.5,0.5,0.5,1.0);
      }  

      glBufferSubData( GL_ARRAY_BUFFER, offset * sizeof(glm::vec4), s * sizeof(glm::vec4), control_points_array);
      offsets0.push_back(offset);
      sizes0.push_back(s);
      offset += s;
    }
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  }

  for(int i=0;i<offsets0.size();i++){
		glDrawArrays(GL_LINE_STRIP, offsets0[i], sizes0[i]);
		//std::cout<<sizes[i]<<" "<<offsets[i]<<std::endl;
	}
  //////////////////////////////////////// curve ////////////////////////////////////
  modelview_matrix = orth_mul_rot;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  glBindVertexArray(vao[3]);
  
  if(change){
	  int offset = 0;
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
  	glBufferData (GL_ARRAY_BUFFER, sizeof(glm::vec4)*num_points_curve*controlPoints.size(), NULL, GL_STATIC_DRAW);
	  for (int idx=0; idx < controlPoints.size(); ++idx) {
      
      std::vector<glm::vec4> lastCP = controlPoints[idx];
      glm::vec4 curve_points_array[num_points_curve];
		  int s = lastCP.size();
      for(int idx1=0;idx1<num_points_curve;++idx1){
      //for(float t=0.0;t<=1.01;t+=interval){
        float t = idx1*interval;
        glm::vec4 P_t = glm::vec4(0.0);
        for(int i=0;i<s;++i){
          float j = nCr(s-1,i)*pow(t, i)*pow(1-t, s-1-i);
          //std::cout<<j<<" "<<t<<std::endl;
          P_t += (j*lastCP[i]);
        }
        curve_points_array[idx1] = P_t + glm::vec4(0.5, 0.5, 0.5, 1.0);
      }  
      glBufferSubData( GL_ARRAY_BUFFER, offset * sizeof(glm::vec4), num_points_curve * sizeof(glm::vec4), curve_points_array);
      offset += num_points_curve;
    }
    change = false;
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  }
  //std::cout<<offsets1.size()<<std::endl;
  for(int i=0;i<controlPoints.size();++i){
    curveColor = CURVECOLORS[i%5];
    glUniform4f(vColor, curveColor[0], curveColor[1], curveColor[2], curveColor[3]);
		glDrawArrays(GL_LINE_STRIP, i*num_points_curve, num_points_curve);
		//std::cout<<sizes1[i]<<" "<<offsets1[i]<<std::endl;
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
  
  //Keyboard Callback
  glfwSetKeyCallback(window, csX75::key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


  // Load shaders and use the resulting shader program
  std::string vertex_shader_file("./src/simple_vs.glsl");
  std::string fragment_shader_file("./src/simple_fs.glsl");

  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram( shaderProgram );
  
  vPosition = glGetAttribLocation( shaderProgram, "vPosition" );
  vColor = glGetUniformLocation( shaderProgram, "vColor" ); 
  uModelViewMatrix = glGetUniformLocation( shaderProgram, "uModelViewMatrix");

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
