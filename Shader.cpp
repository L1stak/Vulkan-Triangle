#include "Shader.h"
#include <fstream>

std::vector<char> Shader::ReadShader(const char* filename) {
	std::ifstream file(filename,std::ios::ate | std::ios::binary);

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	
	
	return buffer;
}