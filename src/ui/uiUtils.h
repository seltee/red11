#pragma once

#define UI_LEFT 0
#define UI_TOP 1
#define UI_RIGHT 2
#define UI_BOTTOM 3

enum class UIContentDirection
{
    Horizontal,
    Vertical
};

enum class UIContentAlign
{
    Start,
    Middle,
    End,
    SpaceBetween,
    SpaceAround
};

enum class UIBlockPositioning
{
    // Usual block surrounded by other blocks
    Inline,
    // Usual block surrounded by other blocks, but will play the root role for children absolute block
    Relative,
    // Attached to previous root block and rendered on the top of other blocks, position will be in the upper left corner of the root block
    // Plays a role of root block for children absolute blocks
    Absolute
};

enum class UIWordWrap
{
    Disabled,
    ByWord,
    ByLetter
};

enum class UIEvent
{
    Click,
    Release,
    ClickOutside,
    ReleaseOutside,
    Enter,
    Leave
};
