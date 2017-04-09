#pragma once

#include <jsoncpp/json/value.h>

class json_serializable
{
public:
	/*constructor*/				json_serializable			()
	{
		//
	}

	virtual /*destructor*/		~json_serializable			()
	{
		//
	}

	virtual bool				serialize					(Json::Value &v) const = 0;
	virtual bool				deserialize					(const Json::Value &v) = 0;
};
