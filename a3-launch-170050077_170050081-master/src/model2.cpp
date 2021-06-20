#include "model2.hpp"

GLuint shaderProgram1;
glm::mat4 payload_translation;
glm::mat4 rocket_translation;
glm::mat4 rotation_matrix;
glm::mat4 translation_matrix;
glm::mat4 ortho_matrix;
glm::mat4 modelview_matrix;

glm::mat4 model_matrix;
glm::mat4 projection_matrix;
glm::mat4 c_rotation_matrix;
glm::mat4 lookat_matrix;
glm::mat4 view_matrix;
glm::mat3 normal_matrix;
glm::mat4 translation;

glm::mat4 panel_rotation, panel_translation1, panel_translation2;

GLuint uModelViewMatrix1, viewMatrix1, normalMatrix1, texExist1, lx, ly, lz;

std::vector<GLuint> shaderList1;

//-----------------------------------------------------------------
GLuint vPosition1, vColor1, vNormal1, texCoord1;
// vaos and vbos
GLuint vbo[NUM_BUFFERS], vao[NUM_BUFFERS];

// Translation Parameters
GLfloat xpos=0.0,ypos=0.0,zpos=0.0;
// Rotation Parameters
GLfloat xrot=0.0,yrot=0.0,zrot=0.0;
// Camera position and rotation Parameters
GLfloat c_xpos = 0.0, c_ypos = 0.0, c_zpos = 100.0;
GLfloat c_up_x = 0.0, c_up_y = 1.0, c_up_z = 0.0;
GLfloat c_xrot=0.0,c_yrot=0.0,c_zrot=0.0;
// Panel rotations wrt payload
GLfloat panel1_xrot=0.0,panel1_yrot=0.0,panel1_zrot=0.0;
GLfloat panel2_xrot=0.0,panel2_yrot=0.0,panel2_zrot=0.0;
//Running variable to toggle culling on/off
bool enable_culling=true;
//Running variable to toggle wireframe/solid modelling
bool solid=true;
//Enable/Disable perspective view
bool enable_perspective=false;
//-----------------------------------------------------------------

//points in homogenous coordinates
glm::vec4 positions[8];

std::vector<glm::vec4> payload_points, panel1_points, panel2_points, rocket_points;
std::vector<glm::vec4> payload_colors, panel1_colors, panel2_colors, rocket_colors;
std::vector<glm::vec4> payload_normals, panel1_normals, panel2_normals, rocket_normals;
std::vector<int> rocket_strip_indices;
glm::vec4 LIGHT_GREY = glm::vec4(0.658824, 0.658824, 0.658824, 1.0);

void assignPositions(float l, float b, float d) {
  positions[0] = glm::vec4(0, 0, d, 1.0);
  positions[1] = glm::vec4(0, b, d, 1.0);
  positions[2] = glm::vec4(l, b, d, 1.0);
  positions[3] = glm::vec4(l, 0, d, 1.0);
  positions[4] = glm::vec4(0, 0, 0, 1.0);
  positions[5] = glm::vec4(0, b, 0, 1.0);
  positions[6] = glm::vec4(l, b, 0, 1.0);
  positions[7] = glm::vec4(l, 0, 0, 1.0);
}

std::vector<glm::vec4> setColorsCuboid(glm::vec4 col)
{
  std::vector<glm::vec4> out_colors;
  for(int i=0; i<NUM_POINTS_PER_CUBE; ++i) out_colors.push_back(col);
  return out_colors;
}

std::vector<glm::vec4> setPositionsCuboid(glm::vec4 offset, float l, float b, float d)
{
  assignPositions(l, b, d);
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

std::vector<glm::vec4> setNormalsCuboid()
{
  std::vector<glm::vec4> out_normals;

  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(0.0, 0.0, 1.0, 1.0));
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(0.0, -1.0, 0.0, 1.0));
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(0.0, 1.0, 0.0, 1.0));
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(0.0, 0.0, -1.0, 1.0));
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(-1.0, 0.0, 0.0, 1.0));

  return out_normals;
}

glm::vec4 white(0.2, 0.7, 0.7, 1.0);

void cylinder(double radius, double height, glm::vec4 offset)
{
  float h,longs, r;

  int slices = 8; 
  int sectors = height;

  float h1;

  int tri_idx = rocket_points.size();
  rocket_strip_indices.push_back(tri_idx);
  for (h = 0.0; h <= height; h+=sectors)  
  {
    h1 = height;
    for(longs = 0.0; longs <= 360; longs+=slices)
    {
      float longs_rads = glm::radians(longs);
      float x = radius * cos(longs_rads);
      float z = radius * sin(longs_rads);
      float y = h;
      glm::vec4 pt(x, y, z, 1.0), pt1(x, 0.0, z, 1.0);;
      pt += offset;

      rocket_colors.push_back(glm::vec4(1.0));
      rocket_points.push_back(pt); 
      rocket_normals.push_back(pt1);
      tri_idx++;
      
      x = radius * cos(longs_rads);
      z = radius * sin(longs_rads);
      y = h1;
      pt =glm::vec4(x, y, z, 1.0); pt1 = glm::vec4(x, 0.0, z, 1.0);
      pt += offset;

      rocket_colors.push_back(glm::vec4(1.0));
      rocket_points.push_back(pt); 
      rocket_normals.push_back(pt1);
      tri_idx++;
    }
  }
  rocket_strip_indices.push_back(tri_idx);

  // std::cout<<tri_idx<<"\n";

  for (float theta = 0; theta <= 360; theta += slices)
  {
    float theta2 = theta+slices>=360 ? 360 : theta+slices;
    float theta_rads1 = glm::radians(theta), theta_rads2 = glm::radians(theta2);

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + glm::vec4(0, 0.0, 0, 1)); 
    rocket_normals.push_back(glm::vec4(0, -1.0, 0, 1));
    tri_idx++;

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + glm::vec4(radius * cos(theta_rads1), 0, radius * sin(theta_rads1), 1));
    rocket_normals.push_back(glm::vec4(0, -1.0, 0, 1));
    tri_idx++;

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + glm::vec4(radius * cos(theta_rads2), 0, radius * sin(theta_rads2), 1));
    rocket_normals.push_back(glm::vec4(0, -1.0, 0, 1));
    tri_idx++;
  }

  for (float theta = 0; theta <= 360; theta += slices)
  {
    float theta2 = theta+slices>=360 ? 360 : theta+slices;
    float theta_rads1 = glm::radians(theta), theta_rads2 = glm::radians(theta2);

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + glm::vec4(0, height, 0, 1)); 
    rocket_normals.push_back(glm::vec4(0, 1.0, 0, 1));
    tri_idx++;

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + glm::vec4(radius * cos(theta_rads1), height, radius * sin(theta_rads1), 1));
    rocket_normals.push_back(glm::vec4(0, 1.0, 0, 1));
    tri_idx++;

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + glm::vec4(radius * cos(theta_rads2), height, radius * sin(theta_rads2), 1));
    rocket_normals.push_back(glm::vec4(0, 1.0, 0, 1));
    tri_idx++;
  }
}

void cone(double radius, double height, glm::vec4 offset)
{
  float h,longs, r;

  int slices = 2; 
  int sectors = 4;
  float h1;

  int tri_idx = rocket_points.size();
  for (float theta = 0; theta <= 360; theta += slices)
  {
    float theta2 = theta+slices>=360 ? 360 : theta+slices;
    float theta_rads1 = glm::radians(theta), theta_rads2 = glm::radians(theta2);
    glm::vec4 a = glm::vec4(0, height, 0, 1);
    glm::vec4 b = glm::vec4(radius * cos(theta_rads1), 0, radius * sin(theta_rads1), 1);
    glm::vec4 c = glm::vec4(radius * cos(theta_rads2), 0, radius * sin(theta_rads2), 1);

    glm::vec4 x = b-a, y = c-a;
    glm::vec4 normal = -glm::vec4((x[1]*y[2] - x[2]*y[1]), -(x[0]*y[2] - x[2]*y[0]), (x[0]*y[1] - x[1]*y[0]), 1);

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + a); 
    rocket_normals.push_back(normal);
    tri_idx++;

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + b);
    rocket_normals.push_back(normal);
    tri_idx++;

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + c);
    rocket_normals.push_back(normal);
    tri_idx++;
  }
  for (float theta = 0; theta <= 360; theta += slices)
  {
    float theta2 = theta+slices>=360 ? 360 : theta+slices;
    float theta_rads1 = glm::radians(theta), theta_rads2 = glm::radians(theta2);

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + glm::vec4(0, 0.0, 0, 1)); 
    rocket_normals.push_back(glm::vec4(0, -1.0, 0, 1));
    tri_idx++;

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + glm::vec4(radius * cos(theta_rads1), 0, radius * sin(theta_rads1), 1));
    rocket_normals.push_back(glm::vec4(0, -1.0, 0, 1));
    tri_idx++;

    rocket_colors.push_back(glm::vec4(1.0));
    rocket_points.push_back(offset + glm::vec4(radius * cos(theta_rads2), 0, radius * sin(theta_rads2), 1));
    rocket_normals.push_back(glm::vec4(0, -1.0, 0, 1));
    tri_idx++;
  }
}

//-----------------------------------------------------------------

float lenPayload = 20, widPayload = 20, depPayload = 20;
float lenPanel = 2, widPanel = 15, depPanel = 15;

void initBuffersGL(void)
{
  std::vector<std::array<float, 12>> mymodel;
  int oldSize = 0;
  std::string filename = "./data/model2_payload.raw";
  std::ofstream output_file(filename);
  std::ostream_iterator<float> output_iter(output_file, " ");

  std::vector<std::array<float, 12>> mymodel1;
  std::string filename1 = "./data/model2_rocket.raw";
  std::ofstream output_file1(filename1);
  std::ostream_iterator<float> output_iter1(output_file1, " ");

  payload_points.clear();
  payload_colors.clear();
  payload_normals.clear();
  //payload//
  payload_points = setPositionsCuboid(glm::vec4(0.0, 0.0, 0.0, 0.0), lenPayload, widPayload, depPayload);
  payload_colors = setColorsCuboid(glm::vec4(1.0, 1.0, 0.0, 1.0));
  payload_normals = setNormalsCuboid();

  oldSize = mymodel.size();
  for ( int i = 0; i < payload_points.size(); ++i ) {
    mymodel.push_back({payload_points[i][0], payload_points[i][1], payload_points[i][2], payload_points[i][3], payload_normals[i][0], payload_normals[i][1], payload_normals[i][2], payload_normals[i][3], payload_colors[i][0], payload_colors[i][1], payload_colors[i][2], payload_colors[i][2]});
  }

  output_file << payload_points.size();
  output_file << " ";
  output_file << "0\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

  panel1_points.clear();
  panel1_colors.clear();
  panel2_normals.clear();
  //panel1//
  panel1_points = setPositionsCuboid(glm::vec4(-lenPanel, (widPayload-widPanel)/2, (depPayload-depPanel)/2, 0.0), lenPanel, widPanel, depPanel);
  panel1_colors = setColorsCuboid(LIGHT_GREY);
  panel1_normals = setNormalsCuboid();

  oldSize = mymodel.size();
  for ( int i = 0; i < panel1_points.size(); ++i ) {
    mymodel.push_back({panel1_points[i][0], panel1_points[i][1], panel1_points[i][2], panel1_points[i][3], panel1_normals[i][0], panel1_normals[i][1], panel1_normals[i][2], panel1_normals[i][3], panel1_colors[i][0], panel1_colors[i][1], panel1_colors[i][2], panel1_colors[i][2]});
  }

  output_file << panel1_points.size();
  output_file << " ";
  output_file << "0\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

  panel2_points.clear();
  panel2_colors.clear();
  panel2_normals.clear();
  //panel2//
  panel2_points = setPositionsCuboid(glm::vec4(lenPayload, (widPayload-widPanel)/2, (depPayload-depPanel)/2, 0.0), lenPanel, widPanel, depPanel);
  panel2_colors = setColorsCuboid(LIGHT_GREY);
  panel2_normals = setNormalsCuboid();

  oldSize = mymodel.size();
  for ( int i = 0; i < panel2_points.size(); ++i ) {
    mymodel.push_back({panel2_points[i][0], panel2_points[i][1], panel2_points[i][2], panel2_points[i][3], panel2_normals[i][0], panel2_normals[i][1], panel2_normals[i][2], panel2_normals[i][3], panel2_colors[i][0], panel2_colors[i][1], panel2_colors[i][2], panel2_colors[i][2]});
  }

  output_file << panel2_points.size();
  output_file << " ";
  output_file << "0\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

  rocket_points.clear();
  rocket_colors.clear();
  rocket_normals.clear();
  //rocket//
  // cone(20.0, 60.0, glm::vec4(0.0, 0.0, 0.0, 0.0));
  cone(10.0, 20.0, glm::vec4(0.0, 100.0, 0.0, 0.0));
  cylinder(10.0, 100.0, glm::vec4(0.0, 0.0, 0.0, 0.0));
  cylinder(5.0, 50.0, glm::vec4(14.0, 0.0, 0.0, 0.0));
  cone(5.0, 6.0, glm::vec4(14.0, 50.0, 0.0, 0.0));
  cylinder(5.0, 50.0, glm::vec4(-14.0, 0.0, 0.0, 0.0));
  cone(5.0, 6.0, glm::vec4(-14.0, 50.0, 0.0, 0.0));

  cone(3.5, 20.0, glm::vec4(4.0, -4.0, 4.0, 0.0));
  cone(3.5, 20.0, glm::vec4(-4.0, -4.0, 4.0, 0.0));
  cone(3.5, 20.0, glm::vec4(-4.0, -4.0, -4.0, 0.0));
  cone(3.5, 20.0, glm::vec4(4.0, -4.0, -4.0, 0.0));

  for ( int i = 0; i < rocket_points.size(); ++i ) {
    mymodel1.push_back({rocket_points[i][0], rocket_points[i][1], rocket_points[i][2], rocket_points[i][3], rocket_normals[i][0], rocket_normals[i][1], rocket_normals[i][2], rocket_normals[i][3], rocket_colors[i][0], rocket_colors[i][1], rocket_colors[i][2], rocket_colors[i][2]});
  }

  output_file1 << rocket_points.size();
  output_file1 << " ";
  output_file1 << "0\n";
  for ( int i = 0; i < mymodel1.size(); ++i ) {
    copy(mymodel1[i].begin(), mymodel1[i].end(), output_iter1);
    output_file1 << '\n';
  }
  copy(rocket_strip_indices.begin(), rocket_strip_indices.end(), output_iter1);
  output_file1 << '\n';

  // for(int i=0; i<rocket_strip_indices.size();++i) std::cout<<rocket_strip_indices[i]<<" ";
  // std::cout<<"\n";
  // std::cout<<"created rocket \n";

  glm::vec4 payload_points_array[payload_points.size()];
  glm::vec4 payload_colors_array[payload_colors.size()];
  glm::vec4 payload_normals_array[payload_normals.size()];
  std::copy(payload_points.begin(), payload_points.end(), payload_points_array);
  std::copy(payload_colors.begin(), payload_colors.end(), payload_colors_array);
  std::copy(payload_normals.begin(), payload_normals.end(), payload_normals_array);
  
  glm::vec4 panel1_points_array[panel1_points.size()];
  glm::vec4 panel1_colors_array[panel1_colors.size()];
  glm::vec4 panel1_normals_array[panel1_normals.size()];
  std::copy(panel1_points.begin(), panel1_points.end(), panel1_points_array);
  std::copy(panel1_colors.begin(), panel1_colors.end(), panel1_colors_array);
  std::copy(panel1_normals.begin(), panel1_normals.end(), panel1_normals_array);

  glm::vec4 panel2_points_array[panel2_points.size()];
  glm::vec4 panel2_colors_array[panel2_colors.size()];
  glm::vec4 panel2_normals_array[panel2_normals.size()];
  std::copy(panel2_points.begin(), panel2_points.end(), panel2_points_array);
  std::copy(panel2_colors.begin(), panel2_colors.end(), panel2_colors_array);
  std::copy(panel2_normals.begin(), panel2_normals.end(), panel2_normals_array);

  glm::vec4 rocket_points_array[rocket_points.size()];
  glm::vec4 rocket_colors_array[rocket_colors.size()];
  glm::vec4 rocket_normals_array[rocket_normals.size()];
  std::copy(rocket_points.begin(), rocket_points.end(), rocket_points_array);
  std::copy(rocket_colors.begin(), rocket_colors.end(), rocket_colors_array);
  std::copy(rocket_normals.begin(), rocket_normals.end(), rocket_normals_array);
  
  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (NUM_BUFFERS, vao);
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (NUM_BUFFERS, vbo);
  
  // ////////////////////////////////  payload  /////////////////////////////////////////////////
  // Set it as the current array to be used by binding it
  glBindVertexArray (vao[0]);
  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(payload_points_array) + sizeof(payload_colors_array) + sizeof(payload_normals_array), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(payload_points_array), payload_points_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(payload_points_array), sizeof(payload_colors_array), payload_colors_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(payload_points_array)+sizeof(payload_colors_array), sizeof(payload_normals_array), payload_normals_array);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor1 );
  glVertexAttribPointer( vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(payload_points_array)));

  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(payload_points_array)+sizeof(payload_colors_array)));
  //////////////////  panel1  ///////////////////////////
  glBindVertexArray (vao[1]);
  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(panel1_points_array) + sizeof(panel1_colors_array) + sizeof(panel1_normals_array), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(panel1_points_array), panel1_points_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(panel1_points_array), sizeof(panel1_colors_array), panel1_colors_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(panel1_points_array)+sizeof(panel1_colors_array), sizeof(panel1_normals_array), panel1_normals_array);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor1 );
  glVertexAttribPointer( vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(panel1_points_array)));

  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(panel1_points_array)+sizeof(panel1_colors_array)));
  //////////////////  panel2  ///////////////////////////
  glBindVertexArray (vao[2]);
  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[2]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(panel2_points_array) + sizeof(panel2_colors_array) + sizeof(panel2_normals_array), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(panel2_points_array), panel2_points_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(panel2_points_array), sizeof(panel2_colors_array), panel2_colors_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(panel2_points_array)+sizeof(panel2_colors_array), sizeof(panel2_normals_array), panel2_normals_array);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor1 );
  glVertexAttribPointer( vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(panel2_points_array)));

  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(panel2_points_array)+sizeof(panel2_colors_array)));
  //////////////////  rocket  ///////////////////////////  
  glBindVertexArray (vao[3]);
  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[3]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(rocket_points_array) + sizeof(rocket_colors_array) + sizeof(rocket_normals_array), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(rocket_points_array), rocket_points_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(rocket_points_array), sizeof(rocket_colors_array), rocket_colors_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(rocket_points_array)+sizeof(rocket_colors_array), sizeof(rocket_normals_array), rocket_normals_array);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor1 );
  glVertexAttribPointer( vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(rocket_points_array)));

  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(rocket_points_array)+sizeof(rocket_colors_array)));

  translation = glm::translate(glm::mat4(1.0f), glm::vec3(-lenPayload/2, -widPayload/2, -depPayload/2));
  // projection_matrix = glm::ortho(-100.0, 100.0, -100.0, 100.0, -100.0, 1000.0);
  projection_matrix = glm::frustum(-1.5, 1.5, -1.5, 1.5, 1.0, 2000.0);
  payload_translation = glm::translate(glm::mat4(1.0f), glm::vec3(-50, 0, 0));
  rocket_translation = glm::translate(glm::mat4(1.0f), glm::vec3(50, -50, 0));
  glUniform1f(lx, 200);
  glUniform1f(ly, 200);
  glUniform1f(lz, 200);
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f,1.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f,0.0f,1.0f));
  model_matrix = payload_translation * rotation_matrix * translation;

  //Creating the lookat and the up vectors for the camera
  c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f,0.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f,1.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f,0.0f,1.0f));

  glm::vec4 c_pos = glm::vec4(c_xpos,c_ypos,c_zpos, 1.0)*c_rotation_matrix;
  glm::vec4 c_up = glm::vec4(c_up_x,c_up_y,c_up_z, 1.0)*c_rotation_matrix;
  //Creating the lookat matrix
  lookat_matrix = glm::lookAt(glm::vec3(c_pos),glm::vec3(0.0),glm::vec3(c_up));

  //creating the projection matrix
 
  // projection_matrix = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
  view_matrix = projection_matrix*lookat_matrix;

  glUniformMatrix4fv(viewMatrix1, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniform1f(texExist1, 0.0);

  // modelview_matrix = ortho_matrix * rotation_matrix;
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[0]);
  glDrawArrays(GL_TRIANGLES, 0, payload_points.size());

  panel_translation1 = glm::translate(glm::mat4(1.0f), glm::vec3(0, (widPayload+widPanel)/2, (depPayload)/2));
  panel_translation2 = glm::translate(glm::mat4(1.0f), glm::vec3(0, -(widPayload+widPanel)/2, -(depPayload)/2));
  panel_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(panel1_xrot), glm::vec3(1.0f,0.0f,0.0f));
  panel_rotation = glm::rotate(panel_rotation, glm::radians(panel1_yrot), glm::vec3(0.0f,1.0f,0.0f));
  panel_rotation = glm::rotate(panel_rotation, glm::radians(-panel1_zrot), glm::vec3(0.0f,0.0f,1.0f));
  modelview_matrix = view_matrix * model_matrix * panel_translation1 * panel_rotation * panel_translation2;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[1]);
  glDrawArrays(GL_TRIANGLES, 0, panel1_points.size());

  panel_translation1 = glm::translate(glm::mat4(1.0f), glm::vec3(lenPayload, (widPayload+widPanel)/2, (depPayload)/2));
  panel_translation2 = glm::translate(glm::mat4(1.0f), glm::vec3(-lenPayload, -(widPayload+widPanel)/2, -(depPayload)/2));
  panel_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(panel2_xrot), glm::vec3(1.0f,0.0f,0.0f));
  panel_rotation = glm::rotate(panel_rotation, glm::radians(panel2_yrot), glm::vec3(0.0f,1.0f,0.0f));
  panel_rotation = glm::rotate(panel_rotation, glm::radians(panel2_zrot), glm::vec3(0.0f,0.0f,1.0f));
  modelview_matrix = view_matrix * model_matrix * panel_translation1 * panel_rotation * panel_translation2;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[2]);
  glDrawArrays(GL_TRIANGLES, 0, panel2_points.size());

  model_matrix = rocket_translation * rotation_matrix;
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[3]);
  for(int i=0; i<rocket_strip_indices.size(); i+=2){
    if(i==0 && rocket_strip_indices[i] == 0) {
      glDrawArrays(GL_TRIANGLE_STRIP, rocket_strip_indices[0], rocket_strip_indices[1]);
    } else if(i==0) {
      glDrawArrays(GL_TRIANGLES, 0, rocket_strip_indices[0]);
      glDrawArrays(GL_TRIANGLE_STRIP, rocket_strip_indices[0], rocket_strip_indices[1]);
    } else {
      glDrawArrays(GL_TRIANGLES, rocket_strip_indices[i-1], rocket_strip_indices[i]);
      glDrawArrays(GL_TRIANGLE_STRIP, rocket_strip_indices[i], rocket_strip_indices[i+1]);
    }
  }
  if (rocket_strip_indices.size() > 0) glDrawArrays(GL_TRIANGLES, rocket_strip_indices[rocket_strip_indices.size()-1], rocket_points.size());
  else glDrawArrays(GL_TRIANGLES, 0, rocket_points.size());
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
  window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "model2", NULL, NULL);
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


  // Load shaders and use the resulting shader program
  std::string vertex_shader_file1("./src/vshader1.glsl");
  std::string fragment_shader_file1("./src/fshader1.glsl");

  
  shaderList1.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file1));
  shaderList1.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file1));

  shaderProgram1 = csX75::CreateProgramGL(shaderList1);
  glUseProgram( shaderProgram1 );
  
  
  vPosition1 = glGetAttribLocation( shaderProgram1, "vPosition1" );
  vColor1 = glGetAttribLocation( shaderProgram1, "vColor1" ); 
  vNormal1 = glGetAttribLocation( shaderProgram1, "vNormal1" ); 
  texCoord1 = glGetAttribLocation( shaderProgram1, "texCoord1" );
  texExist1 = glGetUniformLocation( shaderProgram1, "texExist1" );
  lx = glGetUniformLocation( shaderProgram1, "lx" );
  ly = glGetUniformLocation( shaderProgram1, "ly" );
  lz = glGetUniformLocation( shaderProgram1, "lz" );
  uModelViewMatrix1 = glGetUniformLocation( shaderProgram1, "uModelViewMatrix");
  normalMatrix1 =  glGetUniformLocation( shaderProgram1, "normalMatrix");
  viewMatrix1 = glGetUniformLocation( shaderProgram1, "viewMatrix");

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
