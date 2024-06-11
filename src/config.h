#pragma once

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>



// #define ROBOCONF_COMMON_VECTOR_RESERVE_SIZE      0
#define ROBOCONF_COMMON_VECTOR_RESERVE_SIZE      1024

#define ROBOCONF_ALL_NETS_USE_UNORDERED_MAP

#define ROBOCONF_COMPONENT_PIN_INFO_USE_UNORDERED_SET

#define ROBOCONF_COMPONENT_INFO_PINS_USE_UNORDERED_MAP

#define ROBOCONF_COMPONENT_CMP_LIST_TOKENS_USE_UNORDERED_MAP

#define ROBOCONF_NETLIST_COMPONENT_ATTRIBUTES_MAP_USE_UNORDERED_MAP

#define ROBOCONF_COMPONENT_CLASS_NAME_MAPS_USE_UNORDERED_MAP

#define ROBOCONF_NET_CLASSES_USE_UNORDERED_MAP

#define ROBOCONF_CONNECTION_USE_UNORDERED /* map/set */

#define ROBOCONF_STRING_STRING_MAP_TYPE_USE_UNORDERED_MAP

#define ROBOCONF_STRING_STRING_VECTOR_MAP_TYPE_USE_UNORDERED_MAP

#define ROBOCONF_PARSE_READ_LIST_EXPRESSION_LIST_STACK_USE_UNDERLYING_VECTOR

#define ROBOCONF_PACKAGES_DB_USE_UNORDERED_MAP


//----------------------------------------------------------------------------
// Оптимизация readList

// Mutual exlusive options
////#define ROBOCONF_READ_LIST_USE_MOVE_SEMANTICS
//#define ROBOCONF_READ_LIST_USE_MOVE_SEMANTICS_FORCE_MOVE

//#define ROBOCONF_READ_LIST_USE_FORCE_INLINE
//#define ROBOCONF_READ_LIST_USE_FORCE_FLATTEN
//#define ROBOCONF_READ_LIST_USE_EMPLACE_BACK
#define ROBOCONF_READ_LIST_EXPRESSIONS_PUSH_BACK_AS_FUNCTIONS   /* В итоге только это неплохо подняло производительность */

// Вот это просаживает производительность
// #define ROBOCONF_EXPRESSION_LIST_RESERVE


//----------------------------------------------------------------------------





// // unordered_set не работает в качестве ключа для map (нет operator< для std::set). В 20м стандарте вроде убирают эту возможность вообще.
// // #define ROBOCONF_STRING_SET_TYPE_USE_UNORDERED_SET







/*
component.cpp
component_class.cpp
cube2cmp.cpp
cube2cmp_bak.cpp
encoding.cpp
packages.cpp
roboconf.cpp
roboconf_options.cpp
component.h
component_alias_db.h
component_class.h
component_handlers.h
connections.h
cubeXmlParsing.h
detail_levels.h
encoding.h
file_set.h
icUtils.h
interfaces.h
list_conditional.h
net.h
netlist.h
netlist_src_type.h
packages.h
parse.h
parse_good_01.h
regexp.h
report_generic.h
report_mcu.h
report_nets.h
report_periph.h
report_summary.h
reports.h
roboconf_options.h
*/
