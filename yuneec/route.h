#pragma once

#include <sstream>

#include "tree/node.h"
#include "json_serializable.h"

#include "waypoint.h"
#include "interest_point.h"

namespace yuneec
{

class route : public node, public json_serializable
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
	property_value<int> end_action;
	property_value<double> height;
	property_value<int> height_type;
	property_value<int> pan_mode;
	property_value<int> route_type;
	property_value<double> speed;
	property_value<int> tilt_mode;


	/*constructor*/				route				() : end_action("endAction")
														, height("height")
														, height_type("heightType")
														, pan_mode("panMode")
														, route_type("routeType")
														, speed("speed")
														, tilt_mode("tiltMode")
	{
		add_property(&end_action);
		add_property(&height);
		add_property(&height_type);
		add_property(&pan_mode);
		add_property(&route_type);
		add_property(&speed);
		add_property(&tilt_mode);
	}

	/*destructor*/				~route				()
	{
		//
	}

	bool						load				(std::string path)
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

	bool						save				(std::string path)
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

	bool						serialize			(Json::Value &v) const
	{
		const node *n_wp = at("waypoints");
		const node *n_ip = at("interestPoints");
		if(n_wp == NULL || n_ip == NULL)
		{
			return false;
		}

		Json::Value &interest_points = v["interestPoints"];
		interest_points.resize(0);

		save_node(v["route"]);

		Json::Value &waypoints = v["waypoints"];
		waypoints.resize(0);

		for(auto wp_name : n_wp->ls())
		{
			const waypoint *wp = dynamic_cast<const waypoint *>(n_wp->at(wp_name));
			if(wp == nullptr)
			{
				continue;
			}
			Json::Value wp_v;
			wp->serialize(wp_v);

			waypoints.append(wp_v);
		}

		return true;
	}

	bool						deserialize			(const Json::Value &mission)
	{
		const Json::Value &jroute = mission["route"];
		const Json::Value &jwaypoints = mission["waypoints"];
		const Json::Value &jinterest_points = mission["interestPoints"];
		if(jroute.isNull() == true || jwaypoints.isNull() == true || jinterest_points.isNull())
		{
			return -1;
		}

		for(auto prop_name : jroute.getMemberNames())
		{
			property_base *pp = get_property(prop_name);

			if(pp == NULL)
			{
				continue;
			}

			const Json::Value &wp_prop = jroute[prop_name];

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

		std::stringstream name_render;

		node *n_waypoints = append("waypoints");
		for(Json::ArrayIndex i = 0 ; i < jwaypoints.size() ; i += 1)
		{
			name_render.str(std::string());
			name_render << "waypoint " << i;
			waypoint *wp = new waypoint;
			wp->deserialize(jwaypoints[i]);
			printf("deser %f %f\n", wp->latitude.get_value(), wp->longitude.get_value());
			n_waypoints->attach(name_render.str(), wp);
		}

		node *n_interest_points = append("interestPoints");
		for(Json::ArrayIndex i = 0 ; i < jinterest_points.size() ; i += 1)
		{
			name_render.str(std::string());
			name_render << "interest point " << i;
			interest_point *ip = new interest_point;
			ip->deserialize(jinterest_points[i]);
			n_interest_points->attach(name_render.str(), ip);
		}
		return 0;
	}
};

}
