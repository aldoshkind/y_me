#pragma once

#include "tree/node.h"
#include "json_serializable.h"

namespace yuneec
{

class interest_point : public node, public json_serializable
{
public:
	/*constructor*/				interest_point			()
	{
		//
	}

	/*destructor*/				~interest_point			()
	{
		//
	}

	bool						serialize			(Json::Value &/*v*/) const
	{
		return false;
	}

	bool						deserialize				(const Json::Value &v)
	{
		return true;
	}
};

}
