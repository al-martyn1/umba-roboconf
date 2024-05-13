#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "pugixml/src/pugixml.hpp"

/*
    	xml_node child(const char_t* name) const;
		xml_attribute attribute(const char_t* name) const;


pugi::xml_attribute attr = node.attribute("id");
    	const char_t* name() const;
		const char_t* value() const;

    	int as_int(int def = 0) const;
		unsigned int as_uint(unsigned int def = 0) const;
		double as_double(double def = 0) const;
		float as_float(float def = 0) const;
    	bool as_bool(bool def = false) const;
*/