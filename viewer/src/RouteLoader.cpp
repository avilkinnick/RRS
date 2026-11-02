#include "RouteLoader.h"

#include "CfgReader.h"
#include "Logger.h"
#include "Route.h"
// #include "parse_file_funcs.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vsg/maths/vec3.h>

#define LABEL_BUFFER_SIZE 256
#define RELATIVE_PATH_BUFFER_SIZE 512
#define FLOAT_BUFFER_SIZE 32

bool is_slash(char ch)
{
    return ch == '/' || ch == '\\';
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
RouteLoader::RouteLoader(const std::string& route_path)
    : route_path(route_path)
{
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void RouteLoader::read_description()
{
    QString tmp_qstr = (route_path + "/description.xml").c_str();

    CfgReader cfg;
    cfg.load(tmp_qstr);

    QString sec_name = "Route";

    tmp_qstr = "";
    if (cfg.getString(sec_name, "RouteType", tmp_qstr))
        route_type = tmp_qstr.toStdString();

    tmp_qstr = "";
    if (cfg.getString(sec_name, "ObjectsRefPath", tmp_qstr))
        objects_ref_path = tmp_qstr.toStdString();

    tmp_qstr = "";
    if (cfg.getString(sec_name, "RouteMapPath", tmp_qstr))
        route_map_path = tmp_qstr.toStdString();

    objects_ref_path = route_path + objects_ref_path;
    route_map_path = route_path + route_map_path;
}

