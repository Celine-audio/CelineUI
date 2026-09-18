# CelineUI

The house interface shared by every Céline Audio plugin.
Theming and the theme editor, the base look and feel, fonts and asset lookup, the
About window, and the few widgets every plugin draws with.

## Features

| | |
|---|---|
| **Theming** | `Theme.h`, `ThemeRoles.h`, `ThemePalette.h/.cpp`, `ThemePanel.h/.cpp` |
| **Look and feel** | `LookAndFeelBase.h/.cpp` |
| **Assets and type** | `EmbeddedAssets.h/.cpp`, `Fonts.h/.cpp` |
| **Widgets** | `IconButton.h`, `AboutPanel.h/.cpp`, `ParameterControl.h/.cpp` |
| **Tests** | `tests/ThemeTests.cpp`, `tests/ThemeReachTests.cpp` |

## Using it from a plugin

As a submodule, beside JUCE and the others:

```sh
git submodule add https://github.com/Celine-audio/CelineUI.git modules/CelineUI
```

In the plugin's `CMakeLists.txt`, once `SharedCode` exists:

```cmake
target_include_directories(SharedCode INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(CELINE_UI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/modules/CelineUI" CACHE PATH "The CelineUI checkout to build against")
include("${CELINE_UI_DIR}/CelineUI.cmake")
target_link_libraries(SharedCode INTERFACE CelineUI)
```

and after `include(Tests)`:

```cmake
target_sources(Tests PRIVATE ${CELINE_UI_TESTS})
target_include_directories(Tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
```

Headers are included by repository: `#include <CelineUI/Theme.h>`.

## What each plugin provides

The kit reaches into the plugin through these, by name. All five are required; a
plugin missing one does not compile, which is the point.

- **`source/ProductInfo.h`** : compile-time constants the About window and the theme
  editor read: `companyName`, `copyrightYear`, `extraNotices`, `juceVersion`,
  `repositoryUrl`, `tagline`, `wordmarkAsset`.


- **`source/PluginThemeRoles.h`** : the colour roles this plugin adds to the house
  list. Included by `ThemeRoles.h`.


- **`source/PluginTheme.h`** : the accessors for those roles. Included by `Theme.h`
  *inside* the namespace, so `Theme::irSlot(2)` reads exactly like `Theme::chrome()`.


- **`source/ui/PluginLookAndFeel.h`** — `PluginLookAndFeel`, a subclass of
  `LookAndFeelBase`: the seam a plugin differs through. The About window and the
  theme editor are windows of their own and each owns one, so they wear the plugin's
  look rather than the house default.


- **Assets** :
  `Jura-Light.ttf`, `Jura-Bold.ttf`, `JetBrainsMono-Regular.ttf`,
  `NicoMoji-Regular.ttf`, `logo.svg`, `format-au.svg`, `format-clap.png`,
  `format-lv2.svg`, `vst-compatible.png`, and the plugin's own wordmark.
  `asio-compatible.png` for standalone plugins.

For the kit's tests, also `tests/helpers/test_helpers.h` and `source/PluginEditor.h`.

## Working on the kit

To work on the kit itself, keep one checkout beside the plugins and point every
plugin at it:

```sh
cmake -B cmake-build-debug -DCELINE_UI_DIR="../CelineUI"
```

(or the same `-D` in each CLion CMake profile). One working copy, built by all three,
so an edit is made once and tried everywhere. CI never sets it, and builds exactly
the commit each plugin pins.

Three habits that keep this safe:

1. **Try a kit change in every plugin before committing it.** There is no CI of its
   own: the kit only means something compiled into a plugin.
2. **Commit the kit first, then move each plugin's submodule to that commit** — the
   one you actually built against.
3. **`git config push.recurseSubmodules check`** in each plugin repository. It refuses
   to push a plugin commit that points at a kit commit not yet pushed, which would
   otherwise break every CI run and every fresh clone.

A submodule checks out a commit, not a branch. Before editing inside
`modules/CelineUI`, `git switch main` there — or better, edit in the shared checkout.

## Conventions

All of these are silent when broken, which is why each has a test or a convention
behind it rather than a note.

**A colour is read at paint time.** `Theme::accent()` is a lookup, not a constant. A
colour handed to `setColour`, or painted into a drawable with `Assets::tint`, is a
snapshot and does not follow a theme. Where a JUCE widget insists on being *told*,
gather the colours into an `applyColours()` and call it from `lookAndFeelChanged()`
as well.

**A hover is derived, not themed.** `Theme::underPointer (fill, hovered, held)` is the
one rule, and both `LookAndFeelBase::drawButtonBackground` and
`IconButton::paintButton` call it. A hover with a colour of its own is a second thing
to keep in step with the first.

**Tinting is destructive.** `Assets::tint` writes the colour into the drawable, so
re-tinting colours the previous tint. Artwork has to be re-read from the binary each
time its colour changes.

**A window of its own hears nothing.** `sendLookAndFeelChange()` walks the editor's
tree. A `DialogWindow` (like the About window, the theme editor) is not in it, and has to
register with `Theme::palette()` directly.

**Assets are looked up by filename**, never by the BinaryData identifier, which JUCE
derives by stripping characters rather than replacing them. `Fonts.cpp` is the one
place allowed to touch `BinaryData` directly, because it is what implements the
lookup.

**`setSize` goes last in a constructor.** It fires `resized()`, which measures artwork
and children that must exist by then.

**Renaming a role breaks every saved theme**, because the identifier is the key in a
`.celthm` file. Adding, removing and regrouping are all free.

## Later

- **The assets.** The fonts and format marks above are duplicated in every plugin's
  `assets/`, and belong here with the code that loads them. That needs a rule for a
  plugin file with the same name as a kit file, since lookup is by filename.
- **The namespace.** The kit is at global scope except `Theme`, `Assets` and `Fonts`,
  which are under `Celine`. Worth unifying, as a change of its own: it touches every
  call site in every plugin.

## Licence

AGPL-3.0. See `LICENSE` and `COPYING`.