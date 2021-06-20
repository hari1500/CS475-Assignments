#include "gl_framework2.hpp"
#include "model2.hpp"

//-----------------------------------------------------------------
// Rotation Parameters
extern GLfloat xrot,yrot,zrot,c_xrot,c_yrot,c_zrot,c_xpos,c_ypos,c_zpos;
extern GLfloat panel1_xrot,panel1_yrot,panel1_zrot,panel2_xrot,panel2_yrot,panel2_zrot;

//Running variable to toggle culling on/off
extern bool enable_culling;
//Running variable to toggle wireframe/solid modelling
extern bool solid;
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
    else if (key == GLFW_KEY_LEFT )
      c_xpos -= 1.0;
    else if (key == GLFW_KEY_RIGHT )
      c_xpos += 1.0;
    else if (key == GLFW_KEY_UP )
      c_ypos += 1.0;
    else if (key == GLFW_KEY_DOWN )
      c_ypos -= 1.0;
    else if (key == GLFW_KEY_LEFT_BRACKET )
      c_zpos -= 1.0;
    else if (key == GLFW_KEY_RIGHT_BRACKET)
      c_zpos += 1.0;
    else if (key == GLFW_KEY_A  )
      c_yrot += 1.0;
    else if (key == GLFW_KEY_D  )
      c_yrot -= 1.0;
    else if (key == GLFW_KEY_W  )
      c_xrot += 1.0;
    else if (key == GLFW_KEY_S  )
      c_xrot -= 1.0;        
    else if (key == GLFW_KEY_Q  )
      c_zrot -= 1.0;
    else if (key == GLFW_KEY_E  )
      c_zrot += 1.0;  
    else if (key == GLFW_KEY_O ) { // rotate panel1 
      panel1_xrot = 0;
      if(mods == 0x0001) panel1_zrot = panel1_zrot > 0 ? panel1_zrot-1 : 0;
      else panel1_zrot = panel1_zrot >= 90 ? 90 : panel1_zrot+1;
    }
    else if (key == GLFW_KEY_U  && panel1_zrot == 90) { // rotate panel1 
      if(mods == 0x0001) panel1_xrot = (int(panel1_xrot)-1)%360;
      else panel1_xrot = (int(panel1_xrot+1))%360;
    }
    else if (key == GLFW_KEY_P) { // rotate panel2
      panel2_xrot = 0;  
      if(mods == 0x0001) panel2_zrot = panel2_zrot > 0 ? panel2_zrot-1 : 0;
      else panel2_zrot = panel2_zrot >= 90 ? 90 : panel2_zrot+1;
    }
    else if (key == GLFW_KEY_I  && panel2_zrot == 90) { // rotate panel2 
      if(mods == 0x0001) panel2_xrot = (int(panel2_xrot)-1)%360;
      else panel2_xrot = (int(panel2_xrot)+1)%360;
    }
  }
};
