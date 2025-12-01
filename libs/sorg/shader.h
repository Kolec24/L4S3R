#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>

class Shader
{
  public:
    Shader() = default;
    Shader(const char *vertexPath, const char *fragmentPath);

    void use();
    void setBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void Set4Float(const std::string &name, float value1, float value2,
                   float value3, float value4) const;

  private:
    unsigned int ID;
};

#endif
