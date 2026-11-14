#include "theme.h"

#include <commctrl.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace {

constexpr COLORREF backgroundColor = RGB(32, 32, 32);
constexpr COLORREF fieldColor = RGB(43, 43, 43);
constexpr COLORREF textColor = RGB(245, 245, 245);
constexpr COLORREF mutedColor = RGB(165, 165, 165);
constexpr COLORREF sectionTitleColor = RGB(119, 218, 141);

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

LRESULT CALLBACK drawMonitorHeader(HWND list, UINT message, WPARAM key, LPARAM data,
                                  UINT_PTR, DWORD_PTR dark)
{
    if (dark && message == WM_NOTIFY) {
        const auto& notification = *reinterpret_cast<const NMHDR*>(data);
        if (notification.hwndFrom == ListView_GetHeader(list) && notification.code == NM_CUSTOMDRAW) {
            const auto& item = *reinterpret_cast<const NMCUSTOMDRAW*>(data);
            if (item.dwDrawStage == CDDS_PREPAINT) {
                return CDRF_NOTIFYITEMDRAW;
            }
            if (item.dwDrawStage == CDDS_ITEMPREPAINT) {
                wchar_t text[128]{};
                HDITEMW column{};
                column.mask = HDI_TEXT;
                column.pszText = text;
                column.cchTextMax = ARRAYSIZE(text);
                if (Header_GetItem(notification.hwndFrom, static_cast<int>(item.dwItemSpec), &column)) {
                    const int saved = SaveDC(item.hdc);
                    if (saved) {
                        RECT bounds = item.rc;
                        FillRect(item.hdc, &bounds, selectColor(item.hdc, fieldColor));
                        RECT divider{bounds.right - 1, bounds.top, bounds.right, bounds.bottom};
                        FillRect(item.hdc, &divider, selectColor(item.hdc, backgroundColor));
                        bounds.left += 6;
                        bounds.right -= 6;
                        SetBkMode(item.hdc, TRANSPARENT);
                        SetTextColor(item.hdc, sectionTitleColor);
                        DrawTextW(item.hdc, text, -1, &bounds,
                                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
                        RestoreDC(item.hdc, saved);
                        return CDRF_SKIPDEFAULT;
                    }
                }
            }
        }
    }
    return DefSubclassProc(list, message, key, data);
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
        wchar_t name[32]{};
        GetClassNameW(child, name, ARRAYSIZE(name));
        if (lstrcmpW(name, WC_LISTVIEWW) == 0) {
            const auto background = dark_ ? fieldColor : GetSysColor(COLOR_WINDOW);
            ListView_SetBkColor(child, background);
            ListView_SetTextBkColor(child, background);
            ListView_SetTextColor(child, dark_ ? textColor : GetSysColor(COLOR_WINDOWTEXT));
            const HWND header = ListView_GetHeader(child);
            // The header sends custom-draw notifications to the list view.
            if (!SetWindowSubclass(child, drawMonitorHeader, 1, dark_)) {
                OutputDebugStringW(L"Impossible de personnaliser les en-têtes du tableau.\n");
            }
            SetWindowTheme(header, dark_ ? L"DarkMode_ItemsView" : nullptr, nullptr);
        }
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

bool WindowTheme::drawSection(const DRAWITEMSTRUCT& item) const
{
    if (item.CtlType != ODT_STATIC) {
        return false;
    }
    const int saved = SaveDC(item.hDC);
    if (!saved) {
        return false;
    }
    const auto font = reinterpret_cast<HFONT>(SendMessageW(item.hwndItem, WM_GETFONT, 0, 0));
    if (font) {
        SelectObject(item.hDC, font);
    }
    wchar_t title[128]{};
    GetWindowTextW(item.hwndItem, title, ARRAYSIZE(title));
    auto caption = item.rcItem;
    caption.left += 10;
    DrawTextW(item.hDC, title, -1, &caption, DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);

    auto frame = item.rcItem;
    frame.top = (caption.top + caption.bottom) / 2;
    FrameRect(item.hDC, &frame,
        dark_ ? selectColor(item.hDC, RGB(94, 94, 94)) : GetSysColorBrush(COLOR_3DSHADOW));
    auto gap = caption;
    InflateRect(&gap, 4, 0);
    paintBackground(item.hDC, gap);
    SetBkMode(item.hDC, TRANSPARENT);
    SetTextColor(item.hDC, dark_ ? sectionTitleColor : GetSysColor(COLOR_BTNTEXT));
    DrawTextW(item.hDC, title, -1, &caption, DT_SINGLELINE | DT_NOPREFIX);
    RestoreDC(item.hDC, saved);
    return true;
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
