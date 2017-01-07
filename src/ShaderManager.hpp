#pragma once

#include "Shader.hpp"
#include "Geometry.hpp"
#include <map>
#include <vector>
#include <string>

namespace R308
{
	
class ShaderManager
{
public:
  static ShaderManager *getInstance();
  ~ShaderManager();
  uint addShader(std::string const& filename, uint const& geoID);
  void addUniform(uint const& progID, std::string const& name, uint const& size, uint const& length);
  void registerGeometry(uint const& geoID, uint const& shID);

private:
  static ShaderManager *instance;
  ShaderManager() {}
  std::map<uint, Shader> programs;
  std::map<std::string, size_t> uniforms;
  std::map<uint, std::vector<uint>> geoMap;
};

} // end R308 namespace