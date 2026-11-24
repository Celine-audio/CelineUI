#pragma once

#include "Fonts.h"
#include "Theme.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

namespace Celine
{
    //==========================================================================
    /**
        A row of mutually exclusive choices, drawn as one control.

        The kit had a tab bar (`TabHeader`) that decides *what is underneath it*, and a
        pill switch that decides between two states. Neither is the right shape for
        choosing one of six things -- a cabinet, a microphone -- where the choices are
        peers, all of them visible, and the one in force is one of them rather than a
        page in front of the others.

        Six buttons in a row would also do it, but a row of buttons reads as six things
        to press; a segmented bar reads as one value with six positions, which is what it
        is. The housing is one rounded rectangle and the segments inside it share its
        corners, so the group is one object.

        Selection is *not* a toggle: pressing the segment already selected does nothing
        and says nothing, which is what makes the group answer "which one" rather than
        "is this one on".
    */
    class SegmentedControl : public juce::Component
    {
    public:
        explicit SegmentedControl (juce::StringArray items, int selectedIndex = 0)
            : segments (std::move (items))
        {
            setWantsKeyboardFocus (false);
            selected = juce::jlimit (0, juce::jmax (0, segments.size() - 1), selectedIndex);
        }

        int getSelected() const noexcept { return selected; }

        void setSelected (int index, juce::NotificationType notification = juce::sendNotification)
        {
            const auto clamped = juce::jlimit (0, juce::jmax (0, segments.size() - 1), index);

            if (clamped == selected)
                return;

            selected = clamped;
            repaint();

            if (notification != juce::dontSendNotification && onSelectionChanged != nullptr)
                onSelectionChanged (selected);
        }

        /** Replaces the choices. A selection the new list no longer reaches is clamped
            into it, silently: the caller is the one that changed the list, and is the
            one that knows what the value should now be.

            Labels are not shortened. Every label is drawn at one size, chosen in paint()
            so the widest of them fits the narrowest segment -- see there. */
        void setLabels (juce::StringArray items)
        {
            segments = std::move (items);
            selected = juce::jlimit (0, juce::jmax (0, segments.size() - 1), selected);
            repaint();
        }

        std::function<void (int)> onSelectionChanged;

        void paint (juce::Graphics& g) override
        {
            if (segments.isEmpty())
                return;

            // A disabled group is still drawn, dimmed -- see enablementChanged, which
            // does the dimming. Somewhere in a row of choices there is one that does not
            // apply to what is beside it -- a speaker menu with a cabinet whose hole it
            // does not fit -- and hiding the control would leave a hole in the row where
            // a player remembers one being. Dimmed, it says "not this one" without
            // moving anything.
            const auto bounds = getLocalBounds().toFloat().reduced (Theme::borderWidth * 0.5f);
            const auto radius = juce::jmin (Theme::cornerRadius,
                                            juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f);

            g.setColour (Theme::field());
            g.fillRoundedRectangle (bounds, radius);

            // One size for the whole group, chosen so that the longest label fits the
            // narrowest segment. Nine capsules across a window do not leave room for
            // thirteen-pixel words, and a group whose labels were each sized for
            // themselves would read as a mistake rather than as one control.
            auto label = Fonts::bold (juce::jmin (13.0f, getHeight() * 0.46f));
            const auto room = boundsFor (0).getWidth() - 7.0f;

            auto widest = 0.0f;

            for (const auto& segment : segments)
                widest = juce::jmax (widest, juce::GlyphArrangement::getStringWidth (label, segment));

            // Width is very nearly proportional to height, so one measurement is one
            // correction, and a control that paints on every mouse move cannot afford to
            // search for the size it wants.
            if (widest > room)
                label.setHeight (juce::jmax (7.5f, label.getHeight() * room / widest));

            for (int i = 0; i < segments.size(); ++i)
            {
                const auto area = boundsFor (i);
                const auto isSelected = i == selected;

                if (isSelected)
                {
                    g.setColour (Theme::accent());
                    g.fillRoundedRectangle (area, radius * 0.72f);
                }
                else if (i == hovered)
                {
                    g.setColour (Theme::underPointer (Theme::button(), true, false));
                    g.fillRoundedRectangle (area, radius * 0.72f);
                }

                g.setColour (isSelected ? Theme::consoleBackground() : Theme::textDim());
                g.setFont (label);
                g.drawText (segments[i], area.toNearestInt(), juce::Justification::centred, false);
            }

            // An inset rule around the housing, so it reads as a well the segments sit
            // in rather than as six rectangles that happen to touch.
            g.setColour (Theme::line().withAlpha (0.55f));
            g.drawRoundedRectangle (bounds, radius, Theme::borderWidth);
        }

        void mouseDown (const juce::MouseEvent& event) override
        {
            if (! isEnabled())
                return;

            const auto index = indexAt (event.position);

            if (index >= 0)
                setSelected (index);
        }

        void mouseMove (const juce::MouseEvent& event) override
        {
            if (! isEnabled())
                return;

            const auto index = indexAt (event.position);

            if (index == hovered)
                return;

            hovered = index;
            repaint();
        }

        void mouseExit (const juce::MouseEvent&) override
        {
            if (hovered < 0)
                return;

            hovered = -1;
            repaint();
        }

        /** The whole control at partial alpha, composited as one layer.

            This used to be `g.setOpacity` at the top of paint(), which does nothing
            useful: it changes the opacity of the *current colour*, and every fill below
            it starts with a setColour of its own. A disabled group drew at full strength.
            The component's own alpha applies to everything it paints, including what a
            future change adds. And a hover held when the control was disabled is let go,
            or the highlight would stay drawn on a control that no longer answers. */
        void enablementChanged() override
        {
            setAlpha (isEnabled() ? 1.0f : disabledAlpha);

            if (! isEnabled())
                hovered = -1;

            repaint();
        }

        /** How far a disabled group is dimmed. */
        static constexpr float disabledAlpha = 0.4f;

    private:
        juce::Rectangle<float> boundsFor (int index) const
        {
            if (segments.isEmpty())
                return {};

            const auto notch = Theme::borderWidth;
            const auto width = (getWidth() - notch * 2.0f) / (float) segments.size();

            return { notch + (float) index * width, notch, width, getHeight() - notch * 2.0f };
        }

        int indexAt (juce::Point<float> position) const
        {
            for (int i = 0; i < segments.size(); ++i)
                if (boundsFor (i).contains (position))
                    return i;

            return -1;
        }

        juce::StringArray segments;
        int selected = 0;
        int hovered = -1;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SegmentedControl)
    };
} // namespace Celine
