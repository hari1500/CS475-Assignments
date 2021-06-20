#include "gl_framework1.hpp"
#include "model1.hpp"

//-----------------------------------------------------------------
// Rotation Parameters
extern GLfloat xrot,yrot,zrot,c_xrot,c_yrot,c_zrot,c_xpos,c_ypos,c_zpos;

extern float t_shift, depBase, depTower, depPad, prot_shift, protrusionLength;
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
    else if (key == GLFW_KEY_T ){//&& action == GLFW_PRESS) { // shifting tower
      if(mods == 0x0001) t_shift = t_shift > 0 ? t_shift-1 : 0;
      else t_shift = t_shift >= (depBase-depPad)/2-depTower ? (depBase-depPad)/2-depTower : t_shift+1;
    }
    else if (key == GLFW_KEY_Y ){//&& action == GLFW_PRESS) { // shifting protrusions
      if(mods == 0x0001) prot_shift = prot_shift > -protrusionLength ? prot_shift-1 : -protrusionLength;
      else prot_shift = prot_shift >= 0 ? 0 : prot_shift+1;
    }
  }
};
