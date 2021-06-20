#include "gl_framework.hpp"

extern GLfloat xrot,yrot,zrot,c_xrot,c_yrot,c_zrot,c_xpos,c_ypos,c_zpos;
extern float earthRadius,spaceRadius,f_N;
extern bool enable_perspective;

float d = sqrt(c_xpos*c_xpos + c_ypos*c_ypos + c_zpos*c_zpos);

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
    else if (key == GLFW_KEY_LEFT) c_yrot += 1;
    else if (key == GLFW_KEY_RIGHT) c_yrot -= 1;
    else if (key == GLFW_KEY_UP) c_xrot += 1;
    else if (key == GLFW_KEY_DOWN) c_xrot -= 1;
    else if (key == GLFW_KEY_LEFT_BRACKET && d > earthRadius+f_N+1){      
      c_zpos -= 0.01*c_zpos;
      d = sqrt(c_xpos*c_xpos + c_ypos*c_ypos + c_zpos*c_zpos);
      if(d < earthRadius){
        d = earthRadius+f_N+1;
        c_zpos = sqrt(d*d - (c_xpos*c_xpos + c_ypos*c_ypos));
      }  
    }
    else if (key == GLFW_KEY_RIGHT_BRACKET && d < spaceRadius-1){
      c_zpos += 0.01*c_zpos;
      d = sqrt(c_xpos*c_xpos + c_ypos*c_ypos + c_zpos*c_zpos);
      if(d > spaceRadius){
        d = spaceRadius-1;
        c_zpos = sqrt(d*d - (c_xpos*c_xpos + c_ypos*c_ypos));
      }  
    }
    else if (key == GLFW_KEY_A  )
      zrot -= 1.0;
    else if (key == GLFW_KEY_D  )
      zrot += 1.0;
    else if (key == GLFW_KEY_W  )
      xrot -= 1.0;
    else if (key == GLFW_KEY_S  )
      xrot += 1.0;        
    else if (key == GLFW_KEY_Q  )
      yrot -= 1.0;
    else if (key == GLFW_KEY_E  )
      yrot += 1.0;   
  }
};  
  


