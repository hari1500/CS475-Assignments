#include "hierarchy_node.hpp"
#include <iostream>

extern GLuint vPosition1,vColor1,vNormal1,uModelViewMatrix1,normalMatrix1;
extern std::vector<glm::mat4> matrixStack;

namespace csX75
{

	HNode::HNode(HNode* a_parent, GLuint num_v, glm::vec4* a_vertices, glm::vec4* a_colours, glm::vec4* a_normals, std::size_t v_size, std::size_t c_size, std::size_t n_size){

		num_vertices = num_v;
		vertex_buffer_size = v_size;
		color_buffer_size = c_size;
		normal_buffer_size = n_size;
		// initialize hierarchy_vao and hierarchy_vbo of the object;


		//Ask GL for a Vertex Attribute Objects (hierarchy_vao)
		glGenVertexArrays (1, &hierarchy_vao);
		//Ask GL for aVertex Buffer Object (hierarchy_vbo)
		glGenBuffers (1, &hierarchy_vbo);

		//bind them
		glBindVertexArray (hierarchy_vao);
		glBindBuffer (GL_ARRAY_BUFFER, hierarchy_vbo);

		
		glBufferData (GL_ARRAY_BUFFER, vertex_buffer_size + color_buffer_size + normal_buffer_size, NULL, GL_STATIC_DRAW);
		glBufferSubData( GL_ARRAY_BUFFER, 0, vertex_buffer_size, a_vertices );
		glBufferSubData( GL_ARRAY_BUFFER, vertex_buffer_size, color_buffer_size, a_colours );
		glBufferSubData( GL_ARRAY_BUFFER, vertex_buffer_size+color_buffer_size, normal_buffer_size, a_normals );

		//setup the vertex array as per the shader
		glEnableVertexAttribArray( vPosition1 );
		glVertexAttribPointer( vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

		glEnableVertexAttribArray( vColor1 );
		glVertexAttribPointer( vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertex_buffer_size));

		glEnableVertexAttribArray( vNormal1 );
		glVertexAttribPointer( vNormal1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertex_buffer_size + color_buffer_size));
		// set parent

		if(a_parent == NULL){
			parent = NULL;
		}
		else{
			parent = a_parent;
			parent->add_child(this);
		}

		//initial parameters are set to 0;
		tx=ty=tz=rx=ry=rz=0;

		update_matrices();
	}

	void HNode::update_matrices(){
		rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rx), glm::vec3(1.0f,0.0f,0.0f));
		rotation = glm::rotate(rotation, glm::radians(ry), glm::vec3(0.0f,1.0f,0.0f));
		rotation = glm::rotate(rotation, glm::radians(rz), glm::vec3(0.0f,0.0f,1.0f));

		translation = glm::translate(glm::mat4(1.0f),glm::vec3(tx,ty,tz));
	}

	void HNode::add_child(HNode* a_child){
		children.push_back(a_child);
	}
	void HNode::del_child(HNode* a_child){
		children.erase(std::find(children.begin(),children.end(),a_child));
	}

	void HNode::del_parent(){
		parent->del_child(this);
		parent = NULL;
	}

	void HNode::change_to_parameters(GLfloat atx, GLfloat aty, GLfloat atz, GLfloat arx, GLfloat ary, GLfloat arz){
		tx = atx; ty = aty; tz = atz; rx = arx; ry = ary; rz = arz;

		update_matrices();
	}

	void HNode::change_by_parameters(GLfloat atx, GLfloat aty, GLfloat atz, GLfloat arx, GLfloat ary, GLfloat arz){
		tx += atx; ty += aty; tz += atz; rx += arx; ry += ary; rz += arz;

		update_matrices();
	}

	glm::vec4 HNode::get_trans_params(){
		return glm::vec4(tx, ty, tz, 1.0);
	};

	glm::vec4 HNode::get_rot_params(){
		return glm::vec4(rx, ry, rz, 1.0);
	};

	void HNode::render(){

		//matrixStack multiply
		glm::mat4* ms_mult = multiply_stack(matrixStack);

		glUniformMatrix4fv(uModelViewMatrix1, 1, GL_FALSE, glm::value_ptr(*ms_mult));
		glm::mat3 normal_matrix = glm::transpose (glm::inverse(glm::mat3(*ms_mult)));
  		glUniformMatrix3fv(normalMatrix1, 1, GL_FALSE, glm::value_ptr(normal_matrix));
		glBindVertexArray (hierarchy_vao);
		glDrawArrays(GL_TRIANGLES, 0, num_vertices);

		// for memory 
		delete ms_mult;

	}

	void HNode::render_tree(){
		
		matrixStack.push_back(translation);
		matrixStack.push_back(rotation);

		render();
		for(int i=0;i<children.size();i++){
			children[i]->render_tree();
		}
		matrixStack.pop_back();
		matrixStack.pop_back();

	}

	glm::mat4* multiply_stack(std::vector<glm::mat4> matStack){
		glm::mat4* mult;
		mult = new glm::mat4(1.0f);
	
		for(int i=0;i<matStack.size();i++){
			*mult = (*mult) * matStack[i];
		}	

		return mult;
	}

}