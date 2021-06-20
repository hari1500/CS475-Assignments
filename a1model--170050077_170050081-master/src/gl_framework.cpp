#include "gl_framework.hpp"
#include "a1model.hpp"

//-----------------------------------------------------------------
extern GLuint uModelViewMatrix, vPosition, vColor;
// vaos and vbos
extern GLuint vbo[NUM_BUFFERS], vao[NUM_BUFFERS];

extern glm::vec4 grid_points[GRID_POINTS];
extern glm::vec4 points_colors[GRID_POINTS];
extern glm::vec4 plane_points[PLANE_POINTS];
extern glm::vec4 plane_colors[PLANE_POINTS];

// Translation Parameters
extern GLfloat xpos,ypos,zpos;
// Rotation Parameters
extern GLfloat xrot,yrot,zrot;
// translation parameters for cursor
extern GLfloat cx,cy,cz;

// grid mode, full or only coordinate planes
extern int toggle;
extern bool change;

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
//-----------------------------------------------------------------

enum mode { INSPECTION, MODELLING };

mode curMode = INSPECTION; 

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
    
    else if(key == GLFW_KEY_M && action == GLFW_PRESS){
      curMode = MODELLING;
      std::cout<<"Current mode is MODELLING mode\n";
    }
    
    else if(key == GLFW_KEY_I && action == GLFW_PRESS){
      curMode = INSPECTION;
      std::cout<<"Current mode is INSPECTION mode\n";
    }
    
    else if(key == GLFW_KEY_H && action == GLFW_PRESS){
      toggle = (toggle+1)%2;    // == 0 for full grid, == 1 for just coordinate planes.
      
      //Set it as the current array to be used by binding it
      glBindVertexArray (vao[0]);
      //Set it as the current buffer to be used by binding it
      glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);

      if (toggle == 0){      
        //Copy the points into the current buffer
        glBufferData (GL_ARRAY_BUFFER, sizeof(grid_points) + sizeof(points_colors), NULL, GL_STATIC_DRAW);
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(grid_points), grid_points);
        glBufferSubData( GL_ARRAY_BUFFER, sizeof(grid_points), sizeof(points_colors), points_colors);

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
    
    else if(curMode == MODELLING){
      if (key == GLFW_KEY_X && action == GLFW_PRESS){
        if(mods == 0x0001) cx = cx<GRID_SIZE-1 ? cx+1 : GRID_SIZE-1;
        else cx = cx>=1 ? cx-1 : 0;
      }
      else if (key == GLFW_KEY_Y && action == GLFW_PRESS){
        if(mods == 0x0001) cy = cy<GRID_SIZE-1 ? cy+1 : GRID_SIZE-1;
        else cy = cy>=1 ? cy-1 : 0;
      }
      else if (key == GLFW_KEY_Z && action == GLFW_PRESS){
        if(mods == 0x0001) cz = cz<GRID_SIZE-1 ? cz+1 : GRID_SIZE-1;
        else cz = cz>=1 ? cz-1 : 0;
      }
      else if (key == GLFW_KEY_P && action == GLFW_PRESS){
        change = true;
        int position = -1;
        for ( int i = 0; i < selectedPoints.size(); ++i ) {
          if (selectedPoints[i][0] == cx && selectedPoints[i][1] == cy && selectedPoints[i][2] == cz) {
            position = i;
            selectedColors[i] = currColor;
            break;
          }
        }
        if (position == -1) {
          selectedPoints.push_back(glm::vec4(cx, cy, cz, 0));
          selectedColors.push_back(currColor);
        }
      }
      else if (key == GLFW_KEY_D && action == GLFW_PRESS){
        change = true;
        int position = -1;
        for ( int i = 0; i < selectedPoints.size(); ++i ) {
          if (selectedPoints[i][0] == cx && selectedPoints[i][1] == cy && selectedPoints[i][2] == cz) {
            position = i;
            break;
          }
        }
        if (position >= 0) {
          selectedPoints.erase(selectedPoints.begin()+ position);
          selectedColors.erase(selectedColors.begin()+ position);        
        }
      }
      else if (key == GLFW_KEY_C && action == GLFW_PRESS){
        float r, g, b;
        printf("Input r, g, b intensity each in between 0 to 1: ");
        scanf("%f %f %f", &r, &g, &b);
        currColor = glm::vec4(r, g, b, 1.0);
      }
      else if (key == GLFW_KEY_R && action == GLFW_PRESS){
        change = true;
        currColor = defaultColor;
        selectedPoints.clear();
        selectedColors.clear();
      }
      else if (key == GLFW_KEY_S && action == GLFW_PRESS){
        std::vector<std::array<float, 6>> mymodel;
        
        for ( int i = 0; i < selectedPoints.size(); ++i ) {
          mymodel.push_back({selectedPoints[i][0], selectedPoints[i][1], selectedPoints[i][2], selectedColors[i][0], selectedColors[i][1], selectedColors[i][2]});
        }

        std::sort(mymodel.begin(), mymodel.end(), compareArray);
        std::string filename;  
        printf("Enter File name(inluding .raw): ");
        std::cin>>filename;

        filename = "./data/" + filename;
        std::ofstream output_file(filename);
        std::ostream_iterator<float> output_iter(output_file, " ");

        for ( int i = 0; i < selectedPoints.size(); ++i ) {
          copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
          output_file << '\n';
        }
      }
      else if (key == GLFW_KEY_K && action == GLFW_PRESS){
        change = true;
        selectedPoints.clear();
        selectedColors.clear();

        std::string filename;  
        printf("Enter File name(inluding .raw and directory path) : ");
        std::cin>>filename;

        std::fstream file;
        std::string word;

        file.open(filename.c_str());
        while(true){
          std::string cx, cy, cz, r, g, b;
          if(!(file>>cx && file>>cy && file>>cz && file>>r && file>>g && file>>b)) break;  
          selectedPoints.push_back(glm::vec4(std::stof(cx),std::stof(cy),std::stof(cz), 0.0));
          selectedColors.push_back(glm::vec4(std::stof(r),std::stof(g),std::stof(b), 1.0));
        }  
      }
    }
  }
};
