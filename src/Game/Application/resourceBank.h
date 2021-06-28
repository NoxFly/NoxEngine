#ifndef RESOURCEBANK_H
#define RESOURCEBANK_H

#include <string>

#include "ResourceHolder.hpp"
#include "Shader.h"
#include "Texture.h"

extern ResourceHolder<Shader, std::string> shadersBank;
extern ResourceHolder<Texture, std::string> texturesBank;

#endif // RESOURCEBANK_H