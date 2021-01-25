//------------------------------------------------------------------------------
//
//      Functions for loading external model of vehicle
//      (c) maisvendoo, 24/12/2018
//
//------------------------------------------------------------------------------
/*!
 * \file
 * \brief Functions for loading external model of vehicle
 * \copyright maisvendoo
 * \author maisvendoo
 * \date 24/12/2018
 */

#include    "vehicle-loader.h"

#include    "config-reader.h"
#include    "filesystem.h"

#include    <osgDB/FileUtils>
#include    <osgDB/FileNameUtils>
#include    <osgDB/ReadFile>
#include    <osg/Texture2D>
#include    <osg/Material>
#include    <osgUtil/SmoothingVisitor>
#include    <osg/Depth>

#include    <osg/BlendFunc>
#include    <osg/AlphaFunc>

#include    "model-smooth.h"
#include    "texture-filtering.h"
#include    "model-texturing.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
osg::Node *loadModel(const std::string &modelName, const std::string &textureName)
{
    FileSystem &fs = FileSystem::getInstance();

    osg::ref_ptr<osg::Node> model;

    // Loading 3D-model from file
    std::string model_path = fs.combinePath(fs.getVehicleModelsDir(), modelName);
    std::string modelPath = osgDB::findDataFile(model_path);

    if (!modelPath.empty())
    {
        modelPath = fs.toNativeSeparators(modelPath);
        model = osgDB::readNodeFile(modelPath);
    }
    else
    {
        OSG_FATAL << "ERROR: model " << model_path << " is't found";
        return nullptr;
    }

    if (!model.valid())
    {
        OSG_FATAL << "ERROR: model " << model_path << " loading failed";
        return nullptr;
    }    

    ModelTextureFilter texfilter;
    model->accept(texfilter);

    if (!textureName.empty())
    {
        ModelTexturing mt(textureName);
        mt.setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
        model->accept(mt);
    }

    osg::StateSet *ss = model->getOrCreateStateSet();

    // Set blend function for model
    osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,
                                                                osg::BlendFunc::ONE_MINUS_SRC_ALPHA);    
    ss->setAttributeAndModes(blendFunc.get());
    ss->setMode(GL_BLEND, osg::StateAttribute::ON);
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    // Set alpha function for model
    osg::ref_ptr<osg::AlphaFunc> alphaFunc = new osg::AlphaFunc(osg::AlphaFunc::GEQUAL, 0.6f);
    ss->setAttributeAndModes(alphaFunc.get());
    ss->setMode(GL_ALPHA_TEST, osg::StateAttribute::ON);

    ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 0.0, 1.0));

    return model.release();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
osg::Group *loadVehicle(const std::string &configPath)
{
    // Group node for vehicle model loading
    osg::ref_ptr<osg::Group> group = new osg::Group;

    // Open vehicle config file
    FileSystem &fs = FileSystem::getInstance();
    std::string relative_config_path = configPath + fs.separator() + configPath + ".xml";
    std::string cfg_path = fs.combinePath(fs.getVehiclesDir(), relative_config_path);
    ConfigReader cfg(cfg_path);

    std::string modelName = "";
    std::string textureName = "";
    std::string modelShift = "";

    osg::Vec3 shift(0.0, 0.0, 0.0);

    // Reading data about body's 3D-model and texture
    if (cfg.isOpenned())
    {
        std::string secName = "Vehicle";
        cfg.getValue(secName, "ExtModelName", modelName);
        cfg.getValue(secName, "ExtTexturesDir", textureName);

        if (cfg.getValue(secName, "ModelShift", modelShift))
        {
            std::istringstream ss(modelShift);
            ss >> shift.x() >> shift.y() >> shift.z();
        }
    }

    osg::ref_ptr<osg::MatrixTransform> transShift = new osg::MatrixTransform(osg::Matrix::translate(shift));
    osg::ref_ptr<osg::Node> model = loadModel(modelName, textureName);

    if (model.valid())
    {
        transShift->addChild(model.get());
    }    

    group->addChild(transShift.get());

    return group.release();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void loadCabine(osg::Group *vehicle,
                const std::string &config_name,
                osg::ref_ptr<osg::Node> &cabine_model)
{
    // Calculate vehicle config path
    FileSystem &fs = FileSystem::getInstance();
    std::string relative_cfg_path = config_name + fs.separator() + config_name + ".xml";
    std::string cfg_path = fs.combinePath(fs.getVehiclesDir(), relative_cfg_path);

    // Load config file
    ConfigReader cfg(cfg_path);

    if (!cfg.isOpenned())
    {
        OSG_FATAL << "Vehicle config " << cfg_path << " is't foung" << std::endl;
        return;
    }

    std::string cabineModelName = "";
    std::string cabineTextureName = "";
    std::string cabineShift = "";

    osg::Vec3 shift(0.0, 0.0, 0.0);

    if (cfg.isOpenned())
    {
        std::string secName = "Vehicle";
        cfg.getValue(secName, "CabineModel", cabineModelName);
        cfg.getValue(secName, "CabineTexturesDir", cabineTextureName);

        if (cabineModelName.empty())
            return;

        if (cfg.getValue(secName, "CabineShift", cabineShift))
        {
            std::istringstream ss(cabineShift);
            ss >> shift.x() >> shift.y() >> shift.z();
        }
    }

    osg::ref_ptr<osg::MatrixTransform> transShift = new osg::MatrixTransform(osg::Matrix::translate(shift));
    cabine_model = loadModel(cabineModelName, cabineTextureName);

    if (cabine_model.valid())
        transShift->addChild(cabine_model.get());

    vehicle->addChild(transShift.get());    
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float getLength(const std::string &config_name)
{
    float length = 16.0f;

    // Calculate vehicle config path
    FileSystem &fs = FileSystem::getInstance();
    std::string relative_cfg_path = config_name + fs.separator() + config_name + ".xml";
    std::string cfg_path = fs.combinePath(fs.getVehiclesDir(), relative_cfg_path);

    // Load config file
    ConfigReader cfg(cfg_path);

    if (!cfg.isOpenned())
    {
        OSG_FATAL << "Vehicle config " << cfg_path << " is't foung" << std::endl;
        return length;
    }


    std::string secName = "Vehicle";
    cfg.getValue(secName, "Length", length);

    return length;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
osg::Vec3 getDirverPosition(const std::string &config_name)
{
    osg::Vec3 position(0.9255f, 9.0172f, 3.75f);

    // Calculate vehicle config path
    FileSystem &fs = FileSystem::getInstance();
    std::string relative_cfg_path = config_name + fs.separator() + config_name + ".xml";
    std::string cfg_path = fs.combinePath(fs.getVehiclesDir(), relative_cfg_path);

    // Load config file
    ConfigReader cfg(cfg_path);

    if (!cfg.isOpenned())
    {
        OSG_FATAL << "Vehicle config " << cfg_path << " is't foung" << std::endl;
        return position;
    }

    std::string secName = "Vehicle";
    std::string tmp;
    cfg.getValue(secName, "DriverPos", tmp);

    std::istringstream ss(tmp);
    ss >> position.x() >> position.y() >> position.z();

    return position;
}
