#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Celine
{
    //==========================================================================
    /**
        Jura, the typeface the design is drawn in, loaded out of the binary.

        Bundled rather than asked for by name. A plugin cannot assume anything
        about the machine it lands on, and `juce::Font("Jura", ...)` on a machine
        without it silently falls back to the platform sans -- which looks like a
        layout bug rather than a missing font, because every label is suddenly a
        different width.

        Two weights are used: Light for labels and readouts, Bold for the one
        button that shouts, plus JetBrains Mono for anything that must align. The other weights in `assets/Jura/` are embedded by
        the assets glob but unused; delete the files if the binary size matters
        more than having them to hand.

        If a weight is missing from the binary the accessor returns null and JUCE
        falls back on its own. That is deliberately not an assertion: a missing
        font should not stop the plugin from opening.
    */
    namespace Fonts
    {
        /** The weights the design uses, by the filename that carries them.

            Mono is JetBrains Mono, a separate family rather than a weight of
            Jura, used only where text has to line up in columns. */
        enum class Weight
        {
            Light,
            Bold,
            Mono,

            /** Nico Moji, and only for the wordmark beside the logo. A display face
                with a look of its own, which is the point — and the reason it is not
                on this list for anything else to reach for. */
            Logo,

            // Optional faces, for a plugin drawn in the hardware-strip language: used
            // only where the plugin embeds the file, and otherwise answered by the
            // nearest face it does have (see font()), so a plugin that never ships
            // them looks exactly as it did.

            /** Inter ExtraBold: labels and names set in tracked capitals. Falls back
                to Bold. */
            Display,

            /** Inter Bold: a heading a step below Display. Falls back to Bold. */
            Heading,

            /** JetBrains Mono Medium: values. Falls back to Mono. */
            MonoMedium,

            /** JetBrains Mono Bold: captions, and the words on keys. Falls back to
                Mono. */
            MonoBold,
        };

        /** The typeface, parsed once. Null if the file is not embedded. */
        juce::Typeface::Ptr typeface(Weight weight);

        /** A font of the given weight and height in pixels.

            Height rather than "points": every size in the design is a pixel
            measurement taken off the mockup, and JUCE's `withHeight` is the one
            that means the same thing. */
        juce::Font font(Weight weight, float heightInPixels);

        /** Shorthand for the ones the design actually uses. */
        inline juce::Font light(float height) { return font(Weight::Light, height); }
        inline juce::Font bold(float height) { return font(Weight::Bold, height); }
        inline juce::Font mono(float height) { return font(Weight::Mono, height); }

        /** A font sized the way a stylesheet sizes it: `cssPixels` is the em, not the
            line. A design worked out in HTML -- the hardware-strip language was --
            gives its sizes this way, and JUCE's own `withHeight` would come out a fifth
            smaller for the same number, since it measures ascent plus descent.
            `trackingPixels` is CSS letter-spacing, in pixels. */
        juce::Font css(Weight weight, float cssPixels, float trackingPixels = 0.0f);

        /** The wordmark's face. Falls back to Jura Bold rather than to the platform
            sans if Nico Moji is not embedded: a missing display face should leave the
            name looking like the rest of the design, not like a different program. */
        juce::Font logo(float height);
    } // namespace Fonts
} // namespace Celine
