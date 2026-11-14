#pragma once

#include <windows.h>

class WindowTheme {
public:
    void refresh(HWND window);
    void paintBackground(HDC context, const RECT& bounds) const;
    HBRUSH colorControl(HDC context, HWND control, UINT message) const;
    bool drawSection(const DRAWITEMSTRUCT& item) const;
    bool drawButton(const DRAWITEMSTRUCT& item) const;
    LRESULT drawModeOption(HWND control, HDC context, RECT bounds) const;

private:
    bool dark_ = false;
    bool initialized_ = false;
};
