#include "gl_framework.hpp"
#include "scene.hpp"

// Rotation Parameters
extern glm::vec4 c_pos, c_rot, c_pos_ini, c_rot_ini;

extern float t_shift, depBase, depTower, depPad, prot_shift, protrusionLength;
extern GLfloat panel1_xrot,panel1_yrot,panel1_zrot,panel2_xrot,panel2_yrot,panel2_zrot;
extern csX75::HNode *curr_node, *l1_cone, *l1_rem, *l2, *l3_center, *l3_left, *l3_right, *root_node;
extern glm::vec3 payload_shift;
extern int enable_playback, start_or_stop, pause_or_resume;

glm::vec4 c1_pos = c_pos_ini, c1_rot = c_rot_ini, c2_pos, c2_rot;

int C1 = 1;
int C2 = 2;
extern int cur_mode; 
extern int ROCKETSTAGE;

void get_c2(){
  // c2_pos = -root_node->get_trans_params()+glm::vec4(0.0, -4.0, 0.0, 0.0);
  // c2_rot = -root_node->get_rot_params()+glm::vec4(0.0, 180.0, 0.0, 0.0);
  c2_pos = glm::vec4(payload_shift[0], payload_shift[1], payload_shift[2], 1.0)+glm::vec4(10.0, 30.0, 10.0, 0.0);
  c2_rot = glm::vec4(0.0, 0.0, 0.0, 0.0);
}

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

  void update_cam(){
    if(cur_mode == C1) {
      c_pos = c1_pos;
      c_rot = c1_rot;
    } else {
      get_c2();
      c_pos = c2_pos;
      c_rot = c2_rot;
    }
  }

  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_C && ROCKETSTAGE < 3){
      cur_mode = (cur_mode == C1) ? C2 : C1;
      update_cam();
    }
    else if (key == GLFW_KEY_LEFT && cur_mode == C1){
      c1_pos[0] -= 1.0;
      update_cam();
    }
    else if (key == GLFW_KEY_RIGHT && cur_mode == C1){
      c1_pos[0] += 1.0;
      update_cam();
    }
    else if (key == GLFW_KEY_UP && cur_mode == C1){
      c1_pos[1] += 1.0;
      update_cam();
    }
    else if (key == GLFW_KEY_DOWN && cur_mode == C1){
      c1_pos[1] -= 1.0;
      update_cam();
    }
    else if (key == GLFW_KEY_LEFT_BRACKET && cur_mode == C1){
      c1_pos[2] -= 1.0;
      update_cam();
    }
    else if (key == GLFW_KEY_RIGHT_BRACKET&& cur_mode == C1){
      c1_pos[2] += 1.0;
      update_cam();
    }
    else if (key == GLFW_KEY_A  && cur_mode == C1){
      c1_rot[1] += 1.0;
      update_cam();
    }
    else if (key == GLFW_KEY_D  && cur_mode == C1){
      c1_rot[1] -= 1.0;
      update_cam();
    }
    else if (key == GLFW_KEY_W  && cur_mode == C1){
      c1_rot[0] += 1.0;
      update_cam();
    }
    else if (key == GLFW_KEY_S  && cur_mode == C1){
      c1_rot[0] -= 1.0;        
      update_cam();
    }
    else if (key == GLFW_KEY_Q  && cur_mode == C1){
      c1_rot[2] -= 1.0;
      update_cam();
    }
    else if (key == GLFW_KEY_E  && cur_mode == C1){
      c1_rot[2] += 1.0;  
      update_cam();
    }
    else if (key == GLFW_KEY_T ){//&& action == GLFW_PRESS) { // shifting tower
      if(mods == 0x0001) t_shift = t_shift > 0 ? t_shift-1 : 0;
      else t_shift = t_shift >= (depBase-depPad)/2-depTower ? (depBase-depPad)/2-depTower : t_shift+1;
    }
    else if (key == GLFW_KEY_R ){//&& action == GLFW_PRESS) { // shifting protrusions
      if(mods == 0x0001) prot_shift = prot_shift > -protrusionLength ? prot_shift-1 : -protrusionLength;
      else prot_shift = prot_shift >= 0 ? 0 : prot_shift+1;
    }
    else if (key == GLFW_KEY_O ) { // rotate panel1 
      panel1_xrot = 0;
      if(mods == 0x0001) panel1_zrot = panel1_zrot > 0 ? panel1_zrot-1 : 0;
      else panel1_zrot = panel1_zrot >= 90 ? 90 : panel1_zrot+1;
    }
    else if (key == GLFW_KEY_U  && panel1_zrot == 90) { // rotate panel1 
      if(mods == 0x0001) panel1_xrot = (int(panel1_xrot)-1)%360;
      else panel1_xrot = (int(panel1_xrot+1))%360;
    }
    else if (key == GLFW_KEY_Y) { // rotate panel2
      panel2_xrot = 0;  
      if(mods == 0x0001) panel2_zrot = panel2_zrot > 0 ? panel2_zrot-1 : 0;
      else panel2_zrot = panel2_zrot >= 90 ? 90 : panel2_zrot+1;
    }
    else if (key == GLFW_KEY_I  && panel2_zrot == 90) { // rotate panel2 
      if(mods == 0x0001) panel2_xrot = (int(panel2_xrot)-1)%360;
      else panel2_xrot = (int(panel2_xrot)+1)%360;
    }
    else if (key == GLFW_KEY_J){
      if(mods == 0x0001) payload_shift[0] -= 1;
      else payload_shift[0] += 1;
      update_cam();
    }
    else if (key == GLFW_KEY_K){
      if(mods == 0x0001) payload_shift[1] -= 1;
      else payload_shift[1] += 1;
      update_cam();
    }
    else if (key == GLFW_KEY_L){
      if(mods == 0x0001) payload_shift[2] -= 1;
      else payload_shift[2] += 1;
      update_cam();
    }
    else if (key == GLFW_KEY_P) 
      enable_playback = 1;
    // else if (key == GLFW_KEY_8 && action == GLFW_PRESS)
    //   start_or_stop = !start_or_stop;
    // else if (key == GLFW_KEY_9 && action == GLFW_PRESS)
    //   pause_or_resume = !pause_or_resume;
  }
};
