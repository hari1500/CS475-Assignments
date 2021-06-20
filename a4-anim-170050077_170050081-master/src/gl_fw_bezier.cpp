#include "gl_fw_bezier.hpp"
#include "bezier.hpp"

//-----------------------------------------------------------------
extern GLuint uModelViewMatrix, vPosition, vColor;
// vaos and vbos
extern GLuint vbo[NUM_BUFFERS], vao[NUM_BUFFERS];

extern glm::vec4 planePoints[PLANE_POINTS];
extern glm::vec4 planeColors[PLANE_POINTS];

// Translation Parameters
extern GLfloat xpos,ypos,zpos;
// Rotation Parameters
extern GLfloat xrot,yrot,zrot;
// translation parameters for cursor
extern GLfloat cx,cy,cz;

extern bool change;

// storing selected points, colors
extern std::vector<std::vector<glm::vec4>> controlPoints;
extern int NUMCONTROLPOINTS;

//Running variable to toggle culling on/off
extern bool enable_culling;
//Running variable to toggle wireframe/solid modelling
extern bool solid;
//-----------------------------------------------------------------

bool compareArray(std::array<float, 6> A1, std::array<float, 6> A2){
  return (A1[0] < A2[0] || (A1[0]==A2[0] && A1[1]<A2[1]) || (A1[0]==A2[0] && A1[1]==A2[1] && A1[2]<A2[2]));
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
  
  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    
    else if (key == GLFW_KEY_A)// && action == GLFW_PRESS)
      yrot -= 1.0;
    
    else if (key == GLFW_KEY_D)// && action == GLFW_PRESS)
      yrot += 1.0;
    
    else if (key == GLFW_KEY_W)// && action == GLFW_PRESS)
      xrot -= 1.0;
    
    else if (key == GLFW_KEY_S)// && action == GLFW_PRESS)
      xrot += 1.0;
    
    else if (key == GLFW_KEY_Q)// && action == GLFW_PRESS)
      zrot += 1.0;
    
    else if (key == GLFW_KEY_E)// && action == GLFW_PRESS)
      zrot -= 1.0;
    
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) 
        cx = cx<GRID_SIZE-1 ? cx+1 : GRID_SIZE-1;
        
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)    
        cx = cx>=1 ? cx-1 : 0;

    else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        cy = cy<GRID_SIZE-1 ? cy+1 : GRID_SIZE-1;
        
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)    
        cy = cy>=1 ? cy-1 : 0;

    else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
        cz = cz<GRID_SIZE-1 ? cz+1 : GRID_SIZE-1;
        
    else if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)    
        cz = cz>=1 ? cz-1 : 0;

    else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS){
        change = true;
        int position = -1;
        int idx = controlPoints.size()-1;
        if(idx == -1){
            std::vector<glm::vec4> temp;
            temp.push_back(glm::vec4(cx, cy, cz, 0));
            controlPoints.push_back(temp);
        }
        else{
            controlPoints[idx].push_back(glm::vec4(cx, cy, cz, 0));
        }
        NUMCONTROLPOINTS++;
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS){
        change = true;
        int position = -1;
        int idx = controlPoints.size()-1;
        std::vector<glm::vec4> lastcP = controlPoints[idx];
        for ( int i = 0; i < lastcP.size(); ++i ) {
            if (lastcP[i][0] == cx && lastcP[i][1] == cy && lastcP[i][2] == cz) {
            position = i;
            break;
            }
        }
        if (position >= 0) {
            lastcP.erase(lastcP.begin()+position);
            controlPoints[idx] = lastcP;
            NUMCONTROLPOINTS--;
        }
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS){
        change = true;
        controlPoints.clear();
        NUMCONTROLPOINTS = 0;
    }

    else if (key == GLFW_KEY_T && action == GLFW_PRESS){
        std::string filename = "./data/trajectory.txt";  
        std::ofstream output_file(filename, std::ios::app);
        std::ostream_iterator<float> output_iter(output_file, " ");
        std::array<float, 3> controlPoint;

        int idx = controlPoints.size()-1;
        std::vector<glm::vec4> lastcP = controlPoints[idx];

        output_file << std::to_string(lastcP.size()) <<std::endl;

        for ( int i = 0; i < lastcP.size(); ++i ) {
            output_file << std::to_string(lastcP[i][0]) << " " << std::to_string(lastcP[i][1]) << " " << std::to_string(lastcP[i][2]) << std::endl;
        }
        std::vector<glm::vec4> temp;
        temp.clear();
        controlPoints.push_back(temp);
    }
    else if (key == GLFW_KEY_Y && action == GLFW_PRESS){
        change = true;
        controlPoints.clear();
        NUMCONTROLPOINTS = 0;
        std::string filename = "./data/trajectory.txt";  
    
        std::fstream file;
        std::string word;

        file.open(filename.c_str());
        while(true){
            std::string numPoints;
            if(!(file>>numPoints)) break;
            std::vector<glm::vec4> lastcP;
            for(int i=0;i<stoi(numPoints);++i){
                std::string cx, cy, cz;
                file>>cx; file>>cy; file>>cz;
                lastcP.push_back(glm::vec4(std::stof(cx),std::stof(cy),std::stof(cz), 0.0));
            }
            NUMCONTROLPOINTS += stoi(numPoints);
            controlPoints.push_back(lastcP);
        }
        std::vector<glm::vec4> temp;
        temp.clear();
        controlPoints.push_back(temp);
        file.close();
    }
  }
};
