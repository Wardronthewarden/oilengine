#include "pch/oilpch.h"
#include "OpenGLShader.h"

#include <fstream>

#include <GLAD/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace oil{

static GLenum ShaderTypeFromString(const std::string& type){
	if (type == "vertex")
		return GL_VERTEX_SHADER;
	if (type == "fragment" || type == "pixel")
		return GL_FRAGMENT_SHADER;
	if (type == "geometry")
		return GL_GEOMETRY_SHADER;

	OIL_CORE_ASSERT(false, "Unknown shader type!")
	return 0;
}

static ShaderDomain ShaderDomainFromString(const std::string& domain){
	if (domain == "surface")
		return ShaderDomain::Surface;
	if (domain == "post_process")
		return ShaderDomain::PostProcess;
	if (domain == "engine")
		return ShaderDomain::Engine;
	if (domain == "none")
		return ShaderDomain::None;

	OIL_CORE_ASSERT(false, "Unknown shader domain!")
	return ShaderDomain::None;
}

static ShaderModel ShaderModelFromString(const std::string& model){
	if (model == "lit")
		return ShaderModel::Lit;
	if (model == "unlit")
		return ShaderModel::Unlit;
	if (model == "none")
		return ShaderModel::None;

	OIL_CORE_ASSERT(false, "Unknown shader model!")
	return ShaderModel::None;
}

OpenGLShader::OpenGLShader(const std::string &filepath)
	: m_Path(filepath)
{
	std::string src = ReadFile(filepath);
	auto shaderSources = PreProcess(src);

	Compile(shaderSources);

	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	auto lastDot = filepath.rfind('.');	
	auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string &vertexSrc, const std::string &fragmentSrc)
	: m_Name(name)
{
	std::unordered_map<GLenum, std::string> sources;
	sources[GL_VERTEX_SHADER] = vertexSrc;
	sources[GL_FRAGMENT_SHADER] = fragmentSrc;

	Compile(sources);
}

OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(m_RendererID);
}

void OpenGLShader::Bind() const
{
    glUseProgram(m_RendererID);
}

void OpenGLShader::Unbind() const
{
    glUseProgram(0);
}

void OpenGLShader::Recompile(std::string srcPath)
{
	m_Path = srcPath;
	if(!m_Path.empty()){
		std::string src = ReadFile(m_Path);
		auto shaderSources = PreProcess(src);

		Compile(shaderSources);
	}
}

void OpenGLShader::SetInt(const std::string& name, const int& value){
	UploadUniformInt(name, value);
}

void OpenGLShader::SetIntArray(const std::string &name, int *values, uint32_t count)
{
	UploadUniformIntArray(name, values, count);
}

void OpenGLShader::SetFloat(const std::string &name, const float &value)
{
	UploadUniformFloat(name, value);
}

void OpenGLShader::SetFloat2(const std::string &name, const glm::vec2 &value)
{
	UploadUniformFloat2(name, value);
}

void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value) {
	UploadUniformFloat3(name, value);
}
void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value) {
	UploadUniformFloat4(name, value);
}
void OpenGLShader::SetMat3(const std::string &name, const glm::mat3 &value)
{
	UploadUniformMat3(name, value);
}
void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &value)
{
    UploadUniformMat4(name, value);
}

int OpenGLShader::GetInt(const std::string &name)
{
	int ret;
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glGetUniformiv(m_RendererID, location, &ret);
    return ret;
}

std::vector<int> OpenGLShader::GetIntArray(const std::string &name)
{
    return std::vector<int>();
}

float OpenGLShader::GetFloat(const std::string &name)
{
    float ret;
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glGetUniformfv(m_RendererID, location, &ret);
    return ret;
}

glm::vec2 OpenGLShader::GetFloat2(const std::string &name)
{
    glm::vec2 ret;
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glGetUniformfv(m_RendererID, location, glm::value_ptr(ret));
	return ret;
}

glm::vec3 OpenGLShader::GetFloat3(const std::string &name)
{
    glm::vec3 ret;
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glGetUniformfv(m_RendererID, location, glm::value_ptr(ret));
	return ret;
}

glm::vec4 OpenGLShader::GetFloat4(const std::string &name)
{
    glm::vec4 ret;
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glGetUniformfv(m_RendererID, location, glm::value_ptr(ret));
	return ret;
}

glm::mat3 OpenGLShader::GetMat3(const std::string &name)
{
    glm::mat3 ret;
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glGetUniformfv(m_RendererID, location, glm::value_ptr(ret));
	return ret;
}

glm::mat4 OpenGLShader::GetMat4(const std::string &name)
{
    glm::mat4 ret;
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glGetUniformfv(m_RendererID, location, glm::value_ptr(ret));
	return ret;
}

std::vector<ShaderUniform> OpenGLShader::GetUniformNames() const
{
	std::vector<ShaderUniform> ret;
	GLint count, size;
	GLenum type;
	glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &count);

	const GLsizei bufSize = 16;
	GLchar name[bufSize];
	GLsizei length;

	for(int i = 0; i < count; ++i){
		glGetActiveUniform(m_RendererID, (GLuint)i, bufSize, &length, &size, &type, name);
		ret.push_back({GlTypeToUniformType(type), std::string(name)});
	}
    return ret;
}

UniformType OpenGLShader::GlTypeToUniformType(GLenum type) const
{
	switch (type){
		case GL_FLOAT: 			return UniformType::Float;
		case GL_FLOAT_VEC2: 	return UniformType::Float2;
		case GL_FLOAT_VEC3: 	return UniformType::Float3;
		case GL_FLOAT_VEC4: 	return UniformType::Float4;
		
		case GL_FLOAT_MAT3: 	return UniformType::Mat3x3;
		case GL_FLOAT_MAT4: 	return UniformType::Mat4x4;

		case GL_SAMPLER_2D: 	return UniformType::Texture2D;

		case GL_INT: 			return UniformType::Int;

		default: return UniformType::None;	
	}
	
}

void OpenGLShader::UploadUniformMat3(const std::string &name, const glm::mat3 &matrix)
{ 
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformMat4(const std::string &name, const glm::mat4 &matrix)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformInt(const std::string &name, int value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(const std::string &name, int *values, uint32_t count)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform1iv(location, count, values);
}

void OpenGLShader::UploadUniformFloat(const std::string &name, float value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(const std::string &name, const glm::vec2 &value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform2f(location, value.x, value.y);
}

void OpenGLShader::UploadUniformFloat3(const std::string &name, const glm::vec3 &value)
{
    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::UploadUniformFloat4(const std::string &name, const glm::vec4 &value)
{
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::Compile(std::unordered_map<GLenum, std::string> &shaderSources)
{
	GLuint program = glCreateProgram();
	const size_t shaderNum = shaderSources.size();
	OIL_CORE_ASSERT(shaderNum <= 3, "We only support 3 shaders!");
	std::array<GLenum, 3> glShaderIDs;
	int glShaderIDIndex = 0;
	for (auto& kv : shaderSources){
		GLenum shaderType = kv.first;
		std::string& source = kv.second;

		// Create an empty vertex shader handle
		GLuint shader = glCreateShader(shaderType);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar *sourceCStr = (const GLchar *)source.c_str();
		glShaderSource(shader, 1, &sourceCStr, 0);

		// Compile the vertex shader
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
			
			// We don't need the shader anymore.
			glDeleteShader(shader);

			// Use the infoLog as you see fit.

			OIL_CORE_ERROR("{0}", infoLog.data());
			OIL_CORE_ASSERT(false, "Shader compilation failure!");

			
			// In this simple program, we'll just leave
			break;
		}
	glAttachShader(program, shader);
	glShaderIDs[glShaderIDIndex++] = shader;

	}

	
	// Vertex and fragment shaders are successfully compiled.
	// Now time to link them together into a program.
	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		
		// We don't need the program anymore.
		glDeleteProgram(program);

		// Don't leak shaders either.
		for (auto id : glShaderIDs)
			glDeleteShader(id);

		// Use the infoLog as you see fit.
		OIL_CORE_ERROR("{0}", infoLog.data());
		OIL_CORE_ASSERT(false, "Shader link failure!");
		// In this simple program, we'll just leave
		return;
	}

	// Always detach shaders after a successful link.
	for (auto id : glShaderIDs)
		glDetachShader(program, id);

	m_RendererID = program;
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string &source)
{
    std::unordered_map<GLenum, std::string> shaderSources;
	size_t pos = 0;

	//get shader domain
	const char* domainToken = "#domain";
	size_t domainTokenLength = strlen(domainToken);
	pos = source.find(domainToken, 0);
	if(pos != std::string::npos){
		size_t eol = source.find_first_of("\r\n", pos);
		OIL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin = pos + domainTokenLength + 1;
		std::string domain = source.substr(begin, eol - begin);
		m_ShaderDomain = ShaderDomainFromString(domain); 
		if(!(uint32_t)m_ShaderDomain)
			OIL_CORE_WARN("Invalid shader domain specified!");
	}

	//get shading model
	const char* shModelToken = "#model";
	size_t shModelTokenLength = strlen(shModelToken);
	pos = source.find(shModelToken, 0);
	if(pos != std::string::npos){
		size_t eol = source.find_first_of("\r\n", pos);
		OIL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin = pos + shModelTokenLength + 1;
		std::string model = source.substr(begin, eol - begin);
		m_ShaderModel = ShaderModelFromString(model); 
		if(!(uint32_t)m_ShaderModel)
			OIL_CORE_WARN("Invalid shader model specified!");
	}

	//process shader source for each type
	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	pos = source.find(typeToken, 0);
	while (pos != std::string::npos){
		size_t eol = source.find_first_of("\r\n", pos);
		OIL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin = pos + typeTokenLength + 1;
		std::string type = source.substr(begin, eol - begin);
		OIL_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified!");

		size_t nextLinePos = source.find_first_not_of("\r\n", eol);
		pos = source.find(typeToken, nextLinePos);
		shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() -1 : nextLinePos));
	}

	return shaderSources;
}

std::string OpenGLShader::ReadFile(const std::string &filepath)
{
	m_Path = filepath;
    std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in){
		in.seekg(0, std::ios::end);
		result.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	} else {
		OIL_CORE_ERROR("Could not open file '{0}'", filepath);
	}

	return result;
}
}