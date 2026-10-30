#ifndef EDITOR_OBJECT_REF_H
#define EDITOR_OBJECT_REF_H

#include <vsg/core/ref_ptr.h>

#include <string>

namespace vsg
{

class PagedLOD;

}

struct ObjectRef
{
    std::string relative_path;
    vsg::ref_ptr<vsg::PagedLOD> paged_lod;
};

#endif // EDITOR_OBJECT_REF_H
