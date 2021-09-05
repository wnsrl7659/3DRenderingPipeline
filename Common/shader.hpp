#ifndef SHADER_HPP
#define SHADER_HPP

#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <iostream>

enum In_Primitive_Enum
{
	InPoints = 0,
	InLines,
	InTriangles
};

enum Out_Primitive_Enum
{
    OutTriangleStrip = 0,
    OutPoints,
    OutLineStrip
};

class Shader
{
public:
	GLuint ID;

	// constructor generates the shader on the fly
	Shader(const char* vertex_file_path, const char* fragment_file_path);
	Shader(const char* vertex_file_path, const char* fragment_file_path, GLuint* programIDs);
	// Load shaders where multiple shader files == one complete shader
	// Show how code can be reused across shaders
	Shader(const char* vertex_file_path, const char* fragment_file_path,
		const char* geom_file_path, In_Primitive_Enum in_primitive_type, Out_Primitive_Enum  out_primitive_type, GLuint max_vertieces);


	// activate the shader
	inline void use() {glUseProgram(ID);}
	// utility uniform functions
	inline void setBool(const std::string& name, bool value) const
		{glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);}
	inline void setInt(const std::string& name, int value) const
		{glUniform1i(glGetUniformLocation(ID, name.c_str()), value);}
	inline void setFloat(const std::string& name, float value) const
		{glUniform1f(glGetUniformLocation(ID, name.c_str()), value);}
	inline void setVec2(const std::string& name, const glm::vec2& value) const
		{glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);}
	inline void setVec2(const std::string& name, float x, float y) const
		{glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);}
	inline void setVec3(const std::string& name, const glm::vec3& value) const
		{glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);}
	inline void setVec3(const std::string& name, float x, float y, float z) const
		{glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);}
	inline void setVec4(const std::string& name, const glm::vec4& value) const
		{glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);}
	inline void setVec4(const std::string& name, float x, float y, float z, float w)
		{glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);}
	inline void setMat2(const std::string& name, const glm::mat2& mat) const
		{glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);}
	inline void setMat3(const std::string& name, const glm::mat3& mat) const
		{glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);}
	inline void setMat4(const std::string& name, const glm::mat4& mat) const
		{glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);}

private:
	// utility function for checking shader compilation/linking errors.
	void checkCompileErrors(unsigned int shader, std::string type);
};


#endif // SHADER_HPP
