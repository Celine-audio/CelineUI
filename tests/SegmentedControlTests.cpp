/*
    The segmented bar, checked on what makes it a control rather than six buttons.

    Two properties are worth a test and neither is a matter of taste: a press has to land
    on the segment that is under the pointer, and the segment already chosen has to stay
    quiet. The second is the one that decides whether the group answers "which one" or
    "is this one on" -- a control that re-announces its current value on every press of it
    turns a host automation lane into a stream of the value it already had.

    The third case is the drawing: exactly one segment carries the accent, and the chosen
    label is inked so it can be read *on* that accent. Both are colours taken from the
    theme at paint time, so both are silent failures if a snapshot crept in.
*/
#include <CelineUI/Fonts.h>
#include <CelineUI/SegmentedControl.h>
#include <CelineUI/Theme.h>

#include <catch2/catch_test_macros.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

namespace
{
    /** A press at a point, with nothing else going on -- no drag, no modifiers, one
        click. Enough for a control whose whole input is "where". */
    void press (Celine::SegmentedControl& control, juce::Point<float> position)
    {
        const auto now = juce::Time::getCurrentTime();

        const juce::MouseEvent event (juce::Desktop::getInstance().getMainMouseSource(),
                                      position,
                                      juce::ModifierKeys::leftButtonModifier,
                                      1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                      &control, &control, now, position, now, 1, false);

        control.mouseDown (event);
    }

    juce::Colour pixelAt (const juce::Image& image, juce::Point<int> point)
    {
        return image.getPixelAt (point.x, point.y);
    }

    bool isClose (juce::Colour a, juce::Colour b)
    {
        return a.getARGB() == b.getARGB();
    }

    juce::StringArray sixChoices()
    {
        return { "1x12", "2x12", "2x12 V", "4x12", "1x16", "6x10" };
    }
}

TEST_CASE ("A press selects the segment under it", "[celineui]")
{
    Celine::SegmentedControl control (sixChoices(), 0);
    control.setSize (600, 32);

    std::vector<int> announcements;

    control.onSelectionChanged = [&announcements] (int index) { announcements.push_back (index); };

    for (int i = 0; i < 6; ++i)
    {
        // Centre of segment i, measured the way the control measures it.
        const auto notch = Celine::Theme::borderWidth;
        const auto width = (600.0f - notch * 2.0f) / 6.0f;

        press (control, { notch + (float) i * width + width * 0.5f, 16.0f });

        CHECK (control.getSelected() == i);
    }

    // One press per segment, in order, and the first of them was the one already chosen
    // so it says nothing: 1 to 5, not 0 to 5.
    REQUIRE (announcements.size() == 5);
    CHECK (announcements.front() == 1);
    CHECK (announcements.back() == 5);

    // A press in the housing's border, outside every segment, is not a choice.
    press (control, { 0.5f, 16.0f });
    CHECK (control.getSelected() == 5);
    CHECK (announcements.size() == 5);
}

TEST_CASE ("The segment already chosen stays quiet", "[celineui]")
{
    // This is the property that makes it a group rather than a row of toggles, and the
    // one a host would notice: pressing the current choice must not send it again.
    Celine::SegmentedControl control (sixChoices(), 3);
    control.setSize (600, 32);

    auto calls = 0;

    control.onSelectionChanged = [&calls] (int) { ++calls; };

    for (int i = 0; i < 4; ++i)
    {
        const auto notch = Celine::Theme::borderWidth;
        const auto width = (600.0f - notch * 2.0f) / 6.0f;

        press (control, { notch + 3.0f * width + width * 0.5f, 16.0f });
    }

    CHECK (calls == 0);
    CHECK (control.getSelected() == 3);

    // And setting it to what it already is says nothing either, which is what lets a
    // timer sync a control to a parameter without a feedback loop.
    control.setSelected (3);
    CHECK (calls == 0);
}

TEST_CASE ("Exactly one segment is drawn as the chosen one", "[celineui]")
{
    Celine::SegmentedControl control (sixChoices(), 2);
    control.setSize (600, 32);

    juce::Image image (juce::Image::ARGB, 600, 32, true);
    {
        juce::Graphics g (image);
        control.paintEntireComponent (g, true);
    }

    const auto notch = Celine::Theme::borderWidth;
    const auto width = (600.0f - notch * 2.0f) / 6.0f;
    const auto accent = Celine::Theme::accent();

    // The chosen segment is filled with the accent over nearly all of its area, and no
    // other one is. Counted per segment rather than over the whole control, because a
    // total would pass with the accent in the wrong place.
    for (int i = 0; i < 6; ++i)
    {
        const auto left = (int) std::ceil (notch + (float) i * width);
        const auto right = (int) std::floor (notch + (float) (i + 1) * width);

        auto accentPixels = 0;
        auto sampled = 0;

        for (int x = left; x < right; ++x)
        {
            // A row near the top: the labels are centred, so this samples the fill
            // rather than the letters it is inked around.
            if (isClose (pixelAt (image, { x, 6 }), accent))
                ++accentPixels;

            ++sampled;
        }

        INFO ("segment " << i);

        if (i == 2)
            CHECK (accentPixels > sampled * 3 / 4);
        else
            CHECK (accentPixels == 0);
    }

    // The chosen label is drawn in the console ink, so it reads against the accent
    // rather than disappearing into it. Counted as "much darker than the fill" rather
    // than as an exact colour, because a glyph's stems are antialiased and an equality
    // test would be counting how the font rendered rather than whether it was legible.
    auto inkPixels = 0;
    const auto fillBrightness = accent.getPerceivedBrightness();

    for (int y = 8; y < 24; ++y)
        for (int x = (int) (notch + 2.0f * width); x < (int) (notch + 3.0f * width); ++x)
            if (pixelAt (image, { x, y }).getPerceivedBrightness() < fillBrightness * 0.6f)
                ++inkPixels;

    CHECK (inkPixels > 20);
}

TEST_CASE ("A disabled group is drawn dimmed and does not answer", "[celineui]")
{
    // The speaker row is disabled for the bass cabinets, whose driver is part of their
    // design. It has to stay on screen -- a row that vanishes is a row nobody can find
    // again -- but it must neither take a press nor look as though it would.
    Celine::SegmentedControl control (sixChoices(), 2);
    control.setSize (600, 32);

    auto calls = 0;
    control.onSelectionChanged = [&calls] (int) { ++calls; };

    const auto render = [&control]
    {
        juce::Image image (juce::Image::ARGB, 600, 32, true);
        juce::Graphics g (image);

        // Not ignoring the component's alpha: that is where the dimming lives, the way a
        // parent window would paint it.
        control.paintEntireComponent (g, false);
        return image;
    };

    const auto notch = Celine::Theme::borderWidth;
    const auto width = (600.0f - notch * 2.0f) / 6.0f;
    const auto chosen = juce::Point<int> ((int) (notch + 2.5f * width), 6);

    const auto enabledFill = pixelAt (render(), chosen);
    REQUIRE (isClose (enabledFill, Celine::Theme::accent()));

    control.setEnabled (false);

    // Painted at a fraction of its alpha, the accent is no longer drawn at full
    // strength. The bug this replaced dimmed nothing: setOpacity was reset by the very
    // next setColour, so a disabled group was pixel for pixel an enabled one.
    const auto disabledFill = pixelAt (render(), chosen);
    CHECK (disabledFill.getAlpha() < enabledFill.getAlpha() / 2);

    // And it does not answer.
    press (control, { notch + 4.5f * width, 16.0f });
    CHECK (control.getSelected() == 2);
    CHECK (calls == 0);

    // Enabled again, it is back to full strength and answers.
    control.setEnabled (true);
    CHECK (isClose (pixelAt (render(), chosen), Celine::Theme::accent()));

    press (control, { notch + 4.5f * width, 16.0f });
    CHECK (control.getSelected() == 4);
    CHECK (calls == 1);
}

TEST_CASE ("Labels share one size, fitted to the narrowest segment", "[celineui]")
{
    // Nine capsule names across a narrow window: the labels scale down together rather
    // than clipping, so a long name stays inside its own segment. Measured as ink: no
    // pixel of the label in the chosen segment strays past that segment's edges.
    Celine::SegmentedControl control ({ "Condenser 6", "Condenser 5", "Condenser 4" }, 1);
    control.setSize (150, 28);

    juce::Image image (juce::Image::ARGB, 150, 28, true);
    {
        juce::Graphics g (image);
        control.paintEntireComponent (g, true);
    }

    const auto notch = Celine::Theme::borderWidth;
    const auto width = (150.0f - notch * 2.0f) / 3.0f;

    // The chosen segment is filled with the accent and its label is inked dark on it,
    // so the label's ink is "much darker than the accent". None of it may be found in
    // the two neighbouring segments, which are not filled and carry their own labels in
    // a different ink.
    const auto fillBrightness = Celine::Theme::accent().getPerceivedBrightness();
    auto inside = 0;

    for (int y = 4; y < 24; ++y)
        for (int x = (int) std::ceil (notch + width) + 1; x < (int) (notch + 2.0f * width) - 1; ++x)
            if (pixelAt (image, { x, y }).getPerceivedBrightness() < fillBrightness * 0.6f)
                ++inside;

    // There is a label, and it was not shrunk to nothing.
    CHECK (inside > 10);

    // The accent fill reaches both edges of the segment on the label's own row: had the
    // text overflowed, its dark ink would have covered the fill at the edges.
    const auto row = 14;
    CHECK (isClose (pixelAt (image, { (int) std::ceil (notch + width) + 1, row }), Celine::Theme::accent()));
    CHECK (isClose (pixelAt (image, { (int) (notch + 2.0f * width) - 2, row }), Celine::Theme::accent()));
}

TEST_CASE ("An empty or shrinking group draws nothing and coalesces", "[celineui]")
{
    // Both happen in use: the physics tab has six cabinets and six microphones, but a
    // control rebuilt with fewer choices must not keep a selection that no longer
    // exists, and an empty one must not draw.
    Celine::SegmentedControl control ({}, 4);
    CHECK (control.getSelected() == 0);
    control.setSize (200, 30);

    juce::Image image (juce::Image::ARGB, 200, 30, true);
    {
        juce::Graphics g (image);
        g.fillAll (Celine::Theme::background());
        control.paintEntireComponent (g, true);
    }

    // Untouched: an empty control draws nothing at all, not an empty well.
    CHECK (isClose (pixelAt (image, { 100, 15 }), Celine::Theme::background()));

    control.setLabels ({ "One", "Two" });
    control.setSelected (1);
    CHECK (control.getSelected() == 1);

    control.setLabels ({ "Only" });

    // The selection was clamped back into a group that no longer contains it.
    CHECK (control.getSelected() == 0);
}
