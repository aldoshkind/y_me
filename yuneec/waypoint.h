#pragma once

#include "tree/node.h"
#include "json_serializable.h"

namespace yuneec
{

class waypoint : public node, public json_serializable
{
	void save_node(Json::Value &val) const
	{
		for(auto prop : get_properties())
		{
			//std::cout << prop->get_type() << " " << prop->get_name() << std::endl;
			if(prop->get_type() == typeid(double).name())
			{
				property_value<double> *pd = dynamic_cast<property_value<double> *>(prop);
				val[prop->get_name()] = pd->get_value();
			}
			else if(prop->get_type() == typeid(int).name())
			{
				property_value<int> *pd = dynamic_cast<property_value<int> *>(prop);
				val[prop->get_name()] = pd->get_value();
			}
			else if(prop->get_type() == typeid(std::string).name())
			{
				property_value<std::string> *pd = dynamic_cast<property_value<std::string> *>(prop);
				val[prop->get_name()] = pd->get_value();
			}
		}
	}

public:
	property_value<double>		latitude;
	property_value<double>		longitude;
	property_value<double>		pan_angle;
	property_value<double>		tilt_angle;
	property_value<int>			interest_point_index;
	property_value<int>			end_action;
	property_value<double>		height;
	property_value<int>			height_type;
	property_value<int>			pan_mode;
	property_value<int>			route_type;
	property_value<double>		speed;
	property_value<int>			tilt_mode;

	/*constructor*/				waypoint				() : latitude("latitude")
															, longitude("longitude")
															, pan_angle("panAngle")
															, tilt_angle("tiltAngle")
															, interest_point_index("interestPointIndex")
															, end_action("endAction")
															, height("height")
															, height_type("heightType")
															, pan_mode("panMode")
															, route_type("routeType")
															, speed("speed")
															, tilt_mode("tiltMode")
	{
		add_property(&latitude);
		add_property(&longitude);
		add_property(&pan_angle);
		add_property(&tilt_angle);
		add_property(&interest_point_index);
		add_property(&end_action);
		add_property(&height);
		add_property(&height_type);
		add_property(&pan_mode);
		add_property(&route_type);
		add_property(&speed);
		add_property(&tilt_mode);
	}

	/*destructor*/				~waypoint				()
	{
		//
	}

	bool						serialize			(Json::Value &v) const
	{
		v["waypointTasks"].resize(0);
		save_node(v);
		return false;
	}

	bool						deserialize				(const Json::Value &v)
	{
		for(auto prop_name : v.getMemberNames())
		{
			property_base *pp = get_property(prop_name);

			if(pp == NULL)
			{
				continue;
			}

			const Json::Value &wp_prop = v[prop_name];

			if(pp->get_type() == typeid(double).name())
			{
				if(wp_prop.isConvertibleTo(Json::realValue) == false)
				{
					continue;
				}
				property<double> *p = dynamic_cast<property<double> *>(pp);
				*p = wp_prop.asDouble();
			}

			if(pp->get_type() == typeid(int).name())
			{
				if(wp_prop.isConvertibleTo(Json::intValue) == false)
				{
					continue;
				}
				property<int> *p = dynamic_cast<property<int> *>(pp);
				*p = wp_prop.asInt();
			}
		}
		return true;
	}
};

}
