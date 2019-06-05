#pragma once

#include <memory>

class Mesh;

class OBJImporter
{
public:

	static std::shared_ptr<Mesh> Import(const char* filepath);

};