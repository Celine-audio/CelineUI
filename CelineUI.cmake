# CelineUI -- the house interface shared by every Céline Audio plugin.
#
# A plugin brings it in with three lines, after its SharedCode target exists:
#
#     set(CELINE_UI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/modules/CelineUI" CACHE PATH "...")
#     include("${CELINE_UI_DIR}/CelineUI.cmake")
#     target_link_libraries(SharedCode INTERFACE CelineUI)
#
# and adds ${CELINE_UI_TESTS} to its Tests target. README.md has the rest.
#
# An INTERFACE library on purpose. Its sources compile inside each plugin rather than
# into a library of their own, because they are not complete on their own: Fonts.cpp
# and EmbeddedAssets.cpp read the plugin's generated <BinaryData.h>, and the theme and
# the About window read the plugin's ProductInfo.h, theme fragments and look and feel
# by name. A static library could reach none of those. SharedCode already works this
# way, so to the plugin this is the pattern it has, one directory further off.

include_guard(GLOBAL)

add_library(CelineUI INTERFACE)

file(GLOB CelineUISources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/CelineUI/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CelineUI/*.h")

target_sources(CelineUI INTERFACE ${CelineUISources})

# The repository root, not the source folder, so a plugin writes <CelineUI/Theme.h>
# and every include says which repository the file lives in.
target_include_directories(CelineUI INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

# The kit's tests, for each plugin to add to its own Tests target. They cannot run on
# their own: ThemeReachTests renders the plugin's whole editor, so it only means
# anything compiled against one.
set(CELINE_UI_TESTS
    "${CMAKE_CURRENT_LIST_DIR}/tests/ThemeTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/tests/ThemeReachTests.cpp")
