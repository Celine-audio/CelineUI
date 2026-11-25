#pragma once

#include "PluginThemeRoles.h"

#define CELINE_SHARED_THEME_ROLES(X)                                                    \
    X (consoleBackground, "Window ground",       "Window",   0xff17151a)                \
    X (background,        "Inset ground",        "Window",   0xff28262e)                \
    X (line,              "Border",              "Window",   0xffd9d9d9)                \
                                                                                        \
    X (headerBackground,  "Header band",         "Header",   0xff3b334b)                \
    X (headerText,        "Logo and icons",      "Header",   0xfff9fbff)                \
                                                                                        \
    X (graphText,         "Labels",              "Graph",    0xffb5b3c4)                \
    X (grid,              "Grid line",           "Graph",    0xff5c5c5c)                \
                                                                                        \
    X (button,            "Button",              "Controls", 0xff474660)                \
    X (field,             "Text field",          "Controls", 0xff28262e)                \
    X (surface,           "Control",             "Controls", 0xff37364a)                \
    X (surfaceBright,     "Hover and selection", "Controls", 0xff4f485d)                \
    X (track,             "Unfilled track",      "Controls", 0xff565656)                \
    X (handle,            "Knob cap and grip",   "Controls", 0xfff9fbff)                \
                                                                                        \
    X (text,              "Text",                "Text",     0xfff9fbff)                \
    X (textDim,           "Text, idle",          "Text",     0xffd9d9d9)                \
    X (comment,           "Text, secondary",     "Text",     0xff888791)                \
    X (textDisabled,      "Text, disabled",      "Text",     0xff888791)                \
                                                                                        \
    X (chrome,            "Popup ground",        "Panels",   0xff3b334b)                \
                                                                                        \
    X (accent,            "Accent",              "Accents",  0xff9761dc)                \
                                                                                        \
    X (danger,            "Danger",              "States",   0xfff92672)                \
    X (error,             "Error",               "States",   0xfff92672)

/** A plugin's own shipped values for shared roles, from its PluginThemeRoles.h:

        #define CELINE_PLUGIN_SHARED_ROLE_VALUES(X) \
            X (consoleBackground, 0xff121212)       \
            X (surface,           0xff262626)

    The role keeps its name, its label and its group -- a theme file written by one plugin
    still loads into another -- and only what it ships as changes. A plugin that says
    nothing ships the house values above. */
#ifndef CELINE_PLUGIN_SHARED_ROLE_VALUES
 #define CELINE_PLUGIN_SHARED_ROLE_VALUES(X)
#endif

/** The shared list and whatever this plugin adds to it. */
#define CELINE_THEME_ROLES(X)                                                           \
    CELINE_SHARED_THEME_ROLES(X)                                                        \
    CELINE_PLUGIN_THEME_ROLES(X)
