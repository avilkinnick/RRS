#ifndef ROUTE_READER_OBJECTS_REF_H
#define ROUTE_READER_OBJECTS_REF_H

#include <map>
#include <string>

class ObjectsRef : public std::map<std::string, std::string>
{
public:
    using Label = std::string;
    using RelPath = std::string;

    bool read(const char* path);
};

#endif // ROUTE_READER_OBJECTS_REF_H
