#include "gl_framework.hpp"
#include "a2view.hpp"

//-----------------------------------------------------------------
extern GLuint uModelViewMatrix, vPosition, vColor;
// grid vao and vbo for use in Hide
extern GLuint gridvao, gridvbo;

extern glm::vec4 grid_points[GRID_POINTS];
extern glm::vec4 grid_colors[GRID_POINTS];
extern glm::vec4 plane_points[PLANE_POINTS];
extern glm::vec4 plane_colors[PLANE_POINTS];

// Rotation Parameters
extern GLfloat xrot,yrot,zrot;

// grid mode, full or only coordinate planes
extern int toggle;

// default color
extern glm::vec4 defaultColor;
// current color
extern glm::vec4 currColor;
// storing selected points, colors
extern std::vector<glm::vec4> selectedPoints;
extern std::vector<glm::vec4> selectedColors;

//Running variable to toggle culling on/off
extern bool enable_culling;
//Running variable to toggle wireframe/solid modelling
extern bool solid;

extern int mode;
//-----------------------------------------------------------------

namespace csX75
{
  //! Initialize GL State
  void initGL(void)
  {
    //Set framebuffer clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //Set depth buffer furthest depth
    glClearDepth(1.0);
    //Set depth test to less-than
    glDepthFunc(GL_LESS);
    //Enable depth testing
    glEnable(GL_DEPTH_TEST);
  }
  
  //!GLFW Error Callback
  void error_callback(int error, const char* description)
  {
    std::cerr<<description<<std::endl;
  }
  
  //!GLFW framebuffer resize callback
  void framebuffer_size_callback(GLFWwindow* window, int width, int height)
  {
    //!Resize the viewport to fit the window size - draw to entire window
    glViewport(0, 0, width, height);
  }
  
  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    
    else if(key == GLFW_KEY_0 && action == GLFW_PRESS){
      mode = 0;
      xrot = 0.0;
      yrot = 0.0;
      zrot = 0.0;
      std::cout<<"Current mode is WCS mode\n";
    }
    else if(key == GLFW_KEY_1 && action == GLFW_PRESS){
      mode = 1;
      xrot = 0.0;
      yrot = 0.0;
      zrot = 0.0;
      std::cout<<"Current mode is VCS mode\n";
    }
    else if(key == GLFW_KEY_2 && action == GLFW_PRESS){
      mode = 2;
      xrot = 0.0;
      yrot = 0.0;
      zrot = 0.0;
      std::cout<<"Current mode is CCS mode\n";
    }
    else if(key == GLFW_KEY_3 && action == GLFW_PRESS){
      mode = 3;
      xrot = 0.0;
      yrot = 0.0;
      zrot = 0.0;
      std::cout<<"Current mode is NDCS mode\n";
    }
    else if(key == GLFW_KEY_4 && action == GLFW_PRESS){
      mode = 4;
      xrot = 0.0;
      yrot = 0.0;
      zrot = 0.0;
      std::cout<<"Current mode is DCS mode\n";
    }
    else if(key == GLFW_KEY_H && action == GLFW_PRESS){
      toggle = (toggle+1)%2;    // == 0 for full grid, == 1 for just coordinate planes.
      
      //Set it as the current array to be used by binding it
      glBindVertexArray (gridvao);
      //Set it as the current buffer to be used by binding it
      glBindBuffer (GL_ARRAY_BUFFER, gridvbo);

      if (toggle == 0){      
        //Copy the points into the current buffer
        glBufferData (GL_ARRAY_BUFFER, sizeof(grid_points) + sizeof(grid_colors), NULL, GL_STATIC_DRAW);
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(grid_points), grid_points);
        glBufferSubData( GL_ARRAY_BUFFER, sizeof(grid_points), sizeof(grid_colors), grid_colors);

        glEnableVertexAttribArray( vPosition );
        glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
        
        glEnableVertexAttribArray( vColor );
        glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(grid_points)));
      }
      else{
        //Copy the points into the current buffer
        glBufferData (GL_ARRAY_BUFFER, sizeof(plane_points) + sizeof(plane_colors), NULL, GL_STATIC_DRAW);
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(plane_points), plane_points);
        glBufferSubData( GL_ARRAY_BUFFER, sizeof(plane_points), sizeof(plane_colors), plane_colors);

        glEnableVertexAttribArray( vPosition );
        glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
        
        glEnableVertexAttribArray( vColor );
        glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(plane_points)));
      }
    }
    
    else if (key == GLFW_KEY_LEFT)// && action == GLFW_PRESS)
      yrot -= 1.0;
    
    else if (key == GLFW_KEY_RIGHT)// && action == GLFW_PRESS)
      yrot += 1.0;
    
    else if (key == GLFW_KEY_UP)// && action == GLFW_PRESS)
      xrot -= 1.0;
    
    else if (key == GLFW_KEY_DOWN)// && action == GLFW_PRESS)
      xrot += 1.0;
    
    else if (key == GLFW_KEY_LEFT_BRACKET)// && action == GLFW_PRESS)
      zrot += 1.0;
    
    else if (key == GLFW_KEY_RIGHT_BRACKET)// && action == GLFW_PRESS)
      zrot -= 1.0;
  }
};
