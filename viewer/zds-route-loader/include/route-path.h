//------------------------------------------------------------------------------
//
//      Tracks loader for ZDSimulator routes
//      (c) maisvendoo, 26/11/2018
//
//------------------------------------------------------------------------------
/*!
 * \file
 * \brief Tracks loader for ZDSimulator routes
 * \copyright maisvendoo
 * \author maisvendoo
 * \date 26/11/2018
 */

#ifndef     ROUTE_PATH_H
#define     ROUTE_PATH_H

#include    "abstract-path.h"

#include    <fstream>

#include    <osgDB/FileUtils>
#include    <osgDB/FileNameUtils>

#include    "track.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class RoutePath : public MotionPath
{
public:

    /// Constructor with automatic track loading
    RoutePath(const std::string &track_file_path, int direction = 1);

    /// Get cartesian position of track point
    osg::Vec3 getPosition(float railway_coord);

    /// Get cartesian position and camera's attitude
    osg::Vec3 getPosition(float railway_coord, osg::Vec3 &attitude);

    /// Get cartesian position and local basis
    virtual osg::Vec3 getPosition(float railway_coord, basis_t &basis);

    /// Get cartesian position, camera's attitude and local basis
    virtual osg::Vec3 getPosition(float railway_coord, osg::Vec3 &attitude, basis_t &basis);

protected:

    /// Direction
    int direction;

    /// Tracks data
    std::vector<track_t>    track_data;

    /// Load tracks from file *.trk
    bool load(const std::string &track_file_path);

    /// Load tracks from input stream
    bool load(std::istream &stream);

    /// Get line from input stream
    std::string getLine(std::istream &stream) const;

    /// Find track for some railway coordinate
    track_t findTrack(float railway_coord, track_t &next_track);
};

#endif // ROUTE_PATH_H
