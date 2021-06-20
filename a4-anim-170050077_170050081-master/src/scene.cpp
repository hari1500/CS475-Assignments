#include "scene.hpp"
#include "texture.hpp"

glm::mat4 payload_translation, payload_scale, rocket_translation, rotation_matrix, translation_matrix, ortho_matrix, modelview_matrix, init_rot_matrix, model_matrix_earth, model_matrix_space;
glm::mat4 model_matrix, projection_matrix, c_rotation_matrix, lookat_matrix, view_matrix;
glm::mat4 origin_translation_payload, origin_translation_base, tower_translation, protrusion_translation;
glm::mat4 panel_rotation, panel_translation1, panel_translation2;

glm::mat3 normal_matrix;

int cur_mode = 1;

GLuint shaderProgram1, uModelViewMatrix1, viewMatrix1, normalMatrix1, texExist1, lx, ly, lz, vPosition1, vColor1, vNormal1, texCoord1;
std::vector<GLuint> shaderList1;
GLuint vbo[NUM_BUFFERS], vao[NUM_BUFFERS];
GLuint tex[5];
int nVerticesPerBuffer[NUM_BUFFERS];

// Translation & Rotation Parameters
GLfloat xpos=0.0,ypos=0.0,zpos=0.0, xrot=0.0,yrot=0.0,zrot=0.0;

// Camera position and rotation Parameters
// GLfloat c_xpos = 100.0, c_ypos = 100.0, c_zpos = 100.0, c_up_x = 0.0, c_up_y = 1.0, c_up_z = 0.0, c_xrot=0.0,c_yrot=0.0,c_zrot=0.0;
glm::vec4 c_pos_ini = glm::vec4(300.0, 300.0, 300.0, 1.0), c_rot_ini = glm::vec4(0.0, 0.0, 0.0, 1.0);
// glm::vec4 c_pos_ini = glm::vec4(100.0, 100.0, 100.0, 1.0), c_rot_ini = glm::vec4(0.0, 0.0, 0.0, 1.0);
glm::vec4 c_pos = c_pos_ini, c_rot = c_rot_ini, c_up = glm::vec4(0.0, 1.0, 0.0, 1.0);

// Panel rotations wrt payload
GLfloat panel1_xrot=0.0,panel1_yrot=0.0,panel1_zrot=0.0, panel2_xrot=0.0,panel2_yrot=0.0,panel2_zrot=0.0;
glm::vec3 payload_shift;

// Protrusion shift
GLfloat t_shift=0.0, prot_shift=0.0;

csX75::HNode *root_node, *curr_node;
csX75::HNode *l1_cone, *l1_rem, *l2, *l3_center, *l3_left, *l3_right;
std::vector<glm::mat4> matrixStack;

glm::vec4 positions[8];
glm::vec4 v_positions[MAX_NUM_VERTICES], v_normals[MAX_NUM_VERTICES], v_colors[MAX_NUM_VERTICES];
glm::vec2 v_tex_coords[MAX_NUM_VERTICES];

// Animation variables
int enable_playback = 0, frame_ind = 0, start_or_stop = 0, pause_or_resume = 0; // stop -- 0, pause -- 1
double timer = 1.0/48;
std::vector<std::vector<glm::vec4>> controlPoints;
int NUMCONTROLPOINTS = 0, ROCKETSTAGE = 1;
std::vector<std::vector<float>> key_frames, all_frames;
std::vector<int> interpolate_indices;
//-----------------------------------------------------------------
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

void bindBuffer1(int tri_idx)
{
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES) + ((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES) + ((tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES), v_positions);
  glBufferSubData( GL_ARRAY_BUFFER, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES), ((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES), v_tex_coords);
  glBufferSubData( GL_ARRAY_BUFFER, ((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES)+((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES), ((tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES), v_normals);

  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( texCoord1 );
  glVertexAttribPointer( texCoord1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES)));
  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(((tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES)+((tri_idx*sizeof(v_tex_coords))/MAX_NUM_VERTICES)));
}

std::string filename;
std::fstream file;
std::string num_points, c_or_t;

void readFile0(int &tri_idx)
{
  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 0){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  for(int i=0; i<std::stoi(num_points); ++i, ++tri_idx){
    std::string a, b, c, d;
    file>>a;file>>b;file>>c;file>>d;
    v_positions[tri_idx] = (glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
    file>>a;file>>b;file>>c;file>>d;
    v_normals[tri_idx] = (glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
    file>>a;file>>b;file>>c;file>>d;
    v_colors[tri_idx] = (glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
  }
}

void readFile1(int &tri_idx)
{
  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 1){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  for(int i=0; i<std::stoi(num_points); ++i, ++tri_idx){
    std::string a, b, c, d;
    file>>a;file>>b;file>>c;file>>d;
    v_positions[tri_idx] = (glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
    file>>a;file>>b;file>>c;file>>d;
    v_normals[tri_idx] = (glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
    file>>a;file>>b;file>>c;file>>d;
    v_tex_coords[tri_idx] = (glm::vec2(std::stof(a),std::stof(b)));
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

float lenGround = 6000, widGround = 1, depGround = 6000;
float lenBase = 50*5, widBase = 1, depBase = 50*5;
float lenBuilding = 4*5, widBuilding = 20*5, depBuilding = 40*5;
float lenTower = 4*5, widTower = 100, depTower = 4*5;
float lenPad = 8*5, widPad = 2*5, depPad = 8*5;
const int numProtrusions = 4;
float protrusionLength = 3*5;
float protrusions_heights[numProtrusions] = { 12, 35, 60, 80 };
float protrusioonDimensions[3] = { 3*5, 1*5, 1*5 };

void load_model2()
{
  filename = "./data/model2.raw";
  file.open(filename.c_str());

  ////////////////////////////////  payload  /////////////////////////////////////////////////
  glBindVertexArray (vao[0]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
  int tri_idx = 0; readFile0(tri_idx); bindBuffer0(tri_idx);
  nVerticesPerBuffer[0] = tri_idx;

  glBindVertexArray (vao[1]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);
  tri_idx = 0; readFile0(tri_idx); bindBuffer0(tri_idx);
  nVerticesPerBuffer[1] = tri_idx;

  glBindVertexArray (vao[2]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[2]);
  tri_idx = 0; readFile0(tri_idx); bindBuffer0(tri_idx);
  nVerticesPerBuffer[2] = tri_idx;

  // ////////////////////////////////  rocket  /////////////////////////////////////////////////
  tri_idx = 0; readFile0(tri_idx);
  l3_center = new csX75::HNode(NULL,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);

  tri_idx = 0; readFile0(tri_idx);
  l3_left = new csX75::HNode(l3_center,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  l3_left->change_to_parameters(-(l3_center_cyl_r+l3_left_cyl_r), 0.0, 0.0, 0.0, 0.0, 0.0);

  tri_idx = 0; readFile0(tri_idx);
  l3_right = new csX75::HNode(l3_center,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  l3_right->change_to_parameters((l3_center_cyl_r+l3_right_cyl_r), 0.0, 0.0, 0.0, 0.0, 0.0);

  tri_idx = 0; readFile0(tri_idx);
  l2 = new csX75::HNode(l3_center,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  l2->change_to_parameters(0.0, l3_center_cyl_h, 0.0, 0.0, 0.0, 0.0);

  tri_idx = 0; readFile0(tri_idx);
  l1_rem = new csX75::HNode(l2,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  l1_rem->change_to_parameters(0.0, l2_cyl_h, 0.0, 0.0, 0.0, 0.0);

  tri_idx = 0; readFile0(tri_idx);
  l1_cone = new csX75::HNode(l1_rem,tri_idx,v_positions,v_colors,v_normals,(tri_idx*sizeof(v_positions))/MAX_NUM_VERTICES,(tri_idx*sizeof(v_colors))/MAX_NUM_VERTICES, (tri_idx*sizeof(v_normals))/MAX_NUM_VERTICES);
  l1_cone->change_to_parameters(0.0, l1_rem_cyl_h, 0.0, 0.0, 0.0, 0.0);

  root_node = curr_node = l3_center;

  file.close();
}

void load_model1()
{
  filename = "./data/model1.raw";
  file.open(filename.c_str());

  ////////////////////////////////  base + building + pad-for-rocket-launching  ///////////////////////////
  glBindVertexArray (vao[3]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[3]);
  int tri_idx = 0; readFile0(tri_idx); bindBuffer0(tri_idx);
  nVerticesPerBuffer[3] = tri_idx;

  /////////////////////////////////////////  base-tower  ///////////////////////////////////////////////////
  glBindVertexArray (vao[4]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[4]);
  tri_idx = 0; readFile0(tri_idx); bindBuffer0(tri_idx);
  nVerticesPerBuffer[4] = tri_idx;

  /////////////////////////////////////////  protrusions  ///////////////////////////////////////////////////
  glBindVertexArray (vao[5]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[5]);
  tri_idx = 0; readFile0(tri_idx); bindBuffer0(tri_idx);
  nVerticesPerBuffer[5] = tri_idx;

  /////////////////////////////////////////  ground  ///////////////////////////////////////////////////
  glBindVertexArray (vao[6]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[6]);
  tri_idx = 0; readFile1(tri_idx); bindBuffer1(tri_idx);
  nVerticesPerBuffer[6] = tri_idx;

  /////////////////////////////////////////  outer  ///////////////////////////////////////////////////
  glBindVertexArray (vao[7]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[7]);
  tri_idx = 0; readFile1(tri_idx); bindBuffer1(tri_idx);
  nVerticesPerBuffer[7] = tri_idx;

  file.close();
}

void load_model0()
{
  filename = "./data/model0.raw";
  file.open(filename.c_str());

  ////////////////////////////////  base + building + pad-for-rocket-launching  ///////////////////////////
  glBindVertexArray (vao[8]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[8]);
  int tri_idx = 0; readFile1(tri_idx); bindBuffer1(tri_idx);
  nVerticesPerBuffer[8] = tri_idx;

  glBindVertexArray (vao[9]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[9]);
  tri_idx = 0; readFile1(tri_idx); bindBuffer1(tri_idx);
  nVerticesPerBuffer[9] = tri_idx;

  glBindVertexArray (vao[10]);
  glBindBuffer (GL_ARRAY_BUFFER, vbo[10]);
  tri_idx = 0; readFile1(tri_idx); bindBuffer1(tri_idx);
  nVerticesPerBuffer[10] = tri_idx;

  file.close();
}

void load_key_frames()
{
  controlPoints.clear();
  NUMCONTROLPOINTS = 0;
  std::string filename = "./data/trajectory1.txt";  

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

  filename = "./data/keyframes.txt";
  file.open(filename.c_str());
  while(true){
    std::string rocket_stage;
    std::vector<float> frame;
    if(!(file>>rocket_stage)) break;
    frame.push_back(std::stof(rocket_stage));

    std::string active, param;
    
    file>>active;file>>param;
    interpolate_indices.push_back(2);
    frame.push_back(std::stof(active));
    frame.push_back(std::stof(param));
    
    file>>active;file>>param;
    interpolate_indices.push_back(4);
    frame.push_back(std::stof(active));
    frame.push_back(std::stof(param));

    file>>active;file>>param;
    interpolate_indices.push_back(6);
    frame.push_back(std::stof(active));
    frame.push_back(std::stof(param));

    key_frames.push_back(frame);
  }
}

void interpolate_bw_2_frames(int ind)
{
  if (key_frames[ind][0] != key_frames[ind+1][0]){
    all_frames.push_back(key_frames[ind]);
    return;  
  }
  all_frames.push_back(key_frames[ind]);
  for(int i = 0; i<24; ++i){
    std::vector<float> frame = key_frames[ind];
    for(int j=0; j<interpolate_indices.size(); ++j){
      int k = interpolate_indices[j];
      frame[k] = ((24-i)*key_frames[ind][k] + i*key_frames[ind+1][k])/24.0;
    }
    // for(int j=0; j<key_frames[ind].size();++j){
    //   frame[j] = ((24-i)*key_frames[ind][j] + i*key_frames[ind+1][j])/24;
    // }
    all_frames.push_back(frame);
  }
}

// glm::vec4 get_point_bezier(int ind, float t)
// {
//   std::vector<glm::vec4> lastcP = controlPoints[ind];
//   int i = lastcP.size()-1;
//   while(i>0){
//     for(int k=0; k<i; ++k) lastcP[k] += t*(lastcP[k+1]-lastcP[k]);
//     i--;
//   }
//   return lastcP[0];
// }

glm::vec4 get_point_bezier(int ind, float t)
{
  std::vector<glm::vec4> lastcP = controlPoints[ind];
  glm::vec4 P_t = glm::vec4(0.0);
  int s = lastcP.size();
  for(int i=0;i<s;++i){
    float j = nCr(s-1,i)*pow(t, i)*pow(1-t, s-1-i);
    P_t += (j*lastcP[i]);
  }
  return P_t;
}

void load_frame(int ind)
{
  std::vector<float> frame = all_frames[ind];

  if(frame[0] != ROCKETSTAGE){
    if(ROCKETSTAGE==1 && frame[0]==2.0){
      l3_left->del_parent();
      l3_right->del_parent();
      ROCKETSTAGE = 2;
    } else if(ROCKETSTAGE==2 && frame[0]==3.0){
      root_node = l1_rem;
      l1_rem->del_parent();
      cur_mode = 2;
      ROCKETSTAGE = 3;
    } else if(ROCKETSTAGE==3 && frame[0]==4.0){
      l1_rem->del_parent();
      l1_cone->del_parent();
      root_node = NULL;
      cur_mode = 2;
      ROCKETSTAGE = 4;
    }
  }

  
  if(ROCKETSTAGE == 1){
    if(frame[1] >= 1){
      glm::vec4 new_pos = get_point_bezier(0, frame[2]);  // trajectory 0 for raocket 
      glm::vec4 old_pos = root_node->get_trans_params();
      glm::vec4 rot = root_node->get_rot_params();
      root_node->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]);
      payload_shift += glm::vec3(new_pos-old_pos);
      csX75::update_cam();
    }

    //////////////////// no need ////////////////////
    // if(frame[3] >= 1){
    //   glm::vec4 new_pos = get_point_bezier(1, frame[4]);  // trajectory 1 for l3_left(left booster)
    //   glm::vec4 old_pos = l3_left->get_trans_params();
    //   glm::vec4 rot = l3_left->get_rot_params();
    //   l3_left->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]);
    // }

    // if(frame[5] >= 1){
    //   glm::vec4 new_pos = get_point_bezier(2, frame[6]); // trajectory 2 for l3_right(right booster)
    //   glm::vec4 old_pos = l3_right->get_trans_params();
    //   glm::vec4 rot = l3_right->get_rot_params();
    //   l3_right->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]);
    // }
  }
  else if(ROCKETSTAGE == 2){    // boosters separate in this stage
    if(frame[1] > 0){
      glm::vec4 new_pos = get_point_bezier(1, frame[2]);  // trajectory 1 for raocket 
      glm::vec4 old_pos = root_node->get_trans_params();
      glm::vec4 rot = root_node->get_rot_params();
      root_node->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]);
      payload_shift += glm::vec3(new_pos-old_pos);
      csX75::update_cam();
    }

    if(frame[3] > 0){
      glm::vec4 new_pos = get_point_bezier(2, frame[4]);  // trajectory 2 for l3_left(left booster)
      glm::vec4 old_pos = l3_left->get_trans_params();
      // glm::vec4 rot = l3_left->get_rot_params();
      // float l = std::sqrt(old_pos[0]*old_pos[0] + old_pos[1]*old_pos[1] + old_pos[2]*old_pos[2]);
      // rot += (new_pos - old_pos)/l;
      glm::vec4 rot = glm::normalize(new_pos - old_pos);
      l3_left->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]);
    }

    if(frame[5] > 0){
      glm::vec4 new_pos = get_point_bezier(3, frame[6]); // trajectory 3 for l3_right(right booster)
      glm::vec4 old_pos = l3_right->get_trans_params();
      glm::vec4 rot = l3_right->get_rot_params();
      l3_right->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]);
    }
  }
  else if(ROCKETSTAGE == 3){
    if(frame[1] > 0){
      glm::vec4 new_pos = get_point_bezier(4, frame[2]);  // trajectory 4 for root node
      glm::vec4 old_pos = root_node->get_trans_params();
      glm::vec4 rot = root_node->get_rot_params();
      root_node->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]);
      payload_shift += glm::vec3(new_pos-old_pos);
      csX75::update_cam();
    }

    if(frame[3] > 0){
      glm::vec4 new_pos = get_point_bezier(4, frame[4]);  // trajectory 5 for lowest cylinder()
      glm::vec4 old_pos = l3_center->get_trans_params();
      glm::vec4 rot = l3_center->get_rot_params();
      l3_center->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]); /////////// change var
    }

    /////////////////////////////////////////// no neeed ////////////////////////
    // if(frame[5] > 0){
    //   glm::vec4 new_pos = get_point_bezier(2, frame[6]); // trajectory 2 for l3_right(right booster)
    //   glm::vec4 old_pos = l3_right->get_trans_params();
    //   glm::vec4 rot = l3_right->get_rot_params();
    //   l3_right->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]);
    // }
  }
  else if(ROCKETSTAGE == 4){
    if(frame[1] > 0){
      payload_shift = glm::vec3(get_point_bezier(0, frame[2]));
      csX75::update_cam();
    }

    if(frame[3] > 0){
      glm::vec4 new_pos = get_point_bezier(1, frame[4]);  // trajectory 1 for l1_rem
      glm::vec4 old_pos = l1_rem->get_trans_params();
      glm::vec4 rot = l1_rem->get_rot_params();
      l1_rem->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]);
    }

    if(frame[5] > 0){
      glm::vec4 new_pos = get_point_bezier(2, frame[6]); // trajectory 2 for l1_cone
      glm::vec4 old_pos = l1_cone->get_trans_params();
      glm::vec4 rot = l1_cone->get_rot_params();
      l1_cone->change_to_parameters(new_pos[0], new_pos[1], new_pos[2], rot[0], rot[1], rot[2]);
    }
  }
}

void save_frame(int ind)
{
  unsigned char *pRGB = new unsigned char [3*(WIN_WIDTH+1)*(WIN_HEIGHT+1)];
  glReadBuffer(GL_BACK);
  glPixelStoref(GL_PACK_ALIGNMENT,1);//for word allignment
  glReadPixels(0, 0, WIN_WIDTH, WIN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pRGB);

  for (int j = 0; 2*j < WIN_HEIGHT; ++j) {
    for (int i = WIN_WIDTH*3, x = j*WIN_WIDTH*3, y = (WIN_HEIGHT-1-j)*WIN_WIDTH*3; i > 0; --i, ++x, ++y) {
      std::swap(pRGB[x], pRGB[y]);
    }
  }

  char filename[200];
  sprintf(filename,"./data/frames/frame_%05d.ppm",ind);
  std::ofstream out(filename, std::ios::out);
  out<<"P6"<<std::endl;
  out<<WIN_WIDTH<<" "<<WIN_HEIGHT<<" 255"<<std::endl;
  out.write(reinterpret_cast<char const *>(pRGB), (3*(WIN_WIDTH+1)*(WIN_HEIGHT+1))*sizeof(int));
  out.close();

  delete pRGB;
}

void initBuffersGL(void)
{
  // Load Textures 
  tex[0]=LoadTexture("data/ground-texture.bmp",3072,2304);
  tex[1]=LoadTexture("data/blue-sky.bmp",3888,2592);
  tex[2]=LoadTexture("data/earth.bmp",5400,2700);
  tex[3]=LoadTexturePNG("data/cloud.png");
  tex[4]=LoadTexture("data/space.bmp",1920,1200);

  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (NUM_BUFFERS, vao);
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (NUM_BUFFERS, vbo);

  load_model0();
  load_model1();
  load_model2();
  load_key_frames();
  for(int i=0; i<key_frames.size()-1;++i) interpolate_bw_2_frames(i);
  all_frames.push_back(key_frames[key_frames.size()-1]);

  payload_shift = glm::vec3(4, 106+widPad, 4);
  origin_translation_payload  = glm::translate(glm::mat4(1.0f), glm::vec3(-lenPayload/2, -widPayload/2, -depPayload/2));
  origin_translation_base = glm::translate(glm::mat4(1.0f), glm::vec3(-lenBase/2, 0.0, -depBase/2));
  projection_matrix = glm::frustum(-1.5, 1.5, -1.5, 1.5, 1.0, 6000.0);
  payload_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
  rocket_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0, widPad+4, 0));
  
  xrot = -90.0;
  init_rot_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  xrot = 0;
}

void renderGL1(void)
{
  model_matrix = rotation_matrix * origin_translation_base;
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[3]);  
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[3]);

  tower_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, t_shift));
  modelview_matrix = view_matrix * model_matrix * tower_translation;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[4]);  
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[4]);

  protrusion_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, prot_shift));
  modelview_matrix = view_matrix * model_matrix * tower_translation * protrusion_translation;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[5]);
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[5]);

  glUniform1f(texExist1, 1.0);
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[0]);
  glBindVertexArray(vao[6]);
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[6]);

  modelview_matrix = view_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[1]);
  glBindVertexArray(vao[7]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, nVerticesPerBuffer[7]);
}

void renderGL2(void)
{
  model_matrix_space = glm::mat4(1.0f);
  
  rotation_matrix = glm::rotate(init_rot_matrix, glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f,1.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f,0.0f,1.0f));
  model_matrix_earth = rotation_matrix;

  //Creating the lookat and the up vectors for the camera
  c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_rot[0]), glm::vec3(1.0f,0.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_rot[1]), glm::vec3(0.0f,1.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_rot[2]), glm::vec3(0.0f,0.0f,1.0f));

  glm::vec4 c_position = c_pos*c_rotation_matrix;
  glm::vec4 c_up_vec = c_up*c_rotation_matrix;
  //Creating the lookat matrix
  lookat_matrix = glm::lookAt(glm::vec3(c_pos),glm::vec3(0.0),glm::vec3(c_up));

  //creating the projection matrix
  projection_matrix = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 8000.0);

  view_matrix = projection_matrix*lookat_matrix;
  glUniformMatrix4fv(viewMatrix1, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniform1f(texExist1, 1.0);

  // Draw the sphere
  modelview_matrix = view_matrix*model_matrix_earth;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[2]);
  glBindVertexArray (vao[8]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, nVerticesPerBuffer[8]);

  // Draw the clouds
  modelview_matrix = view_matrix*model_matrix_earth;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[3]);
  glBindVertexArray (vao[9]);
  // glDrawArrays(GL_TRIANGLE_STRIP, 0, nVerticesPerBuffer[9]);

  modelview_matrix = view_matrix*model_matrix_space;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[4]);
  glBindVertexArray (vao[10]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, nVerticesPerBuffer[10]);
}

void renderGL(void)
{
  if(ROCKETSTAGE >= 3) {
    glUniform1f(lx, 1000);
    glUniform1f(ly, 1000);
    glUniform1f(lz, 1000);
  } else {
    glUniform1f(lx, 200);
    glUniform1f(ly, 200);
    glUniform1f(lz, 0);
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f,1.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f,0.0f,1.0f));
  payload_translation = glm::translate(glm::mat4(1.0f), payload_shift);
  model_matrix = payload_translation * rotation_matrix * origin_translation_payload * payload_scale;

  //Creating the lookat and the up vectors for the camera
  c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_rot[0]), glm::vec3(1.0f,0.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_rot[1]), glm::vec3(0.0f,1.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_rot[2]), glm::vec3(0.0f,0.0f,1.0f));


  glm::vec4 c_position; //  = c_pos*c_rotation_matrix;
  glm::vec4 c_up_vec;  // = c_up*c_rotation_matrix;
  
  if(cur_mode == 1){
    c_position = c_pos*c_rotation_matrix;
    c_up_vec = c_up*c_rotation_matrix;
  }
  else{
    glm::mat4 Mat_1 = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f,0.0f,0.0f));
    Mat_1 = glm::rotate(Mat_1, glm::radians(0.0f), glm::vec3(0.0f,1.0f,0.0f));
    Mat_1 = glm::rotate(Mat_1, glm::radians(0.0f), glm::vec3(0.0f,0.0f,1.0f));
    c_position = c_pos*c_rotation_matrix;
    c_up_vec = c_up*c_rotation_matrix*Mat_1;
  }
  //Creating the lookat matrix
  lookat_matrix = glm::lookAt(glm::vec3(c_position),glm::vec3(0.0),glm::vec3(c_up_vec));

  //creating the projection matrix
  // projection_matrix = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
  view_matrix = projection_matrix*lookat_matrix;

  glUniformMatrix4fv(viewMatrix1, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniform1f(texExist1, 0.0);

  matrixStack.clear();
  
  // modelview_matrix = ortho_matrix * rotation_matrix;
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[0]);
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[0]);

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
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[1]);

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
  glDrawArrays(GL_TRIANGLES, 0, nVerticesPerBuffer[2]);

  matrixStack.push_back(view_matrix*rocket_translation);
  root_node->render_tree();
  // if(ROCKETSTAGE==1){
  //   root_node->render_tree();
  // }
  if(ROCKETSTAGE==2){
    l3_left->render_tree();
    l3_right->render_tree();
  } else if(ROCKETSTAGE==3){
    // l3_left->render_tree();
    // l3_right->render_tree();
    l3_center->render_tree();
  } else if(ROCKETSTAGE==4){
    // l3_left->render_tree();
    // l3_right->render_tree();
    // l3_center->render_tree();
    l1_cone->render_tree();
    l1_rem->render_tree();
  }

  if(ROCKETSTAGE >= 3) renderGL2();
  else renderGL1();
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
  window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "scene", NULL, NULL);
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
    if (enable_playback && glfwGetTime() > frame_ind*timer){
      load_frame(frame_ind);
      // Uncomment below line to save frames
      // save_frame(frame_ind);
      frame_ind++;
      if(frame_ind == all_frames.size()) {
        enable_playback = 0;
        frame_ind = 0;
      }
    }
    // if (start_or_stop && !pause_or_resume && glfwGetTime() > frame_ind*timer){

    // }
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
