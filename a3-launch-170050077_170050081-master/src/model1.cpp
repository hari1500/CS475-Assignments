#include "model1.hpp"
#include "texture.hpp"


GLuint shaderProgram1;
glm::mat4 translation;
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

glm::mat4 tower_translation;
glm::mat4 protrusion_translation;

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
// Tower model shift
GLfloat t_shift=0.0, prot_shift=0.0;
//Running variable to toggle culling on/off
bool enable_culling=true;
//Running variable to toggle wireframe/solid modelling
bool solid=true;
//Enable/Disable perspective view
bool enable_perspective=false;
//-----------------------------------------------------------------

//points in homogenous coordinates
glm::vec4 positions[8], normals[8] = {
  glm::vec4(-1.0, -1.0, 1.0, 1.0),
  glm::vec4(-1.0, 1.0, 1.0, 1.0),
  glm::vec4(1.0, 1.0, 1.0, 1.0),
  glm::vec4(1.0, -1.0, 1.0, 1.0),
  glm::vec4(-1.0, -1.0, -1.0, 1.0),
  glm::vec4(-1.0, 1.0, -1.0, 1.0),
  glm::vec4(1.0, 1.0, -1.0, 1.0),
  glm::vec4(1.0, -1.0, -1.0, 1.0)
};
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

void sphere(double radius)
{
  float lats,longs;

  int slices = 8; 
  int sectors = 4;

  float l;

  int tri_idx = 0;
  for (lats = 0.0; lats <= 180; lats+=sectors)  
  {
    float lats_rad = glm::radians(lats);
    if(lats+sectors>180)
        l=180;
      else
        l=lats+sectors;
    float l_rad = glm::radians(l);    

    for(longs = 0.0; longs <= 360; longs+=slices)
    {
      float longs_rads = glm::radians(longs);
      float x = radius * sin(lats_rad) * cos(longs_rads);
      float y = radius * sin(lats_rad) * sin(longs_rads);
      float z = radius * cos(lats_rad);
      glm::vec4 pt(x, y, z, 1.0);
      glm::vec4 pt1(-x, -y, -z, 1.0);
      outer_positions[tri_idx] = pt; outer_normals[tri_idx] = pt1;
      outer_tex_coords[tri_idx] = glm::vec2(longs/360, (1-lats/180)); tri_idx++;
      
      
      x = radius * sin(l_rad) * cos(longs_rads);
      y = radius * sin(l_rad) * sin(longs_rads);
      z = radius * cos(l_rad);
      pt =glm::vec4(x, y, z, 1.0);
      pt1 = glm::vec4(-x, -y, -z, 1.0);
      outer_positions[tri_idx] = pt; outer_normals[tri_idx] = pt1;
      outer_tex_coords[tri_idx] = glm::vec2(longs/360, (1-l/180)); tri_idx++;
    }
  }
}
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
  // std::vector<std::vector<int>> quads {
  //   { 1, 0, 3, 2 },
  //   { 2, 3, 7, 6 },
  //   { 3, 0, 4, 7 },
  //   { 6, 5, 1, 2 },
  //   { 4, 5, 6, 7 },
  //   { 5, 4, 0, 1 },
  // };
  // for(int i=0; i<quads.size(); ++i) {
  //   int a = quads[i][0], b = quads[i][1], c = quads[i][2], d = quads[i][3];
  //   out_normals.push_back(normals[a]);
  //   out_normals.push_back(normals[b]);
  //   out_normals.push_back(normals[c]);
  //   out_normals.push_back(normals[a]);
  //   out_normals.push_back(normals[c]);
  //   out_normals.push_back(normals[d]);
  // }
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(0.0, 0.0, 1.0, 1.0));
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(1.0, 0.0, 0.0, 1.0));
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(0.0, -1.0, 0.0, 1.0));
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(0.0, 1.0, 0.0, 1.0));
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(0.0, 0.0, -1.0, 1.0));
  for(int i=0; i<6;++i) out_normals.push_back(glm::vec4(-1.0, 0.0, 0.0, 1.0));

  return out_normals;
}

std::vector<glm::vec2> setTextureCuboid()
{
  std::vector<glm::vec2> out_texture;
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
    if(i==3){
      out_texture.push_back(glm::vec2(1.0, 0.0));
      out_texture.push_back(glm::vec2(0.0, 0.0));
      out_texture.push_back(glm::vec2(0.0, 1.0));
      out_texture.push_back(glm::vec2(1.0, 0.0));
      out_texture.push_back(glm::vec2(0.0, 1.0));
      out_texture.push_back(glm::vec2(1.0, 1.0));
    } else {
      out_texture.push_back(glm::vec2(1.0, 0.0));
      out_texture.push_back(glm::vec2(0.0, 0.0));
      out_texture.push_back(glm::vec2(0.0, 1.0));
      out_texture.push_back(glm::vec2(1.0, 0.0));
      out_texture.push_back(glm::vec2(0.0, 1.0));
      out_texture.push_back(glm::vec2(1.0, 1.0));
    }
  }

  return out_texture;
}

//-----------------------------------------------------------------

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

  std::vector<std::array<float, 12>> mymodel;
  int oldSize = 0;
  std::string filename = "./data/model1.raw";
  std::ofstream output_file(filename);
  std::ostream_iterator<float> output_iter(output_file, " ");

  model_points.clear();
  model_colors.clear();
  model_normals.clear();
  //base//
  std::vector<glm::vec4> temp = setPositionsCuboid(glm::vec4(0.0, 0.0, 0.0, 0.0), lenBase, widBase, depBase);
  model_points.insert(model_points.end(), temp.begin(), temp.end());
  temp = setColorsCuboid(glm::vec4(0.5555  , 0.5555, 0.5555, 1.0));
  model_colors.insert(model_colors.end(), temp.begin(), temp.end());
  temp = setNormalsCuboid();
  model_normals.insert(model_normals.end(), temp.begin(), temp.end());
  //building//
  temp = setPositionsCuboid(glm::vec4(0.0, widBase, (depBase-depBuilding)/2, 0.0), lenBuilding, widBuilding, depBuilding);
  model_points.insert(model_points.end(), temp.begin(), temp.end());
  temp = setColorsCuboid(glm::vec4(1.0, 0.8, 0.0, 1.0));
  model_colors.insert(model_colors.end(), temp.begin(), temp.end());
  temp = setNormalsCuboid();
  model_normals.insert(model_normals.end(), temp.begin(), temp.end());
  // pad-for-rocket-launching//
  temp = setPositionsCuboid(glm::vec4((lenBase-lenPad)/2, widBase, (depBase-depPad)/2, 0.0), lenPad, widPad, depPad);
  model_points.insert(model_points.end(), temp.begin(), temp.end());
  temp = setColorsCuboid(glm::vec4(0.3, 0.7, 0.7, 1.0));
  model_colors.insert(model_colors.end(), temp.begin(), temp.end());
  temp = setNormalsCuboid();
  model_normals.insert(model_normals.end(), temp.begin(), temp.end());

  oldSize = mymodel.size();
  for ( int i = 0; i < model_points.size(); ++i ) {
    mymodel.push_back({model_points[i][0], model_points[i][1], model_points[i][2], model_points[i][3], model_normals[i][0], model_normals[i][1], model_normals[i][2], model_normals[i][3], model_colors[i][0], model_colors[i][1], model_colors[i][2], model_colors[i][2]});
  }

  output_file << model_points.size();
  output_file << " ";
  output_file << "0\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

  // for(int i=0; i<model_points.size();++i){
  //   std::cout<<model_colors[i][0]<<" "<<model_colors[i][1]<<" "<<model_colors[i][2]<<"\n";
  // }
  tower_points.clear();
  tower_colors.clear();
  tower_normals.clear();
  //base-tower//
  tower_points = setPositionsCuboid(glm::vec4((lenBase-lenTower)/2, widBase, 0.0, 0.0), lenTower, widTower, depTower);
  tower_colors = setColorsCuboid(glm::vec4(0.3, 0.7, 0.7, 1.0));
  tower_normals = setNormalsCuboid();

  oldSize = mymodel.size();
  for ( int i = 0; i < tower_points.size(); ++i ) {
    mymodel.push_back({tower_points[i][0], tower_points[i][1], tower_points[i][2], tower_points[i][3], tower_normals[i][0], tower_normals[i][1], tower_normals[i][2], tower_normals[i][3], tower_colors[i][0], tower_colors[i][1], tower_colors[i][2], tower_colors[i][2]});
  }

  output_file << tower_points.size();
  output_file << " ";
  output_file << "0\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

  //protrusions//
  protrusions_points.clear();
  protrusions_colors.clear();
  protrusions_normals.clear();
  prot_shift = 5-protrusionLength;
  for(int i=0; i<numProtrusions; ++i){
    temp = setPositionsCuboid(glm::vec4((lenBase-protrusioonDimensions[2])/2, protrusions_heights[i], depTower, 0.0), protrusioonDimensions[2], protrusioonDimensions[1], protrusioonDimensions[0]);
    protrusions_points.insert(protrusions_points.end(), temp.begin(), temp.end());
    temp = setColorsCuboid(glm::vec4(0.0, 0.0, 0.3, 1.0));
    protrusions_colors.insert(protrusions_colors.end(), temp.begin(), temp.end());
    temp = setNormalsCuboid();
    protrusions_normals.insert(protrusions_normals.end(), temp.begin(), temp.end());
  }

  oldSize = mymodel.size();
  for ( int i = 0; i < protrusions_points.size(); ++i ) {
    mymodel.push_back({protrusions_points[i][0], protrusions_points[i][1], protrusions_points[i][2], protrusions_points[i][3], protrusions_normals[i][0], protrusions_normals[i][1], protrusions_normals[i][2], protrusions_normals[i][3], protrusions_colors[i][0], protrusions_colors[i][1], protrusions_colors[i][2], protrusions_colors[i][2]});
  }

  output_file << protrusions_points.size();
  output_file << " ";
  output_file << "0\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

  //////Ground/////
  ground_points.clear();
  ground_textures.clear();
  ground_normals.clear();
  ground_points = setPositionsCuboid(glm::vec4(-(lenGround-lenBase)/2, -widGround, -(depGround-depBase)/2, 0.0), lenGround, widGround, depGround);
  ground_textures = setTextureCuboid();
  ground_normals = setNormalsCuboid();

  oldSize = mymodel.size();
  for ( int i = 0; i < ground_points.size(); ++i ) {
    mymodel.push_back({ground_points[i][0], ground_points[i][1], ground_points[i][2], ground_points[i][3], ground_normals[i][0], ground_normals[i][1], ground_normals[i][2], ground_normals[i][3], ground_textures[i][0], ground_textures[i][1], 0, 0});
  }

  output_file << ground_points.size();
  output_file << " ";
  output_file << "1\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

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

  // for(int i=0; i<tower_points.size();++i){
  //   std::cout<<i<<" "<<tower_points_array[i][0]<<" "<<tower_points_array[i][1]<<" "<<tower_points_array[i][2]<<"\n";
  // } 

  // Load Textures 
  tex[0]=LoadTexture("data/ground-texture.bmp",3072,2304);
  tex[1]=LoadTexture("data/blue-sky.bmp",3888,2592);
  // tex[1]=LoadTextureAll("data/blue-sky-with-white-clouds-texture.jpg");
  // tex[1]=LoadTextureAll("data/blue-sky.bmp");


  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (NUM_BUFFERS, vao);
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (NUM_BUFFERS, vbo);
  // ////////////////////////////////  model  /////////////////////////////////////////////////
  // Set it as the current array to be used by binding it
  glBindVertexArray (vao[0]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
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
  glBindVertexArray (vao[1]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);
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
  glBindVertexArray (vao[2]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[2]);
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
  glBindVertexArray (vao[3]);

  //Set it as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[3]);
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

  ////////////////////////// outer ///////////////////////////
  //Set 4 as the current array to be used by binding it
  glBindVertexArray (vao[4]);
  //Set 4 as the current buffer to be used by binding it
  glBindBuffer (GL_ARRAY_BUFFER, vbo[4]);

  sphere(earthRadius);
  
  oldSize = mymodel.size();
  for ( int i = 0; i < num_vertices; ++i ) {
    mymodel.push_back({outer_positions[i][0], outer_positions[i][1], outer_positions[i][2], outer_positions[i][3], outer_normals[i][0], outer_normals[i][1], outer_normals[i][2], outer_normals[i][3], outer_tex_coords[i][0], outer_tex_coords[i][1], 0.0, 0.0});
  }

  output_file << num_vertices;
  output_file << " ";
  output_file << "1\n";
  for ( int i = oldSize; i < mymodel.size(); ++i ) {
    copy(mymodel[i].begin(), mymodel[i].end(), output_iter);
    output_file << '\n';
  }

  //Copy the points into the current buffer
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

  translation = glm::translate(glm::mat4(1.0f), glm::vec3(-lenBase/2, 0.0, -depBase/2));
  // projection_matrix = glm::ortho(-100.0, 100.0, -100.0, 100.0, -100.0, 1000.0);
  projection_matrix = glm::frustum(-1.5, 1.5, -1.5, 1.5, 1.0, 2000.0);
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
  model_matrix = rotation_matrix * translation;

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

  // glUseProgram( shaderProgram1 );

  // Draw the model
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[0]);  
  glDrawArrays(GL_TRIANGLES, 0, model_points.size());

  // Draw the tower
  tower_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, t_shift));
  modelview_matrix = view_matrix * model_matrix * tower_translation;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  // normal_matrix = glm::mat3(modelview_matrix);
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[1]);  
  glDrawArrays(GL_TRIANGLES, 0, tower_points.size());

  // Draw the protrusion
  protrusion_translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, prot_shift));
  modelview_matrix = view_matrix * model_matrix * tower_translation * protrusion_translation;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  // normal_matrix = glm::mat3(modelview_matrix);
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[2]);  
  glDrawArrays(GL_TRIANGLES, 0, protrusions_points.size());

  // Ground
  glUniform1f(texExist1, 1.0);
  modelview_matrix = view_matrix*model_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  // normal_matrix = glm::mat3(modelview_matrix);
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindTexture(GL_TEXTURE_2D, tex[0]);
  glBindVertexArray(vao[3]);
  glDrawArrays(GL_TRIANGLES, 0, ground_points.size());

  // Outer
  modelview_matrix = view_matrix;
  glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose (glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  //  glBindTexture(GL_TEXTURE_2D, tex);
  glBindTexture(GL_TEXTURE_2D, tex[1]);
  glBindVertexArray (vao[4]);
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
  window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "model1", NULL, NULL);
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
