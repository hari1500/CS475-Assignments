#ifndef _HNODE_HPP_
#define _HNODE_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include<algorithm>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#include "gl_framework.hpp"


namespace csX75	 { 

	// A simple class that represents a node in the hierarchy tree
	class HNode {
		//glm::vec4 * vertices;
		//glm::vec4 * colors;
		//glm::vec4 * normals;
		GLfloat tx,ty,tz,rx,ry,rz;

		std::size_t vertex_buffer_size;
		std::size_t color_buffer_size;
		std::size_t normal_buffer_size;

		GLuint num_vertices;
		GLuint hierarchy_vao,hierarchy_vbo;

		glm::mat4 rotation;
		glm::mat4 translation;
		
		std::vector<HNode*> children;
		HNode* parent;

		void update_matrices();

	  public:
		HNode (HNode*, GLuint, glm::vec4*,  glm::vec4*, glm::vec4*, std::size_t, std::size_t, std::size_t);
		//HNode (HNode* , glm::vec4*,  glm::vec4*,GLfloat,GLfloat,GLfloat,GLfloat,GLfloat,GLfloat);

		void add_child(HNode*);
		void del_child(HNode*);
		void del_parent();
		void render();
		void change_to_parameters(GLfloat,GLfloat,GLfloat,GLfloat,GLfloat,GLfloat);
		void change_by_parameters(GLfloat,GLfloat,GLfloat,GLfloat,GLfloat,GLfloat);
		void render_tree();
		glm::vec4 get_trans_params();
		glm::vec4 get_rot_params();
	};

	glm::mat4* multiply_stack(std::vector <glm::mat4> );
};	

#endif