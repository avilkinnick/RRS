#ifndef ROUTE_READER_ROUTE_MAP_H
#define ROUTE_READER_ROUTE_MAP_H

#include <map>
#include <string>
#include <string_view>
#include <vector>

struct RouteMapTransform
{
    float pos[3]{};
    float rot_deg[3]{};
};

class RouteMap : public std::map<std::string, std::vector<RouteMapTransform>>
{
public:
    using Label = std::string;
    using Transform = RouteMapTransform;

    bool read(std::string_view path);

private:
    void parse_line(const std::string& line);
};

#endif // ROUTE_READER_ROUTE_MAP_H
