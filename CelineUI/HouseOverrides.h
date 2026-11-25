#pragma once

/*
    The parts of a plugin's identity that are not colours, and the house values for them.

    Colours are not here: a plugin ships its own values for the shared roles with
    CELINE_PLUGIN_SHARED_ROLE_VALUES in its PluginThemeRoles.h (see ThemeRoles.h). What is
    here is what a theme cannot reach -- the faces the kit sets its text in, and how round
    its corners are -- which the About window, the theme editor and the base look and feel
    all read.

    A plugin that wants them provides `PluginHouseOverrides.h` in its source/ folder, which
    is on the include path of everything the kit compiles. Every macro is optional there,
    and a plugin that does not provide the file gets exactly the house look.
*/

#if __has_include("PluginHouseOverrides.h")
  #include "PluginHouseOverrides.h"
#endif

/** The corner radius of every button, field, popup and panel the kit draws. A plugin
    that is square throughout says 0 here, and the kit's rounded paths come out square
    without any of them being changed. */
#ifndef CELINE_PLUGIN_CORNER_RADIUS
  #define CELINE_PLUGIN_CORNER_RADIUS 8.0f
#endif

/** Asset filenames for the three faces the kit draws its own text in, replacing the house
    Jura and Nico Moji. Mono is not here: it is JetBrains Mono for every plugin, because a
    number has to line up in a column whatever else the plugin looks like. */
#ifndef CELINE_PLUGIN_FONT_LIGHT
  #define CELINE_PLUGIN_FONT_LIGHT "Jura-Light.ttf"
#endif

#ifndef CELINE_PLUGIN_FONT_BOLD
  #define CELINE_PLUGIN_FONT_BOLD "Jura-Bold.ttf"
#endif

#ifndef CELINE_PLUGIN_FONT_LOGO
  #define CELINE_PLUGIN_FONT_LOGO "NicoMoji-Regular.ttf"
#endif
