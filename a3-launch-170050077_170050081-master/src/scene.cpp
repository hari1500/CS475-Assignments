#include "scene.hpp"
#include "texture.hpp"

GLuint shaderProgram1;
glm::mat4 payload_translation;
glm::mat4 payload_scale;
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
glm::mat4 origin_translation_payload, origin_translation_base;

glm::mat4 tower_translation;
glm::mat4 protrusion_translation;

glm::mat4 panel_rotation, panel_translation1, panel_translation2;

GLuint uModelViewMatrix1, viewMatrix1, normalMatrix1, texExist1, lx, ly, lz;

std::vector<GLuint> shaderList1;

//-----------------------------------------------------------------
GLuint vPosition1, vColor1, vNormal1, texCoord1;
// vaos and vbos
GLuint vbo[NUM_BUFFERS], vao[NUM_BUFFERS];
GLuint tex[2];

// Translation Parameters
GLfloat xpos=0.0,ypos=0.0,zpos=0.0;
// Rotation Parameters
GLfloat xrot=0.0,yrot=0.0,zrot=0.0;
// Camera position and rotation Parameters
GLfloat c_xpos = 50.0, c_ypos = 100.0, c_zpos = 300.0;
GLfloat c_up_x = 0.0, c_up_y = 1.0, c_up_z = 0.0;
GLfloat c_xrot=0.0,c_yrot=0.0,c_zrot=0.0;
// Panel rotations wrt payload
GLfloat panel1_xrot=0.0,panel1_yrot=0.0,panel1_zrot=0.0;
GLfloat panel2_xrot=0.0,panel2_yrot=0.0,panel2_zrot=0.0;
// Tower model shift
GLfloat t_shift=0.0, prot_shift=0.0;
//Running variable to toggle culling on/off
bool enable_culling=true;
//Running variable to toggle wireframe/solid modelling
bool solid=true;
//Enable/Disable perspective view
bool enable_perspective=false;
//-----------------------------------------------------------------

std::vector<glm::vec4> payload_points, panel1_points, panel2_points, rocket_points;
std::vector<glm::vec4> payload_colors, panel1_colors, panel2_colors, rocket_colors;
std::vector<glm::vec4> payload_normals, panel1_normals, panel2_normals, rocket_normals;
std::vector<int> rocket_strip_indices;
glm::vec4 LIGHT_GREY = glm::vec4(0.658824, 0.658824, 0.658824, 1.0);

const int num_vertices = 20000;
glm::vec4 white(0.2, 0.7, 0.7, 1.0);

std::vector<glm::vec4> model_points;
std::vector<glm::vec4> model_colors;
std::vector<glm::vec4> model_normals; 
std::vector<glm::vec4> tower_points;
std::vector<glm::vec4> tower_colors;
std::vector<glm::vec4> tower_normals;
std::vector<glm::vec4> protrusions_points;
std::vector<glm::vec4> protrusions_colors;
std::vector<glm::vec4> protrusions_normals;
std::vector<glm::vec4> ground_points;
std::vector<glm::vec2> ground_textures;
std::vector<glm::vec4> ground_normals;
glm::vec4 outer_positions[num_vertices];
glm::vec4 outer_normals[num_vertices];
glm::vec2 outer_tex_coords[num_vertices];
float earthRadius = 1000;

//-----------------------------------------------------------------

float lenPayload = 20, widPayload = 20, depPayload = 20;
float lenPanel = 2, widPanel = 15, depPanel = 15;
float lenGround = 2000, widGround = 1, depGround = 2000;
float lenBase = 50*5, widBase = 1, depBase = 50*5;
float lenBuilding = 4*5, widBuilding = 20*5, depBuilding = 40*5;
float lenTower = 4*5, widTower = 100, depTower = 4*5;
float lenPad = 8*5, widPad = 2*5, depPad = 8*5;
const int numProtrusions = 4;
float protrusionLength = 3*5;
float protrusions_heights[numProtrusions] = { 12, 35, 60, 80 };
float protrusioonDimensions[3] = { 3*5, 1*5, 1*5 };

void initBuffersGL(void)
{

  std::string filename = "./data/model2_payload.raw";
  std::fstream file;
  file.open(filename.c_str());
  std::string num_points, c_or_t;

  payload_points.clear();
  payload_colors.clear();
  payload_normals.clear();

  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 0){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  for(int i=0; i<std::stoi(num_points); ++i){
      std::string a, b, c, d;
      file>>a;file>>b;file>>c;file>>d;
      payload_points.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      payload_normals.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      payload_colors.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
  }

  panel1_points.clear();
  panel1_colors.clear();
  panel2_normals.clear();
  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 0){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  for(int i=0; i<std::stoi(num_points); ++i){
      std::string a, b, c, d;
      file>>a;file>>b;file>>c;file>>d;
      panel1_points.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      panel1_normals.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      panel1_colors.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
  }

  panel2_points.clear();
  panel2_colors.clear();
  panel2_normals.clear();
  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 0){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  for(int i=0; i<std::stoi(num_points); ++i){
      std::string a, b, c, d;
      file>>a;file>>b;file>>c;file>>d;
      panel2_points.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      panel2_normals.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      panel2_colors.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
  }

  file.close();

  filename = "./data/model2_rocket.raw";
  file.open(filename.c_str());
  std::string strip;

  rocket_points.clear();
  rocket_colors.clear();
  rocket_normals.clear();
  rocket_strip_indices.clear();

  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 0){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  for(int i=0; i<std::stoi(num_points); ++i){
      std::string a, b, c, d;
      file>>a;file>>b;file>>c;file>>d;
      rocket_points.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      rocket_normals.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      rocket_colors.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
  }
  while(file>>strip){
      rocket_strip_indices.push_back(std::stof(strip));
  }

  file.close();
  // for(int i=0; i<rocket_strip_indices.size();++i) std::cout<<rocket_strip_indices[i]<<" ";
  // std::cout<<"\n";
  // std::cout<<"created rocket \n";

  filename = "./data/model1.raw";
  file.open(filename.c_str());

  model_points.clear();
  model_colors.clear();
  model_normals.clear();

  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 0){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  for(int i=0; i<std::stoi(num_points); ++i){
      std::string a, b, c, d;
      file>>a;file>>b;file>>c;file>>d;
      model_points.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      model_normals.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      model_colors.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
  }

  tower_points.clear();
  tower_colors.clear();
  tower_normals.clear();

  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 0){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  for(int i=0; i<std::stoi(num_points); ++i){
      std::string a, b, c, d;
      file>>a;file>>b;file>>c;file>>d;
      tower_points.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      tower_normals.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      tower_colors.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
  }

  protrusions_points.clear();
  protrusions_colors.clear();
  protrusions_normals.clear();

  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 0){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  prot_shift = 0;//5-protrusionLength;
  t_shift=(depBase-depPad)/2-depTower;
  for(int i=0; i<std::stoi(num_points); ++i){
      std::string a, b, c, d;
      file>>a;file>>b;file>>c;file>>d;
      protrusions_points.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      protrusions_normals.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      protrusions_colors.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
  }

  ground_points.clear();
  ground_textures.clear();
  ground_normals.clear();

  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 1){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  for(int i=0; i<std::stoi(num_points); ++i){
      std::string a, b, c, d;
      file>>a;file>>b;file>>c;file>>d;
      ground_points.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      ground_normals.push_back(glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      ground_textures.push_back(glm::vec2(std::stof(a),std::stof(b)));
  }

  if(!(file>>num_points && file>>c_or_t)){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  if (std::stoi(c_or_t) != 1){
    std::cout<<"Error in loading raw file \n";
    return;
  }
  for(int i=0; i<std::stoi(num_points); ++i){
      std::string a, b, c, d;
      file>>a;file>>b;file>>c;file>>d;
      outer_positions[i] = (glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      outer_normals[i] = (glm::vec4(std::stof(a),std::stof(b),std::stof(c),std::stof(d)));
      file>>a;file>>b;file>>c;file>>d;
      outer_tex_coords[i] = (glm::vec2(std::stof(a),std::stof(b)));
  }

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
  
  glm::vec4 model_points_array[model_points.size()];
  glm::vec4 model_colors_array[model_colors.size()];
  glm::vec4 model_normals_array[model_normals.size()];
  std::copy(model_points.begin(), model_points.end(), model_points_array);
  std::copy(model_colors.begin(), model_colors.end(), model_colors_array);
  std::copy(model_normals.begin(), model_normals.end(), model_normals_array);

  glm::vec4 tower_points_array[tower_points.size()];
  glm::vec4 tower_colors_array[tower_colors.size()];
  glm::vec4 tower_normals_array[tower_normals.size()];
  std::copy(tower_points.begin(), tower_points.end(), tower_points_array);
  std::copy(tower_colors.begin(), tower_colors.end(), tower_colors_array);
  std::copy(tower_normals.begin(), tower_normals.end(), tower_normals_array);

  glm::vec4 protrusions_points_array[protrusions_points.size()];
  glm::vec4 protrusions_colors_array[protrusions_colors.size()];
  glm::vec4 protrusions_normals_array[protrusions_normals.size()];
  std::copy(protrusions_points.begin(), protrusions_points.end(), protrusions_points_array);
  std::copy(protrusions_colors.begin(), protrusions_colors.end(), protrusions_colors_array);
  std::copy(protrusions_normals.begin(), protrusions_normals.end(), protrusions_normals_array);

  glm::vec4 ground_points_array[ground_points.size()];
  glm::vec2 ground_textures_array[ground_textures.size()];
  glm::vec4 ground_normals_array[ground_normals.size()];
  std::copy(ground_points.begin(), ground_points.end(), ground_points_array);
  std::copy(ground_textures.begin(), ground_textures.end(), ground_textures_array);
  std::copy(ground_normals.begin(), ground_normals.end(), ground_normals_array);

  tex[0]=LoadTexture("data/ground-texture.bmp",3072,2304);
  tex[1]=LoadTexture("data/blue-sky.bmp",3888,2592);

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
//   //////////////////  rocket  ///////////////////////////  
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


  // ////////////////////////////////  model  /////////////////////////////////////////////////
  // Set it as the current array to be used by binding it
  glBindVertexArray (vao[4]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[4]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(model_points_array) + sizeof(model_colors_array) + sizeof(model_normals_array), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(model_points_array), model_points_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(model_points_array), sizeof(model_colors_array), model_colors_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(model_points_array) + sizeof(model_colors_array), sizeof(model_normals_array), model_normals_array );
  
  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor1 );
  glVertexAttribPointer( vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(model_points_array)));

  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(model_points_array) + sizeof(model_colors_array)) );

  //////////////////tower///////////////////////////
  glBindVertexArray (vao[5]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[5]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(tower_points_array) + sizeof(tower_colors_array) + sizeof(tower_normals_array), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(tower_points_array), tower_points_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(tower_points_array), sizeof(tower_colors_array), tower_colors_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(tower_points_array) + sizeof(tower_colors_array), sizeof(tower_normals_array), tower_normals_array );
  
  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor1 );
  glVertexAttribPointer( vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(tower_points_array)));

  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(tower_points_array) + sizeof(tower_colors_array)) );
  
  //////////////////protrusions///////////////////////////
  glBindVertexArray (vao[6]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[6]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(protrusions_points_array) + sizeof(protrusions_colors_array) + sizeof(protrusions_normals_array), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(protrusions_points_array), protrusions_points_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(protrusions_points_array), sizeof(protrusions_colors_array), protrusions_colors_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(protrusions_points_array) + sizeof(protrusions_colors_array), sizeof(protrusions_normals_array), protrusions_normals_array );
  
  // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor1 );
  glVertexAttribPointer( vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(protrusions_points_array)));

  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(protrusions_points_array) + sizeof(protrusions_colors_array)) );

  //////////////////ground///////////////////////////
  glBindVertexArray (vao[7]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[7]);
  //Copy the points into the current buffer
  glBufferData (GL_ARRAY_BUFFER, sizeof(ground_points_array) + sizeof(ground_textures_array) + sizeof(ground_normals_array), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(ground_points_array), ground_points_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(ground_points_array), sizeof(ground_textures_array), ground_textures_array);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(ground_points_array) + sizeof(ground_textures_array), sizeof(ground_normals_array), ground_normals_array );
      // set up vertex arrays
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( texCoord1 );
  glVertexAttribPointer( texCoord1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(ground_points_array)));

  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(ground_points_array) + sizeof(ground_textures_array)) );

  glBindVertexArray (vao[8]);
  //Set 8 as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[8]);

  glBufferData (GL_ARRAY_BUFFER, sizeof (outer_positions) + sizeof(outer_tex_coords) + sizeof(outer_normals), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(outer_positions), outer_positions );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(outer_positions), sizeof(outer_tex_coords), outer_tex_coords);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(outer_tex_coords)+sizeof(outer_positions), sizeof(outer_normals), outer_normals );
  // set up vertex array
  //Position
  glEnableVertexAttribArray( vPosition1 );
  glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  //Textures
  glEnableVertexAttribArray( texCoord1 );
  glVertexAttribPointer( texCoord1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(outer_positions)) );

  //Normal
  glEnableVertexAttribArray( vNormal1 );
  glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(outer_positions)+sizeof(outer_tex_coords)) );

  origin_translation_payload = glm::translate(glm::mat4(1.0f), glm::vec3(-lenPayload/2, -widPayload/2, -depPayload/2));
  origin_translation_base = glm::translate(glm::mat4(1.0f), glm::vec3(-lenBase/2, 0.0, -depBase/2));
  // projection_matrix = glm::ortho(-100.0, 100.0, -100.0, 100.0, -100.0, 1000.0);
  projection_matrix = glm::frustum(-1.5, 1.5, -1.5, 1.5, 1.0, 2000.0);
  payload_translation = glm::translate(glm::mat4(1.0f), glm::vec3(4, 106+widPad, 4));
  payload_scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
  rocket_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0, widPad+6, 0));
  glUniform1f(lx, 200);
  glUniform1f(ly, 200);
  glUniform1f(lz, 0);
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f,0.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f,1.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f,0.0f,1.0f));
  model_matrix = payload_translation * rotation_matrix * origin_translation_payload * payload_scale;

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

  model_matrix = rotation_matrix * origin_translation_base;
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[4]);  
  glDrawArrays(GL_TRIANGLES, 0, model_points.size());

  // Draw the tower
  tower_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, t_shift));
  modelview_matrix = view_matrix * model_matrix * tower_translation;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  // normal_matrix = glm::mat3(modelview_matrix);
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[5]);  
  glDrawArrays(GL_TRIANGLES, 0, tower_points.size());

    // Draw the protrusion
  protrusion_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, prot_shift));
  modelview_matrix = view_matrix * model_matrix * tower_translation * protrusion_translation;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  // normal_matrix = glm::mat3(modelview_matrix);
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[6]);  
  glDrawArrays(GL_TRIANGLES, 0, protrusions_points.size());

    // Ground
  glUniform1f(texExist1, 1.0);
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  // normal_matrix = glm::mat3(modelview_matrix);
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[0]);
  glBindVertexArray(vao[7]);
  glDrawArrays(GL_TRIANGLES, 0, ground_points.size());

  // Outer
  modelview_matrix = view_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  //  glBindTexture(GL_TEXTURE_2D, tex);
  glBindTexture(GL_TEXTURE_2D, tex[1]);
  glBindVertexArray (vao[8]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, num_vertices);
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
