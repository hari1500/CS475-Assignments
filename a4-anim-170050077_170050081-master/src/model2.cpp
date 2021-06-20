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

csX75::HNode *root_node, *curr_node;
csX75::HNode *l1_cone, *l1_rem, *l2, *l3_center, *l3_left, *l3_right;
std::vector<glm::mat4> matrixStack;

//Running variable to toggle culling on/off
bool enable_culling=true;
//Running variable to toggle wireframe/solid modelling
bool solid=true;
//Enable/Disable perspective view
bool enable_perspective=false;
//-----------------------------------------------------------------

//points in homogenous coordinates
glm::vec4 positions[8];

glm::vec4 v_positions[MAX_NUM_VERTICES], v_normals[MAX_NUM_VERTICES], v_colors[MAX_NUM_VERTICES];
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

void cuboid(glm::vec4 offset, glm::vec4 col, float l, float b, float d, int &tri_ind)
{
  assignPositions(l, b, d);
  std::vector<std::vector<int>> quads {
    { 1, 0, 3, 2 },
    { 2, 3, 7, 6 },
    { 3, 0, 4, 7 },
    { 6, 5, 1, 2 },
    { 4, 5, 6, 7 },
    { 5, 4, 0, 1 },
  };
  std::vector<glm::vec4> normals {
    glm::vec4(0.0, 0.0, 1.0, 1.0),
    glm::vec4(1.0, 0.0, 0.0, 1.0),
    glm::vec4(0.0, -1.0, 0.0, 1.0),
    glm::vec4(0.0, 1.0, 0.0, 1.0),
    glm::vec4(0.0, 0.0, -1.0, 1.0),
    glm::vec4(-1.0, 0.0, 0.0, 1.0)
  };
  for(int i=0; i<quads.size(); ++i) {
    int a = quads[i][0], b = quads[i][1], c = quads[i][2], d = quads[i][3];
    v_positions[tri_ind] = (positions[a]+offset); v_colors[tri_ind] = col; v_normals[tri_ind] = normals[i]; tri_ind++;
    v_positions[tri_ind] = (positions[b]+offset); v_colors[tri_ind] = col; v_normals[tri_ind] = normals[i]; tri_ind++;
    v_positions[tri_ind] = (positions[c]+offset); v_colors[tri_ind] = col; v_normals[tri_ind] = normals[i]; tri_ind++;
    v_positions[tri_ind] = (positions[a]+offset); v_colors[tri_ind] = col; v_normals[tri_ind] = normals[i]; tri_ind++;
    v_positions[tri_ind] = (positions[c]+offset); v_colors[tri_ind] = col; v_normals[tri_ind] = normals[i]; tri_ind++;
    v_positions[tri_ind] = (positions[d]+offset); v_colors[tri_ind] = col; v_normals[tri_ind] = normals[i]; tri_ind++;
  }
}

void cylinder(double radius, double height, glm::vec4 offset, int &tri_idx, int slices = 2)
{
  for (float theta = 0; theta <= 360; theta += slices)
  {
    float theta2 = theta+slices>=360 ? 360 : theta+slices;
    float theta_rads1 = glm::radians(theta), theta_rads2 = glm::radians(theta2);
    glm::vec4 a = glm::vec4(radius * cos(theta_rads1), height, radius * sin(theta_rads1), 1);
    glm::vec4 b = glm::vec4(radius * cos(theta_rads1), 0, radius * sin(theta_rads1), 1);
    glm::vec4 c = glm::vec4(radius * cos(theta_rads2), height, radius * sin(theta_rads2), 1);
    glm::vec4 d = glm::vec4(radius * cos(theta_rads2), 0, radius * sin(theta_rads2), 1);

    // Lateral Surface
    v_colors[tri_idx] = (glm::vec4(1.0));v_positions[tri_idx] = (offset + a); 
    v_normals[tri_idx] = (b);tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0));v_positions[tri_idx] = (offset + b);
    v_normals[tri_idx] = (b);tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0));v_positions[tri_idx] = (offset + c);
    v_normals[tri_idx] = (b);tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0));v_positions[tri_idx] = (offset + b); 
    v_normals[tri_idx] = (d);tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0));v_positions[tri_idx] = (offset + c);
    v_normals[tri_idx] = (d);tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0));v_positions[tri_idx] = (offset + d);
    v_normals[tri_idx] = (d);tri_idx++;

    // Bottom and upper faces
    v_colors[tri_idx] = (glm::vec4(1.0)); v_normals[tri_idx] = (glm::vec4(0, -1.0, 0, 1));
    v_positions[tri_idx] = (offset + glm::vec4(0, 0.0, 0, 1));  tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0)); v_normals[tri_idx] = (glm::vec4(0, -1.0, 0, 1));
    v_positions[tri_idx] = (offset + glm::vec4(radius * cos(theta_rads1), 0, radius * sin(theta_rads1), 1)); tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0)); v_normals[tri_idx] = (glm::vec4(0, -1.0, 0, 1));
    v_positions[tri_idx] = (offset + glm::vec4(radius * cos(theta_rads2), 0, radius * sin(theta_rads2), 1)); tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0)); v_normals[tri_idx] = (glm::vec4(0, 1.0, 0, 1));
    v_positions[tri_idx] = (offset + glm::vec4(0, height, 0, 1));  tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0)); v_normals[tri_idx] = (glm::vec4(0, 1.0, 0, 1));
    v_positions[tri_idx] = (offset + glm::vec4(radius * cos(theta_rads1), height, radius * sin(theta_rads1), 1)); tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0)); v_normals[tri_idx] = (glm::vec4(0, 1.0, 0, 1));
    v_positions[tri_idx] = (offset + glm::vec4(radius * cos(theta_rads2), height, radius * sin(theta_rads2), 1)); tri_idx++;
  }
}

void cone(double radius, double height, glm::vec4 offset, int &tri_idx, int slices = 2)
{
  for (float theta = 0; theta <= 360; theta += slices)
  {
    float theta2 = theta+slices>=360 ? 360 : theta+slices;
    float theta_rads1 = glm::radians(theta), theta_rads2 = glm::radians(theta2);
    glm::vec4 a = glm::vec4(0, height, 0, 1);
    glm::vec4 b = glm::vec4(radius * cos(theta_rads1), 0, radius * sin(theta_rads1), 1);
    glm::vec4 c = glm::vec4(radius * cos(theta_rads2), 0, radius * sin(theta_rads2), 1);

    glm::vec4 x = b-a, y = c-a;
    glm::vec4 normal = -glm::vec4((x[1]*y[2] - x[2]*y[1]), -(x[0]*y[2] - x[2]*y[0]), (x[0]*y[1] - x[1]*y[0]), 1);

    // Lateral surface
    v_colors[tri_idx] = (glm::vec4(1.0)); v_positions[tri_idx] = (offset + a); 
    v_normals[tri_idx] = (normal); tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0)); v_positions[tri_idx] = (offset + b);
    v_normals[tri_idx] = (normal); tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0)); v_positions[tri_idx] = (offset + c);
    v_normals[tri_idx] = (normal); tri_idx++;

    // Base
    v_colors[tri_idx] = (glm::vec4(1.0)); v_normals[tri_idx] = (glm::vec4(0, -1.0, 0, 1));
    v_positions[tri_idx] = (offset + glm::vec4(0, 0.0, 0, 1));  tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0)); v_normals[tri_idx] = (glm::vec4(0, -1.0, 0, 1));
    v_positions[tri_idx] = (offset + glm::vec4(radius * cos(theta_rads1), 0, radius * sin(theta_rads1), 1)); tri_idx++;

    v_colors[tri_idx] = (glm::vec4(1.0)); v_normals[tri_idx] = (glm::vec4(0, -1.0, 0, 1));
    v_positions[tri_idx] = (offset + glm::vec4(radius * cos(theta_rads2), 0, radius * sin(theta_rads2), 1)); tri_idx++;
  }
}

//-----------------------------------------------------------------
void bindBuffer0(int tri_idx)
{
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, (tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES + (tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES + (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES, NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, (tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES, v_positions);
  glBufferSubData( GL_ARRAY_BUFFER, (tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, v_colors);
  glBufferSubData( GL_ARRAY_BUFFER, (tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES+(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES, v_normals);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( vColor1 );
  glVertexAttribPointer( vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES));
  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES+(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES));
}
std::vector<std::array<float, 12>> mymodel;
int oldSize = 0;
std::string filename = "./data/model2.raw";
std::ofstream output_file(filename);
std::ostream_iterator<float> output_iter(output_file, " ");

void saveIntoFile0(int num_points)
{
  oldSize = mymodel.size();
  for ( int i = 0; i < num_points; ++i ) {
    mymodel.push_back({v_positions[i][0], v_positions[i][1], v_positions[i][2], v_positions[i][3], v_normals[i][0], v_normals[i][1], v_normals[i][2], v_normals[i][3], v_colors[i][0], v_colors[i][1], v_colors[i][2], v_colors[i][2]});
  }

  output_file << num_points;
  output_file << " ";
  output_file << "0\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }
}

float lenPayload = 20, widPayload = 20, depPayload = 20;
float lenPanel = 2, widPanel = 15, depPanel = 15;

float l1_cone_r = 10, l1_cone_h = 20;
float l1_rem_cyl_r = 10, l1_rem_cyl_h = 30, l1_rem_thr_r = 8, l1_rem_thr_h = 25;

float l2_cyl_r = 10, l2_cyl_h = 30, l2_thr_r = 8, l2_thr_h = 25;

float l3_center_cyl_r = 10, l3_center_cyl_h = 40, l3_center_thr_r = 3.5, l3_center_thr_h = 20;
float l3_left_cyl_r = 5, l3_left_cyl_h = 40, l3_left_cone_r = l3_left_cyl_r, l3_left_cone_h = 6;
float l3_right_cyl_r = 5, l3_right_cyl_h = 40, l3_right_cone_r = l3_right_cyl_r, l3_right_cone_h = 6;

void initBuffersGL(void)
{

  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (NUM_BUFFERS, vao);
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (NUM_BUFFERS, vbo);

  ////////////////////////////////  payload  /////////////////////////////////////////////////
  glBindVertexArray (vao[0]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
  int tri_idx = 0;
  cuboid(glm::vec4(0.0, 0.0, 0.0, 0.0), glm::vec4(1.0, 1.0, 0.0, 1.0), lenPayload, widPayload, depPayload, tri_idx);
  bindBuffer0(tri_idx); saveIntoFile0(tri_idx);

  glBindVertexArray (vao[1]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);
  tri_idx = 0;
  cuboid(glm::vec4(-lenPanel, (widPayload-widPanel)/2, (depPayload-depPanel)/2, 0.0), LIGHT_GREY, lenPanel, widPanel, depPanel, tri_idx);
  bindBuffer0(tri_idx); saveIntoFile0(tri_idx);

  glBindVertexArray (vao[2]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[2]);
  tri_idx = 0;
  cuboid(glm::vec4(lenPayload, (widPayload-widPanel)/2, (depPayload-depPanel)/2, 0.0), LIGHT_GREY, lenPanel, widPanel, depPanel, tri_idx);
  bindBuffer0(tri_idx); saveIntoFile0(tri_idx);

  // ////////////////////////////////  rocket  /////////////////////////////////////////////////
  tri_idx = 0;
  cylinder(l3_center_cyl_r, l3_center_cyl_h, glm::vec4(0.0, 0.0, 0.0, 0.0), tri_idx);
  cone(l3_center_thr_r, l3_center_thr_h, glm::vec4(4.0, -4.0, 4.0, 0.0), tri_idx);
  cone(l3_center_thr_r, l3_center_thr_h, glm::vec4(-4.0, -4.0, 4.0, 0.0), tri_idx);
  cone(l3_center_thr_r, l3_center_thr_h, glm::vec4(-4.0, -4.0, -4.0, 0.0), tri_idx);
  cone(l3_center_thr_r, l3_center_thr_h, glm::vec4(4.0, -4.0, -4.0, 0.0), tri_idx);
  l3_center = new csX75::HNode(NULL,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  saveIntoFile0(tri_idx);

  tri_idx = 0;
  cylinder(l3_left_cyl_r, l3_left_cyl_h, glm::vec4(0.0, 0.0, 0.0, 0.0), tri_idx);
  cone(l3_left_cone_r, l3_left_cone_h, glm::vec4(0.0, l3_left_cyl_h, 0.0, 0.0), tri_idx);
  l3_left = new csX75::HNode(l3_center,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  l3_left->change_to_parameters(-(l3_center_cyl_r+l3_left_cyl_r), 0.0, 0.0, 0.0, 0.0, 0.0);
  saveIntoFile0(tri_idx);

  tri_idx = 0;
  cylinder(l3_right_cyl_r, l3_right_cyl_h, glm::vec4(0.0, 0.0, 0.0, 0.0), tri_idx);
  cone(l3_right_cone_r, l3_right_cone_h, glm::vec4(0.0, l3_right_cyl_h, 0.0, 0.0), tri_idx);
  l3_right = new csX75::HNode(l3_center,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  l3_right->change_to_parameters((l3_center_cyl_r+l3_right_cyl_r), 0.0, 0.0, 0.0, 0.0, 0.0);
  saveIntoFile0(tri_idx);

  tri_idx = 0;
  cylinder(l2_cyl_r, l2_cyl_h, glm::vec4(0.0, 0.0, 0.0, 0.0), tri_idx);
  cone(l2_thr_r, l2_thr_h, glm::vec4(0.0, -4.0, 0.0, 0.0), tri_idx);
  l2 = new csX75::HNode(l3_center,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  l2->change_to_parameters(0.0, l3_center_cyl_h, 0.0, 0.0, 0.0, 0.0);
  saveIntoFile0(tri_idx);

  tri_idx = 0;
  cylinder(l1_rem_cyl_r, l1_rem_cyl_h, glm::vec4(0.0, 0.0, 0.0, 0.0), tri_idx);
  cone(l1_rem_thr_r, l1_rem_thr_h, glm::vec4(0.0, -4.0, 0.0, 0.0), tri_idx);
  l1_rem = new csX75::HNode(l2,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  l1_rem->change_to_parameters(0.0, l2_cyl_h, 0.0, 0.0, 0.0, 0.0);
  saveIntoFile0(tri_idx);

  tri_idx = 0;
  cone(l1_cone_r, l1_cone_h, glm::vec4(0.0, 0.0, 0.0, 0.0), tri_idx);
  l1_cone = new csX75::HNode(l1_rem,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  l1_cone->change_to_parameters(0.0, l1_rem_cyl_h, 0.0, 0.0, 0.0, 0.0);
  saveIntoFile0(tri_idx);  

  root_node = curr_node = l3_center;

  translation = glm::translate(glm::mat4(1.0f), glm::vec3(-lenPayload/2, -widPayload/2, -depPayload/2));
  // projection_matrix = glm::ortho(-100.0, 100.0, -100.0, 100.0, -100.0, 1000.0);
  projection_matrix = glm::frustum(-1.5, 1.5, -1.5, 1.5, 1.0, 2000.0);
  payload_translation = glm::translate(glm::mat4(1.0f), glm::vec3(-50, 0, 0));
  rocket_translation = glm::translate(glm::mat4(1.0f), glm::vec3(50, 0, 0));
  glUniform1f(lx, 200);
  glUniform1f(ly, 200);
  glUniform1f(lz, 200);
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  matrixStack.clear();

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
  glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS_PER_CUBE);

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
  glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS_PER_CUBE);

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
  glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS_PER_CUBE);

  matrixStack.push_back(view_matrix);
  root_node->render_tree();
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
