
#pragma once

#include <Rendering/Mesh.h>

typedef unsigned int uint;

struct Simplex {
	glm::vec3 a, b, c, d; //vertices of a possible tetrahetron, triangle(a,b,c) or line(a, b)
	uint shape; //denotes shape type
	Simplex() {
		shape = 0;
	}
	void triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
		shape = 3; this->a = a; this->b = b; this->c = c;
	}
	void line(glm::vec3 a, glm::vec3 b) {
		shape = 2; this->a = a; this->b = b;
	}
	void push(glm::vec3 p) {
		if (shape == 0) {
			a = p;
		}
		else if (shape == 1) {
			b = a; a = p;
		}
		else if (shape == 2) {
			c = b; b = a; a = p;
		}
		else {
			d = c; c = b; b = a; a = p;
		}
		shape = (shape + 1) % 5;
	}
};

//Returns the support point(furthest along a direction) from a mesh
glm::vec3 getShapeSupport(std::vector<glm::vec3> &verts, glm::vec3 &direction) {
	uint farthestIDX = 0;
	GLfloat farthest = glm::dot(verts[0], direction);
	for (uint i = 1; i < verts.size(); i++) {
		GLfloat dist = glm::dot(verts[i], direction);
		if (dist > farthest) {
			farthestIDX = i;
			farthest = dist;
		}
	}
	return verts[farthestIDX];
}

//Calls the above, and denotes the Minkowski difference of the supports of two meshes along a direction
glm::vec3 getSupportOfTwoShapes(std::vector<glm::vec3> &verts_ofA, std::vector<glm::vec3> &verts_ofB, glm::vec3 &direction) {
	return getShapeSupport(verts_ofA, direction) - getShapeSupport(verts_ofB, -direction);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
Mesh::Mesh():castsShadow(true), solid_color(false){
	r = 1.0f;  g = b = 0.0f;//red
}

Mesh::Mesh(unsigned int vbo, unsigned ibo, unsigned vao, unsigned count, std::vector<glm::vec3> &worldSpaceVerts, std::vector<uint> &indicesCPU) {
	this->vbo = vbo; this->ibo = ibo; this->vao = vao; this->count = count;
	this->worldSpaceVerts = objSpaceVerts = worldSpaceVerts; this->indicesCPU = indicesCPU;
	r = 1.0f;  g = b = 0.0f;//red
	modelMatrix = glm::mat4(1);
	name = "";
}

void Mesh::SetModelMatrix(glm::mat4 &newModelMat)
{
	this->modelMatrix = newModelMat;
	updated = false;
}

glm::mat4 Mesh::GetModelMatrix()
{
	if (!updated)
		Update();
	return modelMatrix;
}

void Mesh::Update()
{
//TODO
	updated = true;
}
void Mesh::setMeshName(const char* n) {
	name = "";
	name += n;
}

Mesh::~Mesh() {
	//distruge mesh incarcat
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	//nu distrug program pt ca nu am ownership.. state machine design
}

void Mesh::Render(Shader *shader, bool color) {//virtual
	glBindVertexArray(vao);
	const GLint POSITION_LOCATION = shader->GetAttributeLocation("in_position");
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);
	
	if (color)
	{
		const GLint NORMAL_LOCATION = shader->GetAttributeLocation("in_normal");
		const GLint TEX_COORD_LOCATION = shader->GetAttributeLocation("TexCoord");
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(3 * sizeof(GLfloat)));

		glEnableVertexAttribArray(TEX_COORD_LOCATION);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(6 * sizeof(GLfloat)));
	}

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);//this should probably be vertexArrays
	glBindVertexArray(0);
}

void Mesh::init(std::vector<VertexFormat>vertices, std::vector<GLuint> indices) {
	printf("\nCALLING Mesh::init(...)\n");
	//creeaza obiectele OpenGL necesare desenarii
	unsigned int gl_vertex_array_object, gl_vertex_buffer_object, gl_index_buffer_object;

	//vertex array object -> un obiect ce reprezinta un container pentru starea de desenare
	glGenVertexArrays(1, &gl_vertex_array_object);
	glBindVertexArray(gl_vertex_array_object);

	//vertex buffer object -> un obiect in care tinem vertecsii
	glGenBuffers(1, &gl_vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(VertexFormat), &vertices[0], GL_STATIC_DRAW);

	//index buffer object -> un obiect in care tinem indecsii
	glGenBuffers(1, &gl_index_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_index_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//legatura intre atributele vertecsilor si pipeline, datele noastre sunt INTERLEAVED.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);						//trimite pozitii pe pipe 0
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(GLfloat) * 3));		//trimite normale pe pipe 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(GLfloat) * 3));	//trimite texcoords pe pipe 2

	vao = gl_vertex_array_object;
	vbo = gl_vertex_buffer_object;
	ibo = gl_index_buffer_object;
	count = indices.size();
}
void Mesh::updateVertices() {
	for (unsigned i = 0; i < worldSpaceVerts.size(); i++) {
		worldSpaceVerts[i] = glm::vec3(this->modelMatrix * glm::vec4(objSpaceVerts[i], 1));
	}
	position = glm::vec3(modelMatrix * glm::vec4(0, 0, 0, 1));
}
//mesh transformation functions
void Mesh::setPosition(glm::vec3 newPosition) {
	this->modelMatrix[3][0] = newPosition.x;
	this->modelMatrix[3][1] = newPosition.y;
	this->modelMatrix[3][2] = newPosition.z;
	this->position = newPosition;
	updateVertices();
}

void Mesh::translateWorldX(GLfloat distance) {
	this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(distance, 0, 0));
	updateVertices();
}
void Mesh::translateWorldY(GLfloat distance) {
	this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(0, distance, 0));
	updateVertices();
}
void Mesh::translateWorldZ(GLfloat distance) {
	this->modelMatrix = glm::translate(this->modelMatrix, glm::vec3(0, 0, distance));
	updateVertices();
}
void Mesh::scaleLocalX(GLfloat scale_factor) {
	this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(scale_factor, 1, 1));
	updateVertices();
}
void Mesh::scaleLocalY(GLfloat scale_factor) {
	this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(1, scale_factor, 1));
	updateVertices();
}
void Mesh::scaleLocalZ(GLfloat scale_factor) {
	this->modelMatrix = glm::scale(this->modelMatrix, glm::vec3(1, 1, scale_factor));
	updateVertices();
}
void Mesh::rotateLocalX(GLfloat angle) {
	this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(angle), glm::vec3(1, 0, 0));
	updateVertices();
}
void Mesh::rotateLocalY(GLfloat angle) {
	this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
	updateVertices();
}
void Mesh::rotateLocalZ(GLfloat angle) {
	this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(angle), glm::vec3(0, 0, 1));
	updateVertices();
}

void Mesh::setColor(GLfloat red, GLfloat green, GLfloat blue) {
	r = red; g = green; b = blue;
}

//GILBERT-JOHNSON-KEERTHI (GJK) collision detection algorithm, returns true if this mesh collides "test" mesh
//Function tries to encapsulate the origin within a simplex built by two support points of this mesh and "test" in a search direction
//updated iteratively, the support is the Minkowski sum of the furthest two points of this mesh and the reflection of "test"
bool Mesh::collidesWith(Mesh *test) {
	Simplex simplex;
	const GLuint MAX_ITERATIONS = 75;
	unsigned iteration = 0;

	//pick a random initial support search direction
	glm::vec3 dir = glm::vec3(1, 0, 0);
	glm::vec3 s = getSupportOfTwoShapes(this->worldSpaceVerts, test->worldSpaceVerts, dir);
	if (fabs(glm::dot(dir, s)) >= glm::dot(s, s)*0.8f) {
		//avoid origin search direction, vector (0,0,0) wouldn't be a direction
		dir = glm::vec3(0, 1, 0);
		s = getSupportOfTwoShapes(this->worldSpaceVerts, test->worldSpaceVerts, dir);
	}
	simplex.push(s);
	dir = -s;
	//search the origin within a simplex updated with respect to origin's direction 
	while (true) {
		if (iteration++ >= MAX_ITERATIONS) return false;

		// get the next support direction that converges towards the origin
		glm::vec3 a = getSupportOfTwoShapes(this->worldSpaceVerts, test->worldSpaceVerts, dir);

		//cannot construct tetrahedron with angle and search direction spread apart more than 90deg, search direction is going away from ao
		if (glm::dot(a, dir) < 0) return false;
		simplex.push(a);

		glm::vec3 ao = -simplex.a;

		if (simplex.shape == 2) { // simplex is a line
			glm::vec3 ab = (simplex.b - simplex.a);
			dir = glm::cross(ab, glm::cross(ao, ab)); //next search along the direction perpendicular to the line perp to ab and vector a
			continue;
		}
		else if (simplex.shape == 3) {
			//check on which side of the simplex(triangle) the origin is found
			//search in orthogonal directions to each possible crossing that keeps O in front of it
			glm::vec3 ab = (simplex.b - simplex.a);
			glm::vec3 ac = (simplex.c - simplex.a);
			glm::vec3 ad = (simplex.d - simplex.a);
			glm::vec3 abc = glm::cross(ab, ac);

			if (glm::dot(glm::cross(ab, abc), ao) > 0) {
				simplex.line(simplex.a, simplex.b);
				dir = glm::cross(ab, glm::cross(ao, ab));
				continue;
			}
			if (glm::dot(glm::cross(abc, ac), ao) > 0) {
				simplex.line(simplex.a, simplex.c);
				dir = glm::cross(ac, glm::cross(ao, ac));
				continue;
			}
			//origin normal to abc plane intersects the triangle
			if (glm::dot(abc, ao) >0) {//determine if above
				dir = abc;
				continue;
			}
			//else it's below, search direction behind abc
			simplex.triangle(simplex.a, simplex.c, simplex.b);
			dir = -abc;
			continue;

		}
		else {
			//For a tetrahedron, check if the origin is outside it
			//by checking on which side of the triangles(except the base bcd) it is
			//if for one face it's on the side pointing away from the other two, check the face, jmp_face
			; {
				glm::vec3 ab = (simplex.b - simplex.a);
				glm::vec3 ac = (simplex.c - simplex.a);

				//check O in front of abc, to keep it in the simplex
				if (glm::dot(glm::cross(ab, ac), ao) > 0) {
					goto jmp_face;
				}

				glm::vec3 ad = (simplex.d - simplex.a);
				//check O in front of acd, to keep it in the simplex
				if (glm::dot(glm::cross(ac, ad), ao) > 0) {
					simplex.triangle(simplex.a, simplex.c, simplex.d);
					goto jmp_face;
				}
				//check O in front of adb, to keep it in the simplex
				if (glm::dot(glm::cross(ad, ab), ao) > 0) {
					simplex.triangle(simplex.a, simplex.d, simplex.b);
					goto jmp_face;
				}
				break; //ORIGIN INSIDE TETRAHEDRON, exit and return TRUE
			}

		jmp_face:
			//update the search direction in the domain reduced by what we checked above
			glm::vec3 ab = (simplex.b - simplex.a);
			glm::vec3 ac = (simplex.c - simplex.a);
			glm::vec3 abc = glm::cross(ab, ac);

			if (glm::dot(glm::cross(ab, abc), ao) > 0) {
				simplex.line(simplex.a, simplex.b);
				dir = glm::cross(ab, glm::cross(ao, ab));
				continue;
			}
			if (glm::dot(glm::cross(abc, ac), ao) > 0) {
				simplex.line(simplex.a, simplex.c);
				dir = glm::cross(ac, glm::cross(ao, ac));
				continue;
			}
			//else O in front of abc
			simplex.triangle(simplex.a, simplex.b, simplex.c);
			dir = abc;
			continue;
		}
	}
	return true;
}