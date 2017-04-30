#include "yuneec_route.h"

#include <fstream>

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

using namespace yuneec;

bool route::load(std::string path)
{
	Json::Value mission;

	std::ifstream mission_file(path, std::ifstream::binary);
	if(mission_file.is_open() == false)
	{
		return false;
	}
	setlocale(LC_ALL, "C");
	mission_file >> mission;

	//std::cout << mission;

	return deserialize(mission);
}

bool route::save(std::string path)
{
	Json::Value mission;
	serialize(mission);

	std::ofstream mission_file(path, std::ofstream::binary);
	if(mission_file.is_open() == false)
	{
		return false;
	}
	mission_file << mission;
	mission_file.close();

	return true;
}
