#include <CelineUI/Fonts.h>
#include <CelineUI/HouseOverrides.h>
#include <CelineUI/Theme.h>
#include <CelineUI/ThemePalette.h>

#include <catch2/catch_test_macros.hpp>

#include <cmath>

/*
    A plugin's own identity: its shipped values for shared roles
    (CELINE_PLUGIN_SHARED_ROLE_VALUES, ThemeRoles.h), and its faces and corner radius
    (HouseOverrides.h).

    The values are compiled in, so these cases mean something only inside a plugin that
    declares some, and a plugin that declares none passes them trivially. A house plugin is
    pinned by ThemeTests; a plugin with its own identity is pinned here, against the values
    it named rather than against the house's.
*/

using namespace Celine;

namespace
{
    /** Puts the palette back however the test leaves, so one case cannot colour the next. */
    struct ShippedPalette
    {
        ShippedPalette() { Theme::palette().reset(); }
        ~ShippedPalette() { Theme::palette().reset(); }
    };
}

TEST_CASE ("A plugin's own values for shared roles are what the palette ships", "[theme]")
{
    ShippedPalette shipped;
    auto& palette = Theme::palette();

    CHECK (palette.isShipped());

    // Each role the plugin names is in the table at its value, and moving it off that
    // value and back takes the palette out of the shipped state and into it again -- which
    // can only happen if the palette compares against the plugin's value, not the house's.
    #define CELINE_TEST_ROLE_VALUE(roleName, value) \
    { \
        const auto role = Theme::Role::roleName; \
        const juce::Colour shippedColour ((std::uint32_t) (value)); \
        \
        CHECK (Theme::info()[(size_t) role].shipped == (std::uint32_t) (value)); \
        CHECK (palette.get (role) == shippedColour); \
        \
        palette.set (role, shippedColour == juce::Colours::black ? juce::Colours::white \
                                                                 : juce::Colours::black); \
        CHECK_FALSE (palette.isShipped()); \
        \
        palette.reset(); \
        CHECK (palette.isShipped()); \
    }

    CELINE_PLUGIN_SHARED_ROLE_VALUES (CELINE_TEST_ROLE_VALUE)

    #undef CELINE_TEST_ROLE_VALUE
}

TEST_CASE ("The plugin's corner radius and typefaces are the ones it names", "[theme]")
{
    CHECK (std::abs (Theme::cornerRadius - (float) CELINE_PLUGIN_CORNER_RADIUS) < 1.0e-6f);

    // A face named by a plugin that is not embedded falls back to JUCE's own, silently and
    // in the wrong shape. Each one the kit draws in has to be in the binary.
    CHECK (Fonts::typeface (Fonts::Weight::Light) != nullptr);
    CHECK (Fonts::typeface (Fonts::Weight::Bold) != nullptr);
    CHECK (Fonts::typeface (Fonts::Weight::Mono) != nullptr);
    CHECK (Fonts::typeface (Fonts::Weight::Logo) != nullptr);
}
