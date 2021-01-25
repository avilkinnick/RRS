//------------------------------------------------------------------------------
//
//      Loading and processing train exterior
//      (c) maisvendoo, 24/12/2018
//
//------------------------------------------------------------------------------
/*!
 * \file
 * \brief Loading and processing train exterior
 * \copyright maisvendoo
 * \author maisvendoo
 * \date 24/12/2018
 */

#ifndef     TRAINE_XTERIOR_H
#define     TRAINE_XTERIOR_H

#include    <QObject>
#include    <QSharedMemory>

#include    <osgGA/GUIEventHandler>
#include    <osg/MatrixTransform>

#include    "abstract-path.h"
#include    "trajectory-element.h"

#include    <osgViewer/Viewer>

#include    "vehicle-exterior.h"
#include    "animation-manager.h"
#include    "camera-position.h"
#include    "settings.h"
#include    "server-data-struct.h"
#include    "config-reader.h"
#include    "display.h"

#include    <queue>

/*!
 * \class
 * \brief Handler of train's exterior
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class TrainExteriorHandler : public QObject, public osgGA::GUIEventHandler
{
    Q_OBJECT

public:

    /// Constructor
    TrainExteriorHandler(settings_t settings, MotionPath *routePath, const std::string &train_config);

    /// Handle method
    virtual bool handle(const osgGA::GUIEventAdapter &ea,
                        osgGA::GUIActionAdapter &aa);

    /// Get exterior scene group
    osg::Group *getExterior();

    std::vector<AnimationManager *> getAnimManagers();

signals:

    void setStatusBar(QString msg);

    void sendCameraPosition(camera_position_t cp);

private:

    settings_t  settings;

    /// Vehicle number? which is a referenced for camera
    int cur_vehicle;

    /// Shift camera along train
    float long_shift;

    /// Shift camera up/down
    float height_shift;

    /// Route path (trajectory) contener
    osg::ref_ptr<MotionPath> routePath;

    /// Train exterior scene group
    osg::ref_ptr<osg::Group> trainExterior;

    /// Time between current and previous frame drawing
    double ref_time;        

    /// Info about train's vehicles exterior
    std::vector<vehicle_exterior_t> vehicles_ext;

    /// Data, received from server
    network_data_t  nd;    

    QSharedMemory   shared_memory;

    /// Animations list
    std::vector<AnimationManager *> anim_managers;

    /// Keyboard handler (camera control)
    void keyboardHandler(int key);

    /// Load train exterior from
    void load(const std::string &train_config);

    /// Moving train along track
    void moveTrain(double ref_time, const network_data_t &nd);

    /// Processing data from server
    void processSharedData(double &ref_time);

    /// Move camera along track
    void moveCamera(osgViewer::Viewer *viewer);

    /// Calculate vehicles attiture
    void recalcAttitude(size_t i);

    /// Load vehicle animations
    void loadAnimations(const std::string vehicle_name, osg::Node *cabine, animations_t &animations);

    void loadModelAnimations(const std::string vehicle_name, osg::Node *model, animations_t &animations);

    void loadDisplays(ConfigReader &cfg, osgDB::XmlNode *vehicle_node, osg::Node *model, displays_t &displays);

    void timerEvent(QTimerEvent *);
};

#endif // TRAIN_EXTERIOR_H
