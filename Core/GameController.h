#pragma once
#include "pch.h"
class GameController
{
    struct KeyBorderPayload
    {
        BOOL leftArrowPressed;
        BOOL rightArrowPressed;
        BOOL topArrowPressed;
        BOOL bottomArrowPressed;
    };

    KeyBorderPayload keyBorderPayload = {};
public:
    void OnKeyDown(int key);
    void OnKeyPress(int key);
    void OnKeyUp(int key);
};
