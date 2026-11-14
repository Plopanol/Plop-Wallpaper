#include "theme.h"

#include <dwmapi.h>
#include <uxtheme.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace {

constexpr COLORREF backgroundColor = RGB(32, 32, 32);
constexpr COLORREF fieldColor = RGB(43, 43, 43);
constexpr COLORREF textColor = RGB(245, 245, 245);
constexpr COLORREF mutedColor = RGB(165, 165, 165);

bool useDarkColors()
{
    HIGHCONTRASTW contrast{sizeof(contrast)};
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON)) {
        return false;
    }
    try {
        using namespace winrt::Windows::UI::ViewManagement;
        const auto foreground = UISettings().GetColorValue(UIColorType::Foreground);
        return 5 * foreground.G + 2 * foreground.R + foreground.B > 8 * 128;
    } catch (const winrt::hresult_error&) {
        OutputDebugStringW(L"Impossible de lire les couleurs du thème Windows.\n");
        return false;
    }
}

HBRUSH selectColor(HDC context, COLORREF color)
{
    SetDCBrushColor(context, color);
    return static_cast<HBRUSH>(GetStockObject(DC_BRUSH));
}

}

void WindowTheme::refresh(HWND window)
{
    const bool dark = useDarkColors();
    if (initialized_ && dark_ == dark) {
        return;
    }
    initialized_ = true;
    dark_ = dark;
    const BOOL darkFrame = dark_;
    if (FAILED(DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkFrame, sizeof(darkFrame)))) {
        OutputDebugStringW(L"Impossible d’actualiser le thème de la barre de titre.\n");
    }
    for (HWND child = GetWindow(window, GW_CHILD); child; child = GetWindow(child, GW_HWNDNEXT)) {
        // Explorer styling complements the explicit colors for native combo boxes and scrollbars.
        SetWindowTheme(child, dark_ ? L"DarkMode_Explorer" : nullptr, nullptr);
    }
    RedrawWindow(window, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}

void WindowTheme::paintBackground(HDC context, const RECT& bounds) const
{
    FillRect(context, &bounds, dark_ ? selectColor(context, backgroundColor) : GetSysColorBrush(COLOR_BTNFACE));
}

HBRUSH WindowTheme::colorControl(HDC context, HWND control, UINT message) const
{
    if (!dark_) {
        return nullptr;
    }
    wchar_t name[16]{};
    GetClassNameW(control, name, ARRAYSIZE(name));
    const bool field = message == WM_CTLCOLOREDIT || message == WM_CTLCOLORLISTBOX ||
        lstrcmpW(name, L"Edit") == 0 || lstrcmpW(name, L"ComboBox") == 0;
    const COLORREF color = field ? fieldColor : backgroundColor;
    SetTextColor(context, IsWindowEnabled(control) ? textColor : mutedColor);
    SetBkColor(context, color);
    return selectColor(context, color);
}

bool WindowTheme::drawButton(const DRAWITEMSTRUCT& item) const
{
    if (item.CtlType != ODT_BUTTON) {
        return false;
    }
    const bool disabled = (item.itemState & ODS_DISABLED) != 0;
    const bool pressed = (item.itemState & ODS_SELECTED) != 0;
    if (dark_) {
        FillRect(item.hDC, &item.rcItem, selectColor(item.hDC, pressed ? RGB(57, 57, 57) : fieldColor));
        FrameRect(item.hDC, &item.rcItem, selectColor(item.hDC, RGB(94, 94, 94)));
        SetTextColor(item.hDC, disabled ? mutedColor : textColor);
    } else {
        auto bounds = item.rcItem;
        DrawFrameControl(item.hDC, &bounds, DFC_BUTTON, DFCS_BUTTONPUSH |
            (pressed ? DFCS_PUSHED : 0) | (disabled ? DFCS_INACTIVE : 0));
        SetTextColor(item.hDC, GetSysColor(disabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
    }
    SetBkMode(item.hDC, TRANSPARENT);
    wchar_t text[128]{};
    GetWindowTextW(item.hwndItem, text, ARRAYSIZE(text));
    auto bounds = item.rcItem;
    DrawTextW(item.hDC, text, -1, &bounds, DT_CENTER | DT_VCENTER | DT_SINGLELINE |
        ((item.itemState & ODS_NOACCEL) ? DT_HIDEPREFIX : 0));
    if ((item.itemState & ODS_FOCUS) && !(item.itemState & ODS_NOFOCUSRECT)) {
        InflateRect(&bounds, -4, -4);
        DrawFocusRect(item.hDC, &bounds);
    }
    return true;
}
