#ifndef     SCREEN_CAPTURE_H
#define     SCREEN_CAPTURE_H

#include    <osgViewer/Viewer>
#include    <osgViewer/ViewerEventHandlers>
#include    <osgDB/WriteFile>
#include    <osgGA/GUIEventHandler>

#include    <QDir>
#include    <QDateTime>
#include    <QString>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class WriteToFileOperation : public osgViewer::ScreenCaptureHandler::CaptureOperation
{
public:

    WriteToFileOperation(const std::string &screenshotsDir);


    virtual void operator()(const osg::Image &image, const unsigned int context_id);


private:

    std::string screenshotsDir;
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class ScreenCaptureHandler : public osgGA::GUIEventHandler
{
public:

    ScreenCaptureHandler(osgViewer::ScreenCaptureHandler *sch);


    virtual bool handle(const osgGA::GUIEventAdapter &ea,
                        osgGA::GUIActionAdapter &aa);



private:

    osgViewer::ScreenCaptureHandler *sch;
};

#endif // SCREEN_CAPTURE_H
