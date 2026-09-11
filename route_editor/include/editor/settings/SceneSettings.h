#ifndef EDITOR_SETTINGS_SCENE_SETTINGS_H
#define EDITOR_SETTINGS_SCENE_SETTINGS_H

class CfgReader;

struct scene_settings_t
{
    int num_lights;

    scene_settings_t();
    void read(CfgReader& cfg);
};

#endif // EDITOR_SETTINGS_SCENE_SETTINGS_H
