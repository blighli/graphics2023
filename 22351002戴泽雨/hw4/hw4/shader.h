#pragma once
#include<iostream>
#include<glm/glm.hpp>
namespace Resource {

	struct Shader {
		Shader() : ID(-1){};
		inline auto Use()->Shader&;
		auto Compile(const char * vertexSource,const char * fragmentSource,const char* geometrySource = nullptr)->void;
		auto setFloat(const char*name,float value,bool useShader)->void ;
		auto SetInteger(const char* name, int value, bool useShader = false)->void;
		auto SetVector2f(const char* name, float x, float y, bool useShader = false)->void;
		auto SetVector2f(const char* name, const glm::vec2& value, bool useShader = false)->void;
		auto SetVector3f(const char* name, float x, float y, float z, bool useShader = false)->void;
		auto SetVector3f(const char* name, const glm::vec3& value, bool useShader = false)->void;
		auto SetVector4f(const char* name, float x, float y, float z, float w, bool useShader = false)->void;
		auto SetVector4f(const char* name, const glm::vec4& value, bool useShader = false)->void;
		auto SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader = false)->void;
	private:
		unsigned int ID;
		auto checkCompileErrors(unsigned int object, std::string type)->void;

	};
}