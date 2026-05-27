#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#define NOMINMAX

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <shellapi.h>
#include <uxtheme.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>

#ifndef EM_REDO
#define EM_REDO (WM_USER + 84)
#endif
#ifndef EM_CANREDO
#define EM_CANREDO (WM_USER + 85)
#endif
#ifndef EM_SETZOOM
#define EM_SETZOOM (WM_USER + 225)
#endif
#ifndef EM_GETZOOM
#define EM_GETZOOM (WM_USER + 224)
#endif
#ifndef TCM_SETBKCOLOR
#define TCM_SETBKCOLOR (TCM_FIRST + 29)
#endif
#ifndef TCM_SETTEXTCOLOR
#define TCM_SETTEXTCOLOR (TCM_FIRST + 36)
#endif
#ifndef MNS_CHECKORBMP
#define MNS_CHECKORBMP 0x04000000
#endif
#ifndef ODT_TAB
#define ODT_TAB 101
#endif
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((HANDLE)-4)
#endif
#ifndef PROCESS_PER_MONITOR_DPI_AWARE
#define PROCESS_PER_MONITOR_DPI_AWARE 2
#endif

static const wchar_t* RICHEDIT50W_CLASS_NAME = L"RICHEDIT50W";
static const wchar_t* RICHEDIT20W_CLASS_NAME = L"RichEdit20W";

#define ID_FILE_NEW_TAB              1001
#define ID_FILE_NEW_WINDOW           1002
#define ID_FILE_OPEN                 1003
#define ID_FILE_SAVE                 1004
#define ID_FILE_SAVE_AS              1005
#define ID_FILE_CHANGE_ENCODING      1006
#define ID_FILE_RELOAD_ENCODING      1007
#define ID_FILE_CLOSE_TAB            1008
#define ID_FILE_CLOSE_WINDOW         1009
#define ID_FILE_EXIT                 1010
#define ID_RECENT_BASE               1100
#define ID_RECENT_MAX                10

#define ID_EDIT_UNDO                 2001
#define ID_EDIT_REDO                 2002
#define ID_EDIT_CUT                  2003
#define ID_EDIT_COPY                 2004
#define ID_EDIT_PASTE                2005
#define ID_EDIT_DELETE               2006
#define ID_EDIT_FIND                 2007
#define ID_EDIT_FIND_NEXT            2008
#define ID_EDIT_FIND_PREV            2009
#define ID_EDIT_REPLACE              2010
#define ID_EDIT_GOTO                 2011
#define ID_EDIT_SELECT_ALL           2012
#define ID_EDIT_FONT                 2013

#define ID_EDIT_CONTEXT_UNDO         2201
#define ID_EDIT_CONTEXT_CUT          2202
#define ID_EDIT_CONTEXT_COPY         2203
#define ID_EDIT_CONTEXT_PASTE        2204
#define ID_EDIT_CONTEXT_SELECT_ALL   2205

#define ID_VIEW_ZOOM_IN              3001
#define ID_VIEW_ZOOM_OUT             3002
#define ID_VIEW_ZOOM_RESET           3003
#define ID_VIEW_STATUS_BAR           3004
#define ID_VIEW_WORD_WRAP            3005
#define ID_VIEW_THEME_LIGHT          3006
#define ID_VIEW_THEME_DARK           3007
#define ID_VIEW_THEME_AUTO           3008
#define ID_VIEW_LANGUAGE_JA           3009
#define ID_VIEW_LANGUAGE_EN           3010

#define IDC_OPTION_ENCODING          4001
#define IDC_OPTION_EOL               4002
#define IDC_OPTION_ENCODING_LABEL    4003
#define IDC_OPTION_EOL_LABEL         4004
#define IDC_INPUT_EDIT               4101
#define IDD_FILE_DIALOG_OPTIONS      5001

typedef enum Encoding {
    ENC_AUTO = -1,
    ENC_UTF8 = 0,
    ENC_UTF8_BOM,
    ENC_UTF16_LE,
    ENC_UTF16_BE,
    ENC_SHIFT_JIS,
    ENC_ANSI
} Encoding;

typedef enum EolMode {
    EOL_AUTO = -1,
    EOL_CRLF = 0,
    EOL_LF,
    EOL_CR
} EolMode;

typedef enum ThemeMode {
    THEME_LIGHT = 0,
    THEME_DARK,
    THEME_AUTO
} ThemeMode;

typedef struct TabDoc {
    HWND edit;
    wchar_t* path;
    wchar_t* title;
    Encoding encoding;
    EolMode eol;
    int zoomPercent;
    bool untitled;
} TabDoc;

typedef struct OptionDialogState {
    Encoding* encoding;
    EolMode* eol;
    bool allowAutoEncoding;
    bool allowAutoEol;
    bool result;
    bool done;
    HWND comboEncoding;
    HWND comboEol;
} OptionDialogState;

typedef struct FileDialogOptionsState {
    Encoding* encoding;
    EolMode* eol;
    bool allowAutoEncoding;
    bool allowAutoEol;
    HWND comboEncoding;
    HWND comboEol;
} FileDialogOptionsState;

typedef struct InputDialogState {
    const wchar_t* prompt;
    wchar_t* buffer;
    int bufferChars;
    bool result;
    bool done;
    HWND edit;
} InputDialogState;

typedef struct MenuDrawData {
    wchar_t* text;
    bool separator;
    bool topLevel;
    bool popup;
} MenuDrawData;

typedef struct ByteBuf {
    unsigned char* data;
    size_t size;
    size_t cap;
} ByteBuf;

typedef struct WBuf {
    wchar_t* data;
    size_t len;
    size_t cap;
} WBuf;

typedef struct TabVec {
    TabDoc** data;
    int count;
    int cap;
} TabVec;

typedef struct WStrVec {
    wchar_t** data;
    int count;
    int cap;
} WStrVec;

typedef struct MenuDataVec {
    MenuDrawData** data;
    int count;
    int cap;
} MenuDataVec;

static HINSTANCE g_hInst = NULL;
static HWND g_hwndMain = NULL;
static HWND g_hwndTab = NULL;
static HWND g_hwndStatus = NULL;
static HMENU g_hRecentMenu = NULL;
static HMODULE g_hRichEdit = NULL;
static const wchar_t* g_richEditClass = L"RICHEDIT50W";
static HACCEL g_hAccel = NULL;

static TabVec g_docs = {0};
static int g_current = -1;
static int g_untitledSerial = 1;
static bool g_statusVisible = true;
static bool g_wordWrap = true;
static ThemeMode g_themeMode = THEME_AUTO;
static HBRUSH g_hMainBrush = NULL;
static HBRUSH g_hPanelBrush = NULL;
static HBRUSH g_hMenuBrush = NULL;
static HBRUSH g_hTabBrush = NULL;
static HBRUSH g_hStatusBrush = NULL;
static MenuDataVec g_menuDrawData = {0};
static wchar_t* g_statusText = NULL;
static WNDPROC g_oldTabProc = NULL;
static WNDPROC g_oldStatusProc = NULL;
static WNDPROC g_oldEditProc = NULL;
static HFONT g_hFont = NULL;
static LOGFONTW g_logFont;
static WStrVec g_recentFiles = {0};
static wchar_t* g_iniPath = NULL;
static wchar_t* g_langPath = NULL;
static wchar_t g_languageCode[16] = L"ja";
static WStrVec g_initialFiles = {0};

static UINT g_findMsg = 0;
static FINDREPLACEW g_findReplace;
static HWND g_hwndFindReplace = NULL;
static wchar_t g_findBuffer[256];
static wchar_t g_replaceBuffer[256];
static wchar_t* g_lastFindText = NULL;
static DWORD g_lastFindFlags = FR_DOWN;


static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK OptionDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK InputDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static UINT_PTR CALLBACK FileDialogHookProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK ThemedTabProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK ThemedStatusProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK ThemedEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void LayoutChildren(HWND hwnd);
static void UpdateStatusBar(void);
static void UpdateTabTitle(int index);
static void UpdateRecentMenu(void);
static const wchar_t* T(const wchar_t* key, const wchar_t* fallback);
static void ApplyLanguage(void);
static void LoadAppSettings(void);
static void SaveAppSettings(void);
static void ApplyThemeToApp(void);
static void ApplyThemeToMenus(HMENU menu);
static void PrepareOwnerDrawMenu(HMENU menu, bool topLevel);
static void PaintMenuBarBackgroundRemainder(HWND hwnd);
static void PaintMenuTabSeam(HWND hwnd);
static void PaintTabTopSeam(HWND hwnd, HDC hdc);
static void PaintTabDarkBorders(HWND hwnd, HDC hdc);
static void PaintStatusBarClient(HWND hwnd, HDC hdc);
static void UpdateBrightCaret(HWND edit);
static void ShowEditorContextMenu(HWND edit, POINT ptScreen);
static void StripStatusBarSystemChrome(HWND hwnd);
static bool SaveDocInteractive(TabDoc* doc, bool saveAs);
static bool ConfirmCloseDoc(int index);
static TabDoc* CurrentDoc(void);

static int max_int(int a, int b) { return a > b ? a : b; }
static int min_int(int a, int b) { return a < b ? a : b; }

static void* xmalloc(size_t n) {
    void* p = malloc(n ? n : 1);
    if (!p) ExitProcess(2);
    return p;
}

static void* xrealloc(void* p, size_t n) {
    void* q = realloc(p, n ? n : 1);
    if (!q) ExitProcess(2);
    return q;
}

static wchar_t* xwcsdup0(const wchar_t* s) {
    if (!s) s = L"";
    size_t n = wcslen(s);
    wchar_t* p = (wchar_t*)xmalloc((n + 1) * sizeof(wchar_t));
    memcpy(p, s, (n + 1) * sizeof(wchar_t));
    return p;
}

static wchar_t* xwcsndup0(const wchar_t* s, size_t n) {
    wchar_t* p = (wchar_t*)xmalloc((n + 1) * sizeof(wchar_t));
    if (n) memcpy(p, s, n * sizeof(wchar_t));
    p[n] = 0;
    return p;
}

static void wstr_set(wchar_t** dst, const wchar_t* src) {
    wchar_t* p = xwcsdup0(src ? src : L"");
    free(*dst);
    *dst = p;
}

static void wstr_append_lit(wchar_t** dst, const wchar_t* suffix) {
    if (!suffix) return;
    const wchar_t* a = *dst ? *dst : L"";
    size_t na = wcslen(a), nb = wcslen(suffix);
    wchar_t* p = (wchar_t*)xmalloc((na + nb + 1) * sizeof(wchar_t));
    memcpy(p, a, na * sizeof(wchar_t));
    memcpy(p + na, suffix, (nb + 1) * sizeof(wchar_t));
    free(*dst);
    *dst = p;
}

static wchar_t* FormatStringDup(const wchar_t* fmt, int value) {
    wchar_t buf[128];
    swprintf(buf, 128, fmt, value);
    return xwcsdup0(buf);
}

static wchar_t* BaseNameOfPathDup(const wchar_t* path) {
    if (!path) return xwcsdup0(L"");
    const wchar_t* lastSlash = wcsrchr(path, L'\\');
    const wchar_t* lastFwd = wcsrchr(path, L'/');
    const wchar_t* p = lastSlash > lastFwd ? lastSlash : lastFwd;
    return xwcsdup0(p ? p + 1 : path);
}

static void bytebuf_init(ByteBuf* b) { b->data = NULL; b->size = b->cap = 0; }
static void bytebuf_free(ByteBuf* b) { free(b->data); b->data = NULL; b->size = b->cap = 0; }
static void bytebuf_reserve(ByteBuf* b, size_t cap) {
    if (cap <= b->cap) return;
    b->data = (unsigned char*)xrealloc(b->data, cap);
    b->cap = cap;
}
static void bytebuf_push(ByteBuf* b, unsigned char v) {
    if (b->size + 1 > b->cap) bytebuf_reserve(b, b->cap ? b->cap * 2 : 256);
    b->data[b->size++] = v;
}
static bool bytebuf_resize(ByteBuf* b, size_t n) {
    bytebuf_reserve(b, n);
    b->size = n;
    return true;
}

static void wbuf_init(WBuf* w) { w->data = NULL; w->len = w->cap = 0; }
static void wbuf_free(WBuf* w) { free(w->data); w->data = NULL; w->len = w->cap = 0; }
static void wbuf_reserve(WBuf* w, size_t cap) {
    if (cap <= w->cap) return;
    w->data = (wchar_t*)xrealloc(w->data, cap * sizeof(wchar_t));
    w->cap = cap;
}
static void wbuf_push(WBuf* w, wchar_t ch) {
    if (w->len + 2 > w->cap) wbuf_reserve(w, w->cap ? w->cap * 2 : 256);
    w->data[w->len++] = ch;
    w->data[w->len] = 0;
}
static void wbuf_append(WBuf* w, const wchar_t* s) {
    size_t n = s ? wcslen(s) : 0;
    if (w->len + n + 1 > w->cap) wbuf_reserve(w, w->len + n + 64);
    if (n) memcpy(w->data + w->len, s, n * sizeof(wchar_t));
    w->len += n;
    w->data[w->len] = 0;
}
static wchar_t* wbuf_take(WBuf* w) {
    if (!w->data) return xwcsdup0(L"");
    wchar_t* p = w->data;
    w->data = NULL;
    w->len = w->cap = 0;
    return p;
}

static void tabvec_push(TabVec* v, TabDoc* doc) {
    if (v->count + 1 > v->cap) {
        v->cap = v->cap ? v->cap * 2 : 8;
        v->data = (TabDoc**)xrealloc(v->data, (size_t)v->cap * sizeof(TabDoc*));
    }
    v->data[v->count++] = doc;
}
static void tabvec_remove_at(TabVec* v, int index) {
    if (index < 0 || index >= v->count) return;
    memmove(v->data + index, v->data + index + 1, (size_t)(v->count - index - 1) * sizeof(TabDoc*));
    v->count--;
}

static void wvec_push_dup(WStrVec* v, const wchar_t* s) {
    if (v->count + 1 > v->cap) {
        v->cap = v->cap ? v->cap * 2 : 8;
        v->data = (wchar_t**)xrealloc(v->data, (size_t)v->cap * sizeof(wchar_t*));
    }
    v->data[v->count++] = xwcsdup0(s);
}
static void wvec_clear(WStrVec* v) {
    for (int i = 0; i < v->count; ++i) free(v->data[i]);
    free(v->data);
    v->data = NULL;
    v->count = v->cap = 0;
}
static void wvec_remove_at(WStrVec* v, int idx) {
    if (idx < 0 || idx >= v->count) return;
    free(v->data[idx]);
    memmove(v->data + idx, v->data + idx + 1, (size_t)(v->count - idx - 1) * sizeof(wchar_t*));
    v->count--;
}
static void wvec_insert_front_dup(WStrVec* v, const wchar_t* s) {
    if (v->count + 1 > v->cap) {
        v->cap = v->cap ? v->cap * 2 : 8;
        v->data = (wchar_t**)xrealloc(v->data, (size_t)v->cap * sizeof(wchar_t*));
    }
    memmove(v->data + 1, v->data, (size_t)v->count * sizeof(wchar_t*));
    v->data[0] = xwcsdup0(s);
    v->count++;
}

static void menudata_push(MenuDrawData* m) {
    if (g_menuDrawData.count + 1 > g_menuDrawData.cap) {
        g_menuDrawData.cap = g_menuDrawData.cap ? g_menuDrawData.cap * 2 : 32;
        g_menuDrawData.data = (MenuDrawData**)xrealloc(g_menuDrawData.data, (size_t)g_menuDrawData.cap * sizeof(MenuDrawData*));
    }
    g_menuDrawData.data[g_menuDrawData.count++] = m;
}

static void FreeMenuDrawData(void) {
    for (int i = 0; i < g_menuDrawData.count; ++i) {
        if (g_menuDrawData.data[i]) {
            free(g_menuDrawData.data[i]->text);
            free(g_menuDrawData.data[i]);
        }
    }
    free(g_menuDrawData.data);
    g_menuDrawData.data = NULL;
    g_menuDrawData.count = g_menuDrawData.cap = 0;
}

static const wchar_t* EncodingName(Encoding enc) {
    switch (enc) {
    case ENC_UTF8: return L"UTF-8";
    case ENC_UTF8_BOM: return L"UTF-8 BOM";
    case ENC_UTF16_LE: return L"UTF-16 LE";
    case ENC_UTF16_BE: return L"UTF-16 BE";
    case ENC_SHIFT_JIS: return L"Shift_JIS";
    case ENC_ANSI: return L"ANSI";
    case ENC_AUTO: return T(L"encoding_auto", L"Auto detect");
    default: return T(L"unknown", L"Unknown");
    }
}

static const wchar_t* EolName(EolMode eol) {
    switch (eol) {
    case EOL_CRLF: return L"Windows (CRLF)";
    case EOL_LF: return L"Unix (LF)";
    case EOL_CR: return L"Classic Mac (CR)";
    case EOL_AUTO: return T(L"eol_auto", L"Auto detect");
    default: return T(L"unknown", L"Unknown");
    }
}

static TabDoc* CurrentDoc(void) {
    if (g_current < 0 || g_current >= g_docs.count) return NULL;
    return g_docs.data[g_current];
}

static void ShowError(HWND owner, const wchar_t* text) {
    MessageBoxW(owner, text, T(L"title_error", L"Error"), MB_OK | MB_ICONERROR);
}

static void EnableDpiAwareness(void) {
    HMODULE user32 = LoadLibraryW(L"user32.dll");
    if (user32) {
        typedef BOOL (WINAPI *SetProcessDpiAwarenessContextProc)(HANDLE);
        SetProcessDpiAwarenessContextProc pContext =
            (SetProcessDpiAwarenessContextProc)(void*)GetProcAddress(user32, "SetProcessDpiAwarenessContext");
        if (pContext && pContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
            FreeLibrary(user32);
            return;
        }
        typedef BOOL (WINAPI *SetProcessDPIAwareProc)(void);
        SetProcessDPIAwareProc pAware =
            (SetProcessDPIAwareProc)(void*)GetProcAddress(user32, "SetProcessDPIAware");
        if (pAware) pAware();
        FreeLibrary(user32);
    }
    HMODULE shcore = LoadLibraryW(L"shcore.dll");
    if (shcore) {
        typedef HRESULT (WINAPI *SetProcessDpiAwarenessProc)(int);
        SetProcessDpiAwarenessProc pAwareness =
            (SetProcessDpiAwarenessProc)(void*)GetProcAddress(shcore, "SetProcessDpiAwareness");
        if (pAwareness) pAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        FreeLibrary(shcore);
    }
}

static UINT GetWindowDpiSafe(HWND hwnd) {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        typedef UINT (WINAPI *GetDpiForWindowProc)(HWND);
        GetDpiForWindowProc pGetDpiForWindow =
            (GetDpiForWindowProc)(void*)GetProcAddress(user32, "GetDpiForWindow");
        if (pGetDpiForWindow) {
            UINT dpi = pGetDpiForWindow(hwnd ? hwnd : g_hwndMain);
            if (dpi) return dpi;
        }
    }
    HDC hdc = GetDC(hwnd ? hwnd : NULL);
    UINT dpi = hdc ? (UINT)GetDeviceCaps(hdc, LOGPIXELSX) : 96;
    if (hdc) ReleaseDC(hwnd ? hwnd : NULL, hdc);
    return dpi ? dpi : 96;
}

static int DpiScale(HWND hwnd, int value) {
    return MulDiv(value, (int)GetWindowDpiSafe(hwnd), 96);
}

static void EnsureDefaultFont(void) {
    if (g_hFont) return;
    ZeroMemory(&g_logFont, sizeof(g_logFont));
    HDC hdc = GetDC(g_hwndMain ? g_hwndMain : NULL);
    int dpiY = hdc ? GetDeviceCaps(hdc, LOGPIXELSY) : 96;
    if (hdc) ReleaseDC(g_hwndMain ? g_hwndMain : NULL, hdc);
    g_logFont.lfHeight = -MulDiv(10, dpiY, 72);
    g_logFont.lfWeight = FW_NORMAL;
    wcscpy(g_logFont.lfFaceName, L"Consolas");
    g_hFont = CreateFontIndirectW(&g_logFont);
}

static bool ShouldUseDarkTheme(void) {
    if (g_themeMode == THEME_DARK) return true;
    if (g_themeMode == THEME_LIGHT) return false;
    DWORD value = 1;
    DWORD size = sizeof(value);
    LONG rc = RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, NULL, &value, &size);
    if (rc != ERROR_SUCCESS) return false;
    return value == 0;
}

static COLORREF ThemeWindowBg(void) { return ShouldUseDarkTheme() ? RGB(30, 30, 30) : GetSysColor(COLOR_BTNFACE); }
static COLORREF ThemePanelBg(void) { return ShouldUseDarkTheme() ? RGB(37, 37, 38) : GetSysColor(COLOR_BTNFACE); }
static COLORREF ThemeMenuBg(void) { return ShouldUseDarkTheme() ? RGB(43, 43, 43) : GetSysColor(COLOR_MENU); }
static COLORREF ThemeMenuHotBg(void) { return ShouldUseDarkTheme() ? RGB(64, 64, 64) : RGB(229, 229, 229); }
static COLORREF ThemeTabBg(void) { return ShouldUseDarkTheme() ? RGB(31, 31, 31) : GetSysColor(COLOR_BTNFACE); }
static COLORREF ThemeTabItemBg(bool selected) { return selected ? (ShouldUseDarkTheme() ? RGB(45, 45, 48) : GetSysColor(COLOR_WINDOW)) : (ShouldUseDarkTheme() ? RGB(37, 37, 38) : GetSysColor(COLOR_BTNFACE)); }
static COLORREF ThemeStatusBg(void) { return ShouldUseDarkTheme() ? RGB(37, 37, 38) : GetSysColor(COLOR_BTNFACE); }
static COLORREF ThemeStatusSeparatorColor(void) { return ShouldUseDarkTheme() ? RGB(55, 55, 58) : GetSysColor(COLOR_3DSHADOW); }
static COLORREF ThemeResizeGripColor(void) { return ShouldUseDarkTheme() ? RGB(105, 105, 108) : GetSysColor(COLOR_3DSHADOW); }
static COLORREF ThemeTextColor(void) { return ShouldUseDarkTheme() ? RGB(235, 235, 235) : GetSysColor(COLOR_MENUTEXT); }
static COLORREF ThemeDisabledTextColor(void) { return ShouldUseDarkTheme() ? RGB(120, 120, 120) : GetSysColor(COLOR_GRAYTEXT); }
static COLORREF ThemeBorderColor(void) { return ShouldUseDarkTheme() ? RGB(80, 80, 80) : GetSysColor(COLOR_3DSHADOW); }
static COLORREF ThemeTabBorderColor(void) { return ShouldUseDarkTheme() ? RGB(48, 48, 51) : GetSysColor(COLOR_3DSHADOW); }
static COLORREF ThemeInactiveTabBorderColor(void) {
    // Dark, but distinguishable from the inactive tab fill. This is used for
    // non-selected tab outlines and to overwrite the native light edge.
    return ShouldUseDarkTheme() ? RGB(50, 50, 54) : GetSysColor(COLOR_3DSHADOW);
}
static COLORREF ThemeTabSubtleBorderColor(void) { return ShouldUseDarkTheme() ? RGB(34, 34, 36) : GetSysColor(COLOR_3DSHADOW); }

static int GetStatusBarHeight(HWND hwnd) {
    HDC hdc = GetDC(hwnd ? hwnd : g_hwndMain);
    int textHeight = DpiScale(hwnd, 16);
    if (hdc) {
        HFONT oldFont = (HFONT)SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
        TEXTMETRICW tm;
        if (GetTextMetricsW(hdc, &tm)) textHeight = tm.tmHeight;
        SelectObject(hdc, oldFont);
        ReleaseDC(hwnd ? hwnd : g_hwndMain, hdc);
    }
    return max_int(DpiScale(hwnd, 24), textHeight + DpiScale(hwnd, 8));
}

static void RebuildThemeBrushes(void) {
    if (g_hMainBrush) DeleteObject(g_hMainBrush);
    if (g_hPanelBrush) DeleteObject(g_hPanelBrush);
    if (g_hMenuBrush) DeleteObject(g_hMenuBrush);
    if (g_hTabBrush) DeleteObject(g_hTabBrush);
    if (g_hStatusBrush) DeleteObject(g_hStatusBrush);
    g_hMainBrush = CreateSolidBrush(ThemeWindowBg());
    g_hPanelBrush = CreateSolidBrush(ThemePanelBg());
    g_hMenuBrush = CreateSolidBrush(ThemeMenuBg());
    g_hTabBrush = CreateSolidBrush(ThemeTabBg());
    g_hStatusBrush = CreateSolidBrush(ThemeStatusBg());
}

static void ApplyDarkWindowAttributes(HWND hwnd) {
    if (!hwnd) return;

    BOOL dark = ShouldUseDarkTheme() ? TRUE : FALSE;
    HMODULE hDwm = LoadLibraryW(L"dwmapi.dll");
    if (!hDwm) return;

    typedef HRESULT (WINAPI *DwmSetWindowAttributeProc)(HWND, DWORD, LPCVOID, DWORD);
    DwmSetWindowAttributeProc pDwmSetWindowAttribute =
        (DwmSetWindowAttributeProc)(void*)GetProcAddress(hDwm, "DwmSetWindowAttribute");

    if (pDwmSetWindowAttribute) {
        /*
           20: DWMWA_USE_IMMERSIVE_DARK_MODE on Windows 10 20H1+ / Windows 11
           19: older Windows 10 builds used this value.
           34: DWMWA_BORDER_COLOR
           35: DWMWA_CAPTION_COLOR
           36: DWMWA_TEXT_COLOR

           Important: when returning from dark to light, disabling immersive dark
           mode is not enough.  The explicit caption/border/text colors set below
           remain cached by DWM unless they are reset to DWMWA_COLOR_DEFAULT.
        */
        pDwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
        pDwmSetWindowAttribute(hwnd, 19, &dark, sizeof(dark));

        if (dark) {
            COLORREF borderColor = RGB(45, 45, 48);
            COLORREF captionColor = RGB(32, 32, 32);
            COLORREF captionTextColor = RGB(235, 235, 235);
            pDwmSetWindowAttribute(hwnd, 34, &borderColor, sizeof(borderColor));
            pDwmSetWindowAttribute(hwnd, 35, &captionColor, sizeof(captionColor));
            pDwmSetWindowAttribute(hwnd, 36, &captionTextColor, sizeof(captionTextColor));
        } else {
            COLORREF defaultColor = (COLORREF)0xFFFFFFFF; /* DWMWA_COLOR_DEFAULT */
            pDwmSetWindowAttribute(hwnd, 34, &defaultColor, sizeof(defaultColor));
            pDwmSetWindowAttribute(hwnd, 35, &defaultColor, sizeof(defaultColor));
            pDwmSetWindowAttribute(hwnd, 36, &defaultColor, sizeof(defaultColor));
        }
    }

    FreeLibrary(hDwm);

    /* Force the non-client area, including the title bar, to be recalculated. */
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

static void SetThemedClientEdge(HWND hwnd, bool enableClientEdge) {
    if (!hwnd) return;
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    LONG_PTR next = ex;
    if (enableClientEdge) next |= WS_EX_CLIENTEDGE;
    else next &= ~(LONG_PTR)(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME);
    if (next != ex) {
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, next);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}

static void ApplyThemeToEdit(HWND edit) {
    if (!edit) return;
    bool dark = ShouldUseDarkTheme();
    COLORREF bg = dark ? RGB(32, 32, 32) : GetSysColor(COLOR_WINDOW);
    COLORREF fg = dark ? RGB(230, 230, 230) : GetSysColor(COLOR_WINDOWTEXT);
    if (dark) {
        /* DarkMode_Explorer makes the RichEdit standard scrollbars follow a dark palette on modern Windows. */
        SetWindowTheme(edit, L"DarkMode_Explorer", NULL);
        SetThemedClientEdge(edit, false);
    } else {
        SetWindowTheme(edit, L"Explorer", NULL);
        SetThemedClientEdge(edit, true);
    }
    SendMessageW(edit, EM_SETBKGNDCOLOR, 0, (LPARAM)bg);
    CHARFORMAT2W cf;
    ZeroMemory(&cf, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR;
    cf.crTextColor = fg;
    SendMessageW(edit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
    SendMessageW(edit, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    UpdateBrightCaret(edit);
    InvalidateRect(edit, NULL, TRUE);
}

static void ApplyThemeToAllEditors(void) {
    for (int i = 0; i < g_docs.count; ++i) ApplyThemeToEdit(g_docs.data[i]->edit);
}

static wchar_t* GetEditTextAlloc(HWND edit) {
    if (!edit) return xwcsdup0(L"");
    int len = GetWindowTextLengthW(edit);
    if (len <= 0) return xwcsdup0(L"");
    wchar_t* buf = (wchar_t*)xmalloc((size_t)(len + 1) * sizeof(wchar_t));
    GetWindowTextW(edit, buf, len + 1);
    return buf;
}

static size_t CountStatusChars(const wchar_t* s) {
    size_t n = 0;
    if (!s) return 0;
    for (; *s; ++s) if (*s != L'\r') ++n;
    return n;
}

static void SetEditorZoom(HWND edit, int percent) {
    if (!edit) return;
    if (percent < 10) percent = 10;
    if (percent > 500) percent = 500;
    SendMessageW(edit, EM_SETZOOM, (WPARAM)percent, (LPARAM)100);
}

static bool ReadAllBytes(const wchar_t* path, ByteBuf* out) {
    bytebuf_free(out);
    bytebuf_init(out);
    FILE* fp = _wfopen(path, L"rb");
    if (!fp) return false;
    if (_fseeki64(fp, 0, SEEK_END) != 0) { fclose(fp); return false; }
    long long size = (long long)_ftelli64(fp);
    if (size < 0) { fclose(fp); return false; }
    if (_fseeki64(fp, 0, SEEK_SET) != 0) { fclose(fp); return false; }
    if (size > 0) {
        bytebuf_resize(out, (size_t)size);
        size_t readBytes = fread(out->data, 1, (size_t)size, fp);
        fclose(fp);
        return readBytes == (size_t)size;
    }
    fclose(fp);
    return true;
}

static bool WriteAllBytes(const wchar_t* path, const ByteBuf* bytes) {
    FILE* fp = _wfopen(path, L"wb");
    if (!fp) return false;
    size_t written = 0;
    if (bytes->size > 0) written = fwrite(bytes->data, 1, bytes->size, fp);
    bool ok = (written == bytes->size);
    fclose(fp);
    return ok;
}

static bool IsValidUtf8Range(const unsigned char* p, size_t n) {
    size_t i = 0;
    while (i < n) {
        unsigned char c = p[i];
        if (c <= 0x7F) ++i;
        else if ((c & 0xE0) == 0xC0) {
            if (i + 1 >= n || (p[i + 1] & 0xC0) != 0x80 || c < 0xC2) return false;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            if (i + 2 >= n || (p[i + 1] & 0xC0) != 0x80 || (p[i + 2] & 0xC0) != 0x80) return false;
            if (c == 0xE0 && p[i + 1] < 0xA0) return false;
            if (c == 0xED && p[i + 1] >= 0xA0) return false;
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            if (i + 3 >= n || (p[i + 1] & 0xC0) != 0x80 || (p[i + 2] & 0xC0) != 0x80 || (p[i + 3] & 0xC0) != 0x80) return false;
            if (c == 0xF0 && p[i + 1] < 0x90) return false;
            if (c > 0xF4) return false;
            if (c == 0xF4 && p[i + 1] >= 0x90) return false;
            i += 4;
        } else return false;
    }
    return true;
}

static Encoding DetectEncoding(const ByteBuf* bytes) {
    if (bytes->size >= 3 && bytes->data[0] == 0xEF && bytes->data[1] == 0xBB && bytes->data[2] == 0xBF) return ENC_UTF8_BOM;
    if (bytes->size >= 2 && bytes->data[0] == 0xFF && bytes->data[1] == 0xFE) return ENC_UTF16_LE;
    if (bytes->size >= 2 && bytes->data[0] == 0xFE && bytes->data[1] == 0xFF) return ENC_UTF16_BE;
    if (bytes->size == 0) return ENC_UTF8;
    if (IsValidUtf8Range(bytes->data, bytes->size)) return ENC_UTF8;
    return ENC_SHIFT_JIS;
}

static bool DecodeMultiByte(const unsigned char* p, size_t n, UINT codepage, wchar_t** out) {
    *out = xwcsdup0(L"");
    if (n == 0) return true;
    DWORD flags = (codepage == CP_UTF8) ? MB_ERR_INVALID_CHARS : 0;
    int needed = MultiByteToWideChar(codepage, flags, (const char*)p, (int)n, NULL, 0);
    if (needed <= 0 && codepage == CP_UTF8) {
        flags = 0;
        needed = MultiByteToWideChar(codepage, flags, (const char*)p, (int)n, NULL, 0);
    }
    if (needed <= 0) return false;
    free(*out);
    *out = (wchar_t*)xmalloc((size_t)(needed + 1) * sizeof(wchar_t));
    int done = MultiByteToWideChar(codepage, flags, (const char*)p, (int)n, *out, needed);
    if (done <= 0) { (*out)[0] = 0; return false; }
    (*out)[done] = 0;
    return true;
}

static bool DecodeBytesToWide(const ByteBuf* bytes, Encoding requested, wchar_t** out, Encoding* actual) {
    Encoding enc = (requested == ENC_AUTO) ? DetectEncoding(bytes) : requested;
    size_t offset = 0;
    *out = NULL;
    if (enc == ENC_UTF8_BOM || enc == ENC_UTF8) {
        if (bytes->size >= 3 && bytes->data[0] == 0xEF && bytes->data[1] == 0xBB && bytes->data[2] == 0xBF) offset = 3;
        const unsigned char* p = (offset < bytes->size) ? bytes->data + offset : NULL;
        bool ok = DecodeMultiByte(p, bytes->size - offset, CP_UTF8, out);
        if (actual) *actual = enc;
        return ok;
    }
    if (enc == ENC_SHIFT_JIS || enc == ENC_ANSI) {
        UINT cp = (enc == ENC_SHIFT_JIS) ? 932 : CP_ACP;
        bool ok = DecodeMultiByte(bytes->size ? bytes->data : NULL, bytes->size, cp, out);
        if (actual) *actual = enc;
        return ok;
    }
    if (enc == ENC_UTF16_LE || enc == ENC_UTF16_BE) {
        if (bytes->size >= 2) {
            if (enc == ENC_UTF16_LE && bytes->data[0] == 0xFF && bytes->data[1] == 0xFE) offset = 2;
            if (enc == ENC_UTF16_BE && bytes->data[0] == 0xFE && bytes->data[1] == 0xFF) offset = 2;
        }
        size_t n = (bytes->size - offset) / 2;
        wchar_t* s = (wchar_t*)xmalloc((n + 1) * sizeof(wchar_t));
        for (size_t i = 0; i < n; ++i) {
            unsigned char b0 = bytes->data[offset + i * 2];
            unsigned char b1 = bytes->data[offset + i * 2 + 1];
            s[i] = (enc == ENC_UTF16_LE) ? (wchar_t)((unsigned)b0 | ((unsigned)b1 << 8)) : (wchar_t)(((unsigned)b0 << 8) | (unsigned)b1);
        }
        s[n] = 0;
        *out = s;
        if (actual) *actual = enc;
        return true;
    }
    if (actual) *actual = ENC_UTF8;
    return DecodeMultiByte(bytes->size ? bytes->data : NULL, bytes->size, CP_UTF8, out);
}

static bool EncodeWideToBytes(const wchar_t* text, Encoding enc, ByteBuf* bytes) {
    bytebuf_free(bytes);
    bytebuf_init(bytes);
    if (!text) text = L"";
    if (enc == ENC_AUTO) enc = ENC_UTF8;
    int chars = (int)wcslen(text);
    if (enc == ENC_UTF8 || enc == ENC_UTF8_BOM || enc == ENC_SHIFT_JIS || enc == ENC_ANSI) {
        if (enc == ENC_UTF8_BOM) { bytebuf_push(bytes, 0xEF); bytebuf_push(bytes, 0xBB); bytebuf_push(bytes, 0xBF); }
        UINT cp = CP_UTF8;
        if (enc == ENC_SHIFT_JIS) cp = 932;
        if (enc == ENC_ANSI) cp = CP_ACP;
        if (chars == 0) return true;
        int needed = WideCharToMultiByte(cp, 0, text, chars, NULL, 0, NULL, NULL);
        if (needed <= 0) return false;
        size_t base = bytes->size;
        bytebuf_resize(bytes, base + (size_t)needed);
        int done = WideCharToMultiByte(cp, 0, text, chars, (char*)bytes->data + base, needed, NULL, NULL);
        return done > 0;
    }
    if (enc == ENC_UTF16_LE || enc == ENC_UTF16_BE) {
        if (enc == ENC_UTF16_LE) { bytebuf_push(bytes, 0xFF); bytebuf_push(bytes, 0xFE); }
        else { bytebuf_push(bytes, 0xFE); bytebuf_push(bytes, 0xFF); }
        for (int i = 0; i < chars; ++i) {
            unsigned int ch = (unsigned int)text[i];
            if (enc == ENC_UTF16_LE) { bytebuf_push(bytes, (unsigned char)(ch & 0xFF)); bytebuf_push(bytes, (unsigned char)((ch >> 8) & 0xFF)); }
            else { bytebuf_push(bytes, (unsigned char)((ch >> 8) & 0xFF)); bytebuf_push(bytes, (unsigned char)(ch & 0xFF)); }
        }
        return true;
    }
    return false;
}

static EolMode DetectEol(const wchar_t* s) {
    if (!s) return EOL_CRLF;
    for (size_t i = 0; s[i]; ++i) {
        if (s[i] == L'\r') return (s[i + 1] == L'\n') ? EOL_CRLF : EOL_CR;
        if (s[i] == L'\n') return EOL_LF;
    }
    return EOL_CRLF;
}

static wchar_t* NormalizeToCRLF(const wchar_t* s) {
    WBuf out; wbuf_init(&out);
    if (!s) return xwcsdup0(L"");
    for (size_t i = 0; s[i]; ++i) {
        if (s[i] == L'\r') {
            if (s[i + 1] == L'\n') ++i;
            wbuf_append(&out, L"\r\n");
        } else if (s[i] == L'\n') {
            wbuf_append(&out, L"\r\n");
        } else {
            wbuf_push(&out, s[i]);
        }
    }
    return wbuf_take(&out);
}

static wchar_t* ConvertEolForSave(const wchar_t* s, EolMode eol) {
    const wchar_t* nl = L"\r\n";
    WBuf out; wbuf_init(&out);
    if (!s) return xwcsdup0(L"");
    if (eol == EOL_AUTO) eol = EOL_CRLF;
    if (eol == EOL_LF) nl = L"\n";
    if (eol == EOL_CR) nl = L"\r";
    for (size_t i = 0; s[i]; ++i) {
        if (s[i] == L'\r') {
            if (s[i + 1] == L'\n') ++i;
            wbuf_append(&out, nl);
        } else if (s[i] == L'\n') {
            wbuf_append(&out, nl);
        } else {
            wbuf_push(&out, s[i]);
        }
    }
    return wbuf_take(&out);
}

static void BuildIniPath(void) {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    wstr_set(&g_iniPath, path);
    wstr_append_lit(&g_iniPath, L".ini");
}

static void BuildLangPath(void) {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    wchar_t* slash1 = wcsrchr(path, L'\\');
    wchar_t* slash2 = wcsrchr(path, L'/');
    wchar_t* slash = slash1;
    if (slash2 && (!slash || slash2 > slash)) slash = slash2;
    if (slash) *(slash + 1) = 0;
    else path[0] = 0;
    wstr_set(&g_langPath, path);
    wstr_append_lit(&g_langPath, L"lang_");
    wstr_append_lit(&g_langPath, g_languageCode);
    wstr_append_lit(&g_langPath, L".ini");
}

static const wchar_t* T(const wchar_t* key, const wchar_t* fallback) {
    static wchar_t buffers[16][1024];
    static int idx = 0;
    idx = (idx + 1) & 15;
    if (!g_langPath) BuildLangPath();
    GetPrivateProfileStringW(L"Strings", key, fallback ? fallback : L"", buffers[idx], 1024, g_langPath);
    return buffers[idx];
}

static void LoadAppSettings(void) {
    if (!g_iniPath) BuildIniPath();
    wchar_t lang[16];
    GetPrivateProfileStringW(L"App", L"Language", L"ja", lang, 16, g_iniPath);
    if (wcscmp(lang, L"en") == 0) wcscpy(g_languageCode, L"en");
    else wcscpy(g_languageCode, L"ja");
    BuildLangPath();
}

static void SaveAppSettings(void) {
    if (!g_iniPath) BuildIniPath();
    WritePrivateProfileStringW(L"App", L"Language", g_languageCode, g_iniPath);
}

static bool CurrentLanguageIs(const wchar_t* code) {
    return wcscmp(g_languageCode, code) == 0;
}

static void SetLanguageCode(const wchar_t* code) {
    if (!code || (wcscmp(code, L"ja") != 0 && wcscmp(code, L"en") != 0)) return;
    if (wcscmp(g_languageCode, code) == 0) return;
    wcscpy(g_languageCode, code);
    BuildLangPath();
    SaveAppSettings();
    ApplyLanguage();
}

static void BuildFileFilter(wchar_t* out, size_t chars) {
    const wchar_t* label1 = T(L"filter_text_files", L"Text files (*.txt;*.log;*.ini;*.md;*.c;*.cpp;*.h;*.hpp)");
    const wchar_t* pat1 = L"*.txt;*.log;*.ini;*.md;*.c;*.cpp;*.h;*.hpp";
    const wchar_t* label2 = T(L"filter_all_files", L"All files (*.*)");
    const wchar_t* pat2 = L"*.*";
    size_t pos = 0;
    const wchar_t* parts[] = { label1, pat1, label2, pat2, L"" };
    for (int i = 0; i < 5; ++i) {
        size_t n = wcslen(parts[i]);
        if (pos + n + 1 >= chars) break;
        memcpy(out + pos, parts[i], n * sizeof(wchar_t));
        pos += n;
        out[pos++] = 0;
    }
    if (pos < chars) out[pos] = 0;
}


static void LoadRecentFiles(void) {
    wvec_clear(&g_recentFiles);
    if (!g_iniPath) BuildIniPath();
    wchar_t key[32];
    wchar_t value[4096];
    for (int i = 0; i < ID_RECENT_MAX; ++i) {
        swprintf(key, 32, L"File%d", i);
        value[0] = 0;
        GetPrivateProfileStringW(L"Recent", key, L"", value, 4096, g_iniPath);
        if (value[0]) wvec_push_dup(&g_recentFiles, value);
    }
}

static void SaveRecentFiles(void) {
    if (!g_iniPath) BuildIniPath();
    wchar_t key[32];
    for (int i = 0; i < ID_RECENT_MAX; ++i) {
        swprintf(key, 32, L"File%d", i);
        if (i < g_recentFiles.count) WritePrivateProfileStringW(L"Recent", key, g_recentFiles.data[i], g_iniPath);
        else WritePrivateProfileStringW(L"Recent", key, NULL, g_iniPath);
    }
}

static void AddRecentFile(const wchar_t* path) {
    if (!path || !path[0]) return;
    for (int i = 0; i < g_recentFiles.count; ) {
        if (wcscmp(g_recentFiles.data[i], path) == 0) wvec_remove_at(&g_recentFiles, i);
        else ++i;
    }
    wvec_insert_front_dup(&g_recentFiles, path);
    while (g_recentFiles.count > ID_RECENT_MAX) wvec_remove_at(&g_recentFiles, g_recentFiles.count - 1);
    SaveRecentFiles();
    UpdateRecentMenu();
}

static void ApplyThemeToMenus(HMENU menu) {
    if (!menu) return;
    MENUINFO mi;
    ZeroMemory(&mi, sizeof(mi));
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND | MIM_STYLE;
    mi.hbrBack = g_hMenuBrush ? g_hMenuBrush : (HBRUSH)(COLOR_MENU + 1);
    mi.dwStyle = MNS_CHECKORBMP;
    SetMenuInfo(menu, &mi);
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        MENUITEMINFOW item;
        ZeroMemory(&item, sizeof(item));
        item.cbSize = sizeof(item);
        item.fMask = MIIM_SUBMENU;
        if (GetMenuItemInfoW(menu, i, TRUE, &item) && item.hSubMenu) ApplyThemeToMenus(item.hSubMenu);
    }
}

static void PrepareOwnerDrawMenu(HMENU menu, bool topLevel) {
    if (!menu) return;
    ApplyThemeToMenus(menu);
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        MENUITEMINFOW info;
        ZeroMemory(&info, sizeof(info));
        info.cbSize = sizeof(info);
        info.fMask = MIIM_FTYPE | MIIM_SUBMENU | MIIM_ID | MIIM_DATA;
        if (!GetMenuItemInfoW(menu, i, TRUE, &info)) continue;
        if ((info.fType & MFT_OWNERDRAW) && info.dwItemData) {
            MenuDrawData* old = (MenuDrawData*)info.dwItemData;
            if (info.hSubMenu) PrepareOwnerDrawMenu(info.hSubMenu, false);
            old->topLevel = topLevel;
            old->popup = info.hSubMenu != NULL;
            continue;
        }
        int len = GetMenuStringW(menu, i, NULL, 0, MF_BYPOSITION);
        wchar_t* text = xwcsdup0(L"");
        if (len > 0) {
            free(text);
            text = (wchar_t*)xmalloc((size_t)(len + 1) * sizeof(wchar_t));
            GetMenuStringW(menu, i, text, len + 1, MF_BYPOSITION);
        }
        MenuDrawData* data = (MenuDrawData*)xmalloc(sizeof(MenuDrawData));
        data->text = text;
        data->separator = (info.fType & MFT_SEPARATOR) != 0;
        data->topLevel = topLevel;
        data->popup = info.hSubMenu != NULL;
        menudata_push(data);
        MENUITEMINFOW setInfo;
        ZeroMemory(&setInfo, sizeof(setInfo));
        setInfo.cbSize = sizeof(setInfo);
        setInfo.fMask = MIIM_FTYPE | MIIM_DATA;
        setInfo.fType = info.fType | MFT_OWNERDRAW;
        setInfo.dwItemData = (ULONG_PTR)data;
        SetMenuItemInfoW(menu, i, TRUE, &setInfo);
        if (info.hSubMenu) PrepareOwnerDrawMenu(info.hSubMenu, false);
    }
}

static void DrawMenuItem(DRAWITEMSTRUCT* dis) {
    if (!dis || dis->CtlType != ODT_MENU) return;
    MenuDrawData* data = (MenuDrawData*)dis->itemData;
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    bool selected = (dis->itemState & ODS_SELECTED) != 0;
    bool disabled = (dis->itemState & ODS_DISABLED) != 0;
    bool checked = (dis->itemState & ODS_CHECKED) != 0;
    bool topLevel = data && data->topLevel;
    bool separator = data && data->separator;
    bool popup = data && data->popup;
    HBRUSH bg = CreateSolidBrush(selected ? ThemeMenuHotBg() : ThemeMenuBg());
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);
    if (separator) {
        HPEN pen = CreatePen(PS_SOLID, 1, ThemeBorderColor());
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        int y = rc.top + (rc.bottom - rc.top) / 2;
        MoveToEx(hdc, rc.left + (topLevel ? 8 : 28), y, NULL);
        LineTo(hdc, rc.right - 8, y);
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
        return;
    }
    COLORREF oldText = SetTextColor(hdc, disabled ? ThemeDisabledTextColor() : ThemeTextColor());
    COLORREF oldBk = SetBkColor(hdc, selected ? ThemeMenuHotBg() : ThemeMenuBg());
    int oldMode = SetBkMode(hdc, TRANSPARENT);
    HFONT oldFont = (HFONT)SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
    RECT textRc = rc;
    if (topLevel) {
        textRc.left -= 4;
        textRc.right += 4;
    }
    if (!topLevel) {
        textRc.left += 28;
        textRc.right -= popup ? 24 : 10;
    }
    if (checked && !topLevel) {
        RECT checkRc = rc;
        checkRc.left += 7;
        checkRc.top += 4;
        checkRc.right = checkRc.left + 14;
        checkRc.bottom = checkRc.top + 14;
        HPEN pen = CreatePen(PS_SOLID, 2, disabled ? ThemeDisabledTextColor() : ThemeTextColor());
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        MoveToEx(hdc, checkRc.left + 2, checkRc.top + 7, NULL);
        LineTo(hdc, checkRc.left + 6, checkRc.bottom - 2);
        LineTo(hdc, checkRc.right - 1, checkRc.top + 2);
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }
    if (popup && !topLevel) {
        COLORREF arrowColor = disabled ? ThemeDisabledTextColor() : ThemeTextColor();
        HBRUSH arrowBrush = CreateSolidBrush(arrowColor);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, arrowBrush);
        HPEN arrowPen = CreatePen(PS_SOLID, 1, arrowColor);
        HPEN oldPen = (HPEN)SelectObject(hdc, arrowPen);
        int cx = rc.right - 14;
        int cy = rc.top + (rc.bottom - rc.top) / 2;
        POINT pts[3] = { { cx, cy - 4 }, { cx, cy + 4 }, { cx + 5, cy } };
        Polygon(hdc, pts, 3);
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(arrowPen);
        DeleteObject(arrowBrush);
    }
    if (data && data->text) {
        UINT flags = DT_SINGLELINE | DT_VCENTER | DT_EXPANDTABS;
        if (topLevel) flags |= DT_CENTER;
        if (dis->itemState & ODS_NOACCEL) flags |= DT_HIDEPREFIX;
        DrawTextW(hdc, data->text, -1, &textRc, flags);
    }
    SelectObject(hdc, oldFont);
    SetBkMode(hdc, oldMode);
    SetBkColor(hdc, oldBk);
    SetTextColor(hdc, oldText);
}

static void MeasureMenuItem(MEASUREITEMSTRUCT* mis) {
    if (!mis || mis->CtlType != ODT_MENU) return;
    MenuDrawData* data = (MenuDrawData*)mis->itemData;
    HDC hdc = GetDC(g_hwndMain ? g_hwndMain : NULL);
    HFONT oldFont = hdc ? (HFONT)SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT)) : NULL;
    SIZE sz = { 80, 18 };
    if (hdc && data && data->text && data->text[0]) {
        GetTextExtentPoint32W(hdc, data->text, (int)wcslen(data->text), &sz);
    } else if (data && data->separator) { sz.cx = 80; sz.cy = 6; }
    if (hdc) {
        if (oldFont) SelectObject(hdc, oldFont);
        ReleaseDC(g_hwndMain ? g_hwndMain : NULL, hdc);
    }
    if (data && data->separator) {
        mis->itemWidth = (UINT)sz.cx;
        mis->itemHeight = 8;
    }
    else if (data && data->topLevel) {
        mis->itemWidth = (UINT)sz.cx - 16;
        mis->itemHeight = (UINT)max_int(sz.cy + 2, GetSystemMetrics(SM_CYMENU));
    }
    else {
        mis->itemWidth = (UINT)(sz.cx + 44);
        mis->itemHeight = (UINT)max_int(sz.cy + 8, 24);
    }
}

static HMENU CreateMainMenu(void) {
    HMENU menu = CreateMenu();
    HMENU file = CreatePopupMenu();
    AppendMenuW(file, MF_STRING, ID_FILE_NEW_TAB, T(L"menu_file_new_tab", L"New Tab(&T)\tCtrl+T"));
    AppendMenuW(file, MF_STRING, ID_FILE_NEW_WINDOW, T(L"menu_file_new_window", L"New Window(&N)\tCtrl+N"));
    AppendMenuW(file, MF_STRING, ID_FILE_OPEN, T(L"menu_file_open", L"Open(&O)...\tCtrl+O"));
    g_hRecentMenu = CreatePopupMenu();
    AppendMenuW(file, MF_POPUP, (UINT_PTR)g_hRecentMenu, T(L"menu_file_recent", L"Recent Files(&R)"));
    AppendMenuW(file, MF_SEPARATOR, 0, NULL);
    AppendMenuW(file, MF_STRING, ID_FILE_SAVE, T(L"menu_file_save", L"Save(&S)\tCtrl+S"));
    AppendMenuW(file, MF_STRING, ID_FILE_SAVE_AS, T(L"menu_file_save_as", L"Save As(&A)...\tCtrl+Shift+S"));
    AppendMenuW(file, MF_STRING, ID_FILE_CHANGE_ENCODING, T(L"menu_file_change_encoding", L"Change Encoding(&E)..."));
    AppendMenuW(file, MF_STRING, ID_FILE_RELOAD_ENCODING, T(L"menu_file_reload_encoding", L"Reload with Encoding(&L)..."));
    AppendMenuW(file, MF_SEPARATOR, 0, NULL);
    AppendMenuW(file, MF_STRING, ID_FILE_CLOSE_TAB, T(L"menu_file_close_tab", L"Close Tab(&C)\tCtrl+W"));
    AppendMenuW(file, MF_STRING, ID_FILE_CLOSE_WINDOW, T(L"menu_file_close_window", L"Close Window(&W)"));
    AppendMenuW(file, MF_STRING, ID_FILE_EXIT, T(L"menu_file_exit", L"Exit(&X)"));
    AppendMenuW(menu, MF_POPUP, (UINT_PTR)file, T(L"menu_file", L"File(&F)"));

    HMENU edit = CreatePopupMenu();
    AppendMenuW(edit, MF_STRING, ID_EDIT_UNDO, T(L"menu_edit_undo", L"Undo(&U)\tCtrl+Z"));
    AppendMenuW(edit, MF_STRING, ID_EDIT_REDO, T(L"menu_edit_redo", L"Redo(&R)\tCtrl+Y"));
    AppendMenuW(edit, MF_SEPARATOR, 0, NULL);
    AppendMenuW(edit, MF_STRING, ID_EDIT_CUT, T(L"menu_edit_cut", L"Cut(&T)\tCtrl+X"));
    AppendMenuW(edit, MF_STRING, ID_EDIT_COPY, T(L"menu_edit_copy", L"Copy(&C)\tCtrl+C"));
    AppendMenuW(edit, MF_STRING, ID_EDIT_PASTE, T(L"menu_edit_paste", L"Paste(&P)\tCtrl+V"));
    AppendMenuW(edit, MF_STRING, ID_EDIT_DELETE, T(L"menu_edit_delete", L"Delete(&D)\tDel"));
    AppendMenuW(edit, MF_SEPARATOR, 0, NULL);
    AppendMenuW(edit, MF_STRING, ID_EDIT_FIND, T(L"menu_edit_find", L"Find(&F)...\tCtrl+F"));
    AppendMenuW(edit, MF_STRING, ID_EDIT_FIND_NEXT, T(L"menu_edit_find_next", L"Find Next(&N)\tF3"));
    AppendMenuW(edit, MF_STRING, ID_EDIT_FIND_PREV, T(L"menu_edit_find_prev", L"Find Previous(&P)\tShift+F3"));
    AppendMenuW(edit, MF_STRING, ID_EDIT_REPLACE, T(L"menu_edit_replace", L"Replace(&E)...\tCtrl+H"));
    AppendMenuW(edit, MF_STRING, ID_EDIT_GOTO, T(L"menu_edit_goto", L"Go To(&G)...\tCtrl+G"));
    AppendMenuW(edit, MF_SEPARATOR, 0, NULL);
    AppendMenuW(edit, MF_STRING, ID_EDIT_SELECT_ALL, T(L"menu_edit_select_all", L"Select All(&A)\tCtrl+A"));
    AppendMenuW(edit, MF_STRING, ID_EDIT_FONT, T(L"menu_edit_font", L"Font(&O)..."));
    AppendMenuW(menu, MF_POPUP, (UINT_PTR)edit, T(L"menu_edit", L"Edit(&E)"));

    HMENU view = CreatePopupMenu();
    HMENU zoom = CreatePopupMenu();
    AppendMenuW(zoom, MF_STRING, ID_VIEW_ZOOM_IN, T(L"menu_view_zoom_in", L"Zoom In(&I)\tCtrl++"));
    AppendMenuW(zoom, MF_STRING, ID_VIEW_ZOOM_OUT, T(L"menu_view_zoom_out", L"Zoom Out(&O)\tCtrl+-"));
    AppendMenuW(zoom, MF_STRING, ID_VIEW_ZOOM_RESET, T(L"menu_view_zoom_reset", L"Reset Zoom(&R)\tCtrl+0"));
    AppendMenuW(view, MF_POPUP, (UINT_PTR)zoom, T(L"menu_view_zoom", L"Zoom(&Z)"));
    AppendMenuW(view, MF_SEPARATOR, 0, NULL);
    AppendMenuW(view, MF_STRING, ID_VIEW_STATUS_BAR, T(L"menu_view_status_bar", L"Status Bar(&S)"));
    AppendMenuW(view, MF_STRING, ID_VIEW_WORD_WRAP, T(L"menu_view_word_wrap", L"Word Wrap(&W)"));
    HMENU theme = CreatePopupMenu();
    AppendMenuW(theme, MF_STRING, ID_VIEW_THEME_LIGHT, T(L"menu_view_theme_light", L"Light(&L)"));
    AppendMenuW(theme, MF_STRING, ID_VIEW_THEME_DARK, T(L"menu_view_theme_dark", L"Dark(&D)"));
    AppendMenuW(theme, MF_STRING, ID_VIEW_THEME_AUTO, T(L"menu_view_theme_auto", L"Auto(&A)"));
    AppendMenuW(view, MF_POPUP, (UINT_PTR)theme, T(L"menu_view_theme", L"App Theme(&T)"));
    HMENU lang = CreatePopupMenu();
    AppendMenuW(lang, MF_STRING, ID_VIEW_LANGUAGE_JA, T(L"menu_view_language_ja", L"Japanese"));
    AppendMenuW(lang, MF_STRING, ID_VIEW_LANGUAGE_EN, T(L"menu_view_language_en", L"English"));
    AppendMenuW(view, MF_POPUP, (UINT_PTR)lang, T(L"menu_view_language", L"Language(&L)"));
    AppendMenuW(menu, MF_POPUP, (UINT_PTR)view, T(L"menu_view", L"View(&V)"));
    PrepareOwnerDrawMenu(menu, true);
    return menu;
}

static void UpdateMenuChecks(HMENU menu) {
    if (!menu) return;
    CheckMenuItem(menu, ID_VIEW_STATUS_BAR, MF_BYCOMMAND | (g_statusVisible ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(menu, ID_VIEW_WORD_WRAP, MF_BYCOMMAND | (g_wordWrap ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(menu, ID_VIEW_THEME_LIGHT, MF_BYCOMMAND | (g_themeMode == THEME_LIGHT ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(menu, ID_VIEW_THEME_DARK, MF_BYCOMMAND | (g_themeMode == THEME_DARK ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(menu, ID_VIEW_THEME_AUTO, MF_BYCOMMAND | (g_themeMode == THEME_AUTO ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(menu, ID_VIEW_LANGUAGE_JA, MF_BYCOMMAND | (CurrentLanguageIs(L"ja") ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(menu, ID_VIEW_LANGUAGE_EN, MF_BYCOMMAND | (CurrentLanguageIs(L"en") ? MF_CHECKED : MF_UNCHECKED));
}

static void UpdateRecentMenu(void) {
    if (!g_hRecentMenu) return;
    while (GetMenuItemCount(g_hRecentMenu) > 0) DeleteMenu(g_hRecentMenu, 0, MF_BYPOSITION);
    if (g_recentFiles.count == 0) {
        AppendMenuW(g_hRecentMenu, MF_GRAYED | MF_STRING, 0, T(L"recent_none", L"(None)"));
        PrepareOwnerDrawMenu(g_hRecentMenu, false);
        return;
    }
    for (int i = 0; i < g_recentFiles.count && i < ID_RECENT_MAX; ++i) {
        wchar_t label[4600];
        swprintf(label, 4600, L"%d: %s", i + 1, g_recentFiles.data[i]);
        AppendMenuW(g_hRecentMenu, MF_STRING, ID_RECENT_BASE + i, label);
    }
    PrepareOwnerDrawMenu(g_hRecentMenu, false);
}

static void CenterWindowOnOwner(HWND hwnd, HWND owner) {
    RECT rcOwner;
    if (owner && GetWindowRect(owner, &rcOwner)) {
        RECT rc;
        GetWindowRect(hwnd, &rc);
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        int x = rcOwner.left + ((rcOwner.right - rcOwner.left) - w) / 2;
        int y = rcOwner.top + ((rcOwner.bottom - rcOwner.top) - h) / 2;
        SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }
}

static void AddComboItem(HWND combo, const wchar_t* text, LPARAM data) {
    int idx = (int)SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)text);
    SendMessageW(combo, CB_SETITEMDATA, (WPARAM)idx, data);
}

static void SelectComboByData(HWND combo, LPARAM data) {
    int count = (int)SendMessageW(combo, CB_GETCOUNT, 0, 0);
    for (int i = 0; i < count; ++i) {
        LPARAM d = SendMessageW(combo, CB_GETITEMDATA, (WPARAM)i, 0);
        if (d == data) { SendMessageW(combo, CB_SETCURSEL, (WPARAM)i, 0); return; }
    }
    if (count > 0) SendMessageW(combo, CB_SETCURSEL, 0, 0);
}

static void FillEncodingCombo(HWND combo, Encoding initial, bool allowAutoEncoding) {
    SendMessageW(combo, CB_RESETCONTENT, 0, 0);
    if (allowAutoEncoding) AddComboItem(combo, T(L"encoding_auto", L"Auto detect"), (LPARAM)ENC_AUTO);
    AddComboItem(combo, L"UTF-8", (LPARAM)ENC_UTF8);
    AddComboItem(combo, L"UTF-8 BOM", (LPARAM)ENC_UTF8_BOM);
    AddComboItem(combo, L"UTF-16 LE", (LPARAM)ENC_UTF16_LE);
    AddComboItem(combo, L"UTF-16 BE", (LPARAM)ENC_UTF16_BE);
    AddComboItem(combo, L"Shift_JIS (CP932)", (LPARAM)ENC_SHIFT_JIS);
    AddComboItem(combo, L"ANSI (CP_ACP)", (LPARAM)ENC_ANSI);
    SelectComboByData(combo, (LPARAM)initial);
}

static void FillEolCombo(HWND combo, EolMode initial, bool allowAutoEol) {
    SendMessageW(combo, CB_RESETCONTENT, 0, 0);
    if (allowAutoEol) AddComboItem(combo, T(L"eol_auto", L"Auto detect"), (LPARAM)EOL_AUTO);
    AddComboItem(combo, L"Windows (CRLF)", (LPARAM)EOL_CRLF);
    AddComboItem(combo, L"Unix (LF)", (LPARAM)EOL_LF);
    AddComboItem(combo, L"Classic Mac (CR)", (LPARAM)EOL_CR);
    SelectComboByData(combo, (LPARAM)initial);
}

static void ReadEncodingEolCombos(HWND comboEncoding, HWND comboEol, Encoding* enc, EolMode* eol) {
    int ei = (int)SendMessageW(comboEncoding, CB_GETCURSEL, 0, 0);
    int li = (int)SendMessageW(comboEol, CB_GETCURSEL, 0, 0);
    if (ei >= 0 && enc) *enc = (Encoding)SendMessageW(comboEncoding, CB_GETITEMDATA, (WPARAM)ei, 0);
    if (li >= 0 && eol) *eol = (EolMode)SendMessageW(comboEol, CB_GETITEMDATA, (WPARAM)li, 0);
}

static bool ShowEncodingEolDialog(HWND owner, const wchar_t* title, Encoding* enc, EolMode* eol, bool allowAutoEncoding, bool allowAutoEol) {
    OptionDialogState state;
    ZeroMemory(&state, sizeof(state));
    state.encoding = enc;
    state.eol = eol;
    state.allowAutoEncoding = allowAutoEncoding;
    state.allowAutoEol = allowAutoEol;
    HWND hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE, L"PlainEditorOptionDialog", title,
        WS_POPUP | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
        DpiScale(owner, 360), DpiScale(owner, 178), owner, NULL, g_hInst, &state);
    if (!hwnd) return false;
    CenterWindowOnOwner(hwnd, owner);
    EnableWindow(owner, FALSE);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    MSG msg;
    while (!state.done && GetMessageW(&msg, NULL, 0, 0)) {
        if (!IsDialogMessageW(hwnd, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    }
    EnableWindow(owner, TRUE);
    SetForegroundWindow(owner);
    return state.result;
}

static bool ShowInputDialog(HWND owner, const wchar_t* title, const wchar_t* prompt, wchar_t* buffer, int bufferChars) {
    InputDialogState state;
    ZeroMemory(&state, sizeof(state));
    state.prompt = prompt;
    state.buffer = buffer;
    state.bufferChars = bufferChars;
    HWND hwnd = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE, L"PlainEditorInputDialog", title,
        WS_POPUP | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
        DpiScale(owner, 350), DpiScale(owner, 150), owner, NULL, g_hInst, &state);
    if (!hwnd) return false;
    CenterWindowOnOwner(hwnd, owner);
    EnableWindow(owner, FALSE);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    MSG msg;
    while (!state.done && GetMessageW(&msg, NULL, 0, 0)) {
        if (!IsDialogMessageW(hwnd, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    }
    EnableWindow(owner, TRUE);
    SetForegroundWindow(owner);
    return state.result;
}

static LRESULT CALLBACK OptionDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    OptionDialogState* st = (OptionDialogState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_NCCREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return TRUE;
    }
    case WM_CREATE: {
        st = (OptionDialogState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        HWND label1 = CreateWindowW(L"STATIC", T(L"label_encoding", L"Encoding:"), WS_CHILD | WS_VISIBLE, 18, 20, 90, 22, hwnd, NULL, g_hInst, NULL);
        st->comboEncoding = CreateWindowW(WC_COMBOBOXW, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST, 118, 16, 210, 160, hwnd, (HMENU)IDC_OPTION_ENCODING, g_hInst, NULL);
        HWND label2 = CreateWindowW(L"STATIC", T(L"label_eol", L"Line endings:"), WS_CHILD | WS_VISIBLE, 18, 58, 90, 22, hwnd, NULL, g_hInst, NULL);
        st->comboEol = CreateWindowW(WC_COMBOBOXW, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST, 118, 54, 210, 160, hwnd, (HMENU)IDC_OPTION_EOL, g_hInst, NULL);
        HWND ok = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 158, 104, 74, 26, hwnd, (HMENU)IDOK, g_hInst, NULL);
        HWND cancel = CreateWindowW(L"BUTTON", T(L"button_cancel", L"Cancel"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 240, 104, 86, 26, hwnd, (HMENU)IDCANCEL, g_hInst, NULL);
        SendMessageW(label1, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageW(label2, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageW(st->comboEncoding, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageW(st->comboEol, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageW(ok, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageW(cancel, WM_SETFONT, (WPARAM)font, TRUE);
        FillEncodingCombo(st->comboEncoding, *(st->encoding), st->allowAutoEncoding);
        FillEolCombo(st->comboEol, *(st->eol), st->allowAutoEol);
        SetFocus(st->comboEncoding);
        return 0;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == IDOK) {
            ReadEncodingEolCombos(st->comboEncoding, st->comboEol, st->encoding, st->eol);
            st->result = true; st->done = true; DestroyWindow(hwnd); return 0;
        }
        if (id == IDCANCEL) { st->result = false; st->done = true; DestroyWindow(hwnd); return 0; }
        break;
    }
    case WM_CLOSE:
        if (st) { st->result = false; st->done = true; }
        DestroyWindow(hwnd); return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK InputDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    InputDialogState* st = (InputDialogState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_NCCREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return TRUE;
    }
    case WM_CREATE: {
        st = (InputDialogState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        HWND label = CreateWindowW(L"STATIC", st->prompt ? st->prompt : T(L"label_input", L"Input:"), WS_CHILD | WS_VISIBLE, 18, 18, 300, 22, hwnd, NULL, g_hInst, NULL);
        st->edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", st->buffer ? st->buffer : L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 18, 44, 300, 24, hwnd, (HMENU)IDC_INPUT_EDIT, g_hInst, NULL);
        HWND ok = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 152, 86, 74, 26, hwnd, (HMENU)IDOK, g_hInst, NULL);
        HWND cancel = CreateWindowW(L"BUTTON", T(L"button_cancel", L"Cancel"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 234, 86, 86, 26, hwnd, (HMENU)IDCANCEL, g_hInst, NULL);
        SendMessageW(label, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageW(st->edit, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageW(ok, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageW(cancel, WM_SETFONT, (WPARAM)font, TRUE);
        SendMessageW(st->edit, EM_SETSEL, 0, -1);
        SetFocus(st->edit);
        return 0;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == IDOK) {
            if (st && st->buffer && st->bufferChars > 0) GetWindowTextW(st->edit, st->buffer, st->bufferChars);
            st->result = true; st->done = true; DestroyWindow(hwnd); return 0;
        }
        if (id == IDCANCEL) { st->result = false; st->done = true; DestroyWindow(hwnd); return 0; }
        break;
    }
    case WM_CLOSE:
        if (st) { st->result = false; st->done = true; }
        DestroyWindow(hwnd); return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static HWND CreateEditorControl(HWND parent) {
    DWORD style = WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL | ES_NOHIDESEL | ES_SAVESEL;
    if (!g_wordWrap) style |= WS_HSCROLL | ES_AUTOHSCROLL;
    DWORD exStyle = ShouldUseDarkTheme() ? 0 : WS_EX_CLIENTEDGE;
    HWND edit = CreateWindowExW(exStyle, g_richEditClass, L"", style, 0, 0, 100, 100, parent, NULL, g_hInst, NULL);
    if (!edit) return NULL;
    SendMessageW(edit, EM_SETTEXTMODE, TM_PLAINTEXT | TM_MULTILEVELUNDO, 0);
    SendMessageW(edit, EM_EXLIMITTEXT, 0, 0x7FFFFFFE);
    SendMessageW(edit, EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE);
    SendMessageW(edit, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    WNDPROC oldProc = (WNDPROC)SetWindowLongPtrW(edit, GWLP_WNDPROC, (LONG_PTR)ThemedEditProc);
    if (!g_oldEditProc) g_oldEditProc = oldProc;
    ApplyThemeToEdit(edit);
    return edit;
}

static wchar_t* DisplayTitleForDocDup(TabDoc* doc) {
    if (!doc) return xwcsdup0(L"");
    wchar_t* base = (doc->path && doc->path[0]) ? BaseNameOfPathDup(doc->path) : xwcsdup0(doc->title ? doc->title : L"");
    if (doc->edit && SendMessageW(doc->edit, EM_GETMODIFY, 0, 0)) wstr_append_lit(&base, L"*");
    return base;
}

static void UpdateTabTitle(int index) {
    if (index < 0 || index >= g_docs.count) return;
    wchar_t* title = DisplayTitleForDocDup(g_docs.data[index]);
    TCITEMW item;
    ZeroMemory(&item, sizeof(item));
    item.mask = TCIF_TEXT;
    item.pszText = title;
    TabCtrl_SetItem(g_hwndTab, index, &item);
    free(title);
}

static void FreeTabDoc(TabDoc* doc) {
    if (!doc) return;
    free(doc->path);
    free(doc->title);
    free(doc);
}

static TabDoc* CreateEmptyTab(void) {
    TabDoc* doc = (TabDoc*)xmalloc(sizeof(TabDoc));
    ZeroMemory(doc, sizeof(*doc));
    doc->encoding = ENC_UTF8;
    doc->eol = EOL_CRLF;
    doc->zoomPercent = 100;
    doc->untitled = true;
    doc->path = xwcsdup0(L"");
    doc->title = FormatStringDup(T(L"untitled_format", L"Untitled %d"), g_untitledSerial++);
    doc->edit = CreateEditorControl(g_hwndTab);
    if (!doc->edit) { FreeTabDoc(doc); return NULL; }
    SendMessageW(doc->edit, EM_SETMODIFY, FALSE, 0);
    int index = g_docs.count;
    tabvec_push(&g_docs, doc);
    wchar_t* title = DisplayTitleForDocDup(doc);
    TCITEMW item;
    ZeroMemory(&item, sizeof(item));
    item.mask = TCIF_TEXT;
    item.pszText = title;
    TabCtrl_InsertItem(g_hwndTab, index, &item);
    free(title);
    if (g_current >= 0 && g_current < g_docs.count) ShowWindow(g_docs.data[g_current]->edit, SW_HIDE);
    g_current = index;
    TabCtrl_SetCurSel(g_hwndTab, index);
    ShowWindow(doc->edit, SW_SHOW);
    LayoutChildren(g_hwndMain);
    SetFocus(doc->edit);
    UpdateStatusBar();
    return doc;
}

static void SetCurrentTab(int index) {
    if (index < 0 || index >= g_docs.count) return;
    if (g_current >= 0 && g_current < g_docs.count) ShowWindow(g_docs.data[g_current]->edit, SW_HIDE);
    g_current = index;
    TabCtrl_SetCurSel(g_hwndTab, index);
    ShowWindow(g_docs.data[g_current]->edit, SW_SHOW);
    LayoutChildren(g_hwndMain);
    SetFocus(g_docs.data[g_current]->edit);
    UpdateStatusBar();
}

static void RecreateEditorForDoc(TabDoc* doc) {
    if (!doc) return;
    wchar_t* text = xwcsdup0(L"");
    CHARRANGE sel;
    sel.cpMin = sel.cpMax = 0;
    BOOL modified = FALSE;
    if (doc->edit) {
        free(text);
        text = GetEditTextAlloc(doc->edit);
        SendMessageW(doc->edit, EM_EXGETSEL, 0, (LPARAM)&sel);
        modified = (BOOL)SendMessageW(doc->edit, EM_GETMODIFY, 0, 0);
        DestroyWindow(doc->edit);
    }
    doc->edit = CreateEditorControl(g_hwndTab);
    SetWindowTextW(doc->edit, text);
    SendMessageW(doc->edit, EM_SETMODIFY, modified, 0);
    SetEditorZoom(doc->edit, doc->zoomPercent);
    SendMessageW(doc->edit, EM_EXSETSEL, 0, (LPARAM)&sel);
    ShowWindow(doc->edit, (doc == CurrentDoc()) ? SW_SHOW : SW_HIDE);
    free(text);
}

static void ToggleWordWrap(void) {
    g_wordWrap = !g_wordWrap;
    for (int i = 0; i < g_docs.count; ++i) RecreateEditorForDoc(g_docs.data[i]);
    LayoutChildren(g_hwndMain);
    UpdateStatusBar();
}

static bool LoadFileIntoTab(const wchar_t* path, Encoding requestedEncoding, EolMode requestedEol, TabDoc* target) {
    ByteBuf bytes; bytebuf_init(&bytes);
    if (!ReadAllBytes(path, &bytes)) { ShowError(g_hwndMain, T(L"msg_open_failed", L"Could not open the file.")); return false; }
    wchar_t* wide = NULL;
    Encoding actualEncoding = ENC_UTF8;
    if (!DecodeBytesToWide(&bytes, requestedEncoding, &wide, &actualEncoding)) {
        bytebuf_free(&bytes);
        ShowError(g_hwndMain, T(L"msg_decode_failed", L"Could not read the file with the specified encoding."));
        return false;
    }
    EolMode actualEol = (requestedEol == EOL_AUTO) ? DetectEol(wide) : requestedEol;
    wchar_t* normalized = NormalizeToCRLF(wide);
    TabDoc* doc = target ? target : CreateEmptyTab();
    if (!doc) { free(wide); free(normalized); bytebuf_free(&bytes); return false; }
    wstr_set(&doc->path, path);
    wchar_t* base = BaseNameOfPathDup(path);
    wstr_set(&doc->title, base);
    free(base);
    doc->encoding = actualEncoding;
    doc->eol = actualEol;
    doc->untitled = false;
    SetWindowTextW(doc->edit, normalized);
    SendMessageW(doc->edit, EM_SETMODIFY, FALSE, 0);
    SendMessageW(doc->edit, EM_SETSEL, 0, 0);
    SetEditorZoom(doc->edit, doc->zoomPercent);
    AddRecentFile(path);
    for (int i = 0; i < g_docs.count; ++i) if (g_docs.data[i] == doc) UpdateTabTitle(i);
    UpdateStatusBar();
    free(wide);
    free(normalized);
    bytebuf_free(&bytes);
    return true;
}

static bool SaveDocToPath(TabDoc* doc, const wchar_t* path, Encoding enc, EolMode eol) {
    if (!doc) return false;
    if (enc == ENC_AUTO) enc = ENC_UTF8;
    if (eol == EOL_AUTO) eol = EOL_CRLF;
    wchar_t* text = GetEditTextAlloc(doc->edit);
    wchar_t* converted = ConvertEolForSave(text, eol);
    ByteBuf bytes; bytebuf_init(&bytes);
    if (!EncodeWideToBytes(converted, enc, &bytes)) {
        free(text); free(converted); bytebuf_free(&bytes);
        ShowError(g_hwndMain, T(L"msg_encode_failed", L"Could not convert the text to the specified encoding."));
        return false;
    }
    if (!WriteAllBytes(path, &bytes)) {
        free(text); free(converted); bytebuf_free(&bytes);
        ShowError(g_hwndMain, T(L"msg_save_failed", L"Could not save the file."));
        return false;
    }
    wstr_set(&doc->path, path);
    wchar_t* base = BaseNameOfPathDup(path);
    wstr_set(&doc->title, base);
    free(base);
    doc->encoding = enc;
    doc->eol = eol;
    doc->untitled = false;
    SendMessageW(doc->edit, EM_SETMODIFY, FALSE, 0);
    AddRecentFile(path);
    for (int i = 0; i < g_docs.count; ++i) if (g_docs.data[i] == doc) UpdateTabTitle(i);
    UpdateStatusBar();
    free(text); free(converted); bytebuf_free(&bytes);
    return true;
}

static void InitFileDialogOptionControls(HWND customDlg, FileDialogOptionsState* st) {
    if (!customDlg || !st) return;
    SetDlgItemTextW(customDlg, IDC_OPTION_ENCODING_LABEL, T(L"label_encoding_accel", L"Encoding(&E):"));
    SetDlgItemTextW(customDlg, IDC_OPTION_EOL_LABEL, T(L"label_eol_accel", L"Line endings(&L):"));
    st->comboEncoding = GetDlgItem(customDlg, IDC_OPTION_ENCODING);
    st->comboEol = GetDlgItem(customDlg, IDC_OPTION_EOL);
    if (st->comboEncoding && st->encoding) FillEncodingCombo(st->comboEncoding, *(st->encoding), st->allowAutoEncoding);
    if (st->comboEol && st->eol) FillEolCombo(st->comboEol, *(st->eol), st->allowAutoEol);
}

static void CaptureFileDialogOptionControls(FileDialogOptionsState* st) {
    if (!st || !st->comboEncoding || !st->comboEol) return;
    ReadEncodingEolCombos(st->comboEncoding, st->comboEol, st->encoding, st->eol);
}

static UINT_PTR CALLBACK FileDialogHookProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    (void)wParam;
    FileDialogOptionsState* st = (FileDialogOptionsState*)GetWindowLongPtrW(hdlg, GWLP_USERDATA);
    switch (msg) {
    case WM_INITDIALOG: {
        OPENFILENAMEW* ofn = (OPENFILENAMEW*)lParam;
        st = ofn ? (FileDialogOptionsState*)ofn->lCustData : NULL;
        SetWindowLongPtrW(hdlg, GWLP_USERDATA, (LONG_PTR)st);
        InitFileDialogOptionControls(hdlg, st);
        CaptureFileDialogOptionControls(st);
        return TRUE;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);
        if ((id == IDC_OPTION_ENCODING || id == IDC_OPTION_EOL) && code == CBN_SELCHANGE) {
            CaptureFileDialogOptionControls(st);
            return TRUE;
        }
        if (id == IDOK) CaptureFileDialogOptionControls(st);
        break;
    }
    case WM_NOTIFY: {
        NMHDR* nm = (NMHDR*)lParam;
        if (nm && nm->code == CDN_FILEOK) {
            OFNOTIFYW* notify = (OFNOTIFYW*)lParam;
            if (notify && notify->lpOFN && !st) {
                st = (FileDialogOptionsState*)notify->lpOFN->lCustData;
                SetWindowLongPtrW(hdlg, GWLP_USERDATA, (LONG_PTR)st);
            }
            CaptureFileDialogOptionControls(st);
        }
        break;
    }
    }
    return 0;
}

static bool GetOpenFileNameWithOptions(HWND owner, wchar_t** outPath, Encoding* enc, EolMode* eol) {
    *enc = ENC_AUTO;
    *eol = EOL_AUTO;
    *outPath = NULL;
    FileDialogOptionsState state;
    ZeroMemory(&state, sizeof(state));
    state.encoding = enc;
    state.eol = eol;
    state.allowAutoEncoding = true;
    state.allowAutoEol = true;
    wchar_t fileName[4096];
    fileName[0] = 0;
    wchar_t filter[512];
    BuildFileFilter(filter, 512);
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = 4096;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
        OFN_ENABLESIZING | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
    ofn.lpstrTitle = T(L"dialog_open", L"Open");
    ofn.hInstance = g_hInst;
    ofn.lpTemplateName = MAKEINTRESOURCEW(IDD_FILE_DIALOG_OPTIONS);
    ofn.lpfnHook = FileDialogHookProc;
    ofn.lCustData = (LPARAM)&state;
    if (!GetOpenFileNameW(&ofn)) return false;
    *outPath = xwcsdup0(fileName);
    return true;
}

static bool GetSaveFileNameWithOptions(HWND owner, TabDoc* doc, wchar_t** outPath, Encoding* enc, EolMode* eol) {
    *enc = doc ? doc->encoding : ENC_UTF8;
    *eol = doc ? doc->eol : EOL_CRLF;
    if (*enc == ENC_AUTO) *enc = ENC_UTF8;
    if (*eol == EOL_AUTO) *eol = EOL_CRLF;
    *outPath = NULL;
    FileDialogOptionsState state;
    ZeroMemory(&state, sizeof(state));
    state.encoding = enc;
    state.eol = eol;
    state.allowAutoEncoding = false;
    state.allowAutoEol = false;
    wchar_t fileName[4096];
    fileName[0] = 0;
    if (doc && doc->path && doc->path[0]) { wcsncpy(fileName, doc->path, 4095); fileName[4095] = 0; }
    wchar_t filter[512];
    BuildFileFilter(filter, 512);
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = 4096;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY |
        OFN_ENABLESIZING | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
    ofn.lpstrTitle = T(L"dialog_save_as", L"Save As");
    ofn.hInstance = g_hInst;
    ofn.lpTemplateName = MAKEINTRESOURCEW(IDD_FILE_DIALOG_OPTIONS);
    ofn.lpfnHook = FileDialogHookProc;
    ofn.lCustData = (LPARAM)&state;
    if (!GetSaveFileNameW(&ofn)) return false;
    *outPath = xwcsdup0(fileName);
    return true;
}

static void DoOpenFile(void) {
    wchar_t* path = NULL;
    Encoding enc;
    EolMode eol;
    if (!GetOpenFileNameWithOptions(g_hwndMain, &path, &enc, &eol)) return;
    LoadFileIntoTab(path, enc, eol, NULL);
    free(path);
}

static bool SaveDocInteractive(TabDoc* doc, bool saveAs) {
    if (!doc) return false;
    if (!saveAs && doc->path && doc->path[0]) return SaveDocToPath(doc, doc->path, doc->encoding, doc->eol);
    wchar_t* path = NULL;
    Encoding enc;
    EolMode eol;
    if (!GetSaveFileNameWithOptions(g_hwndMain, doc, &path, &enc, &eol)) return false;
    bool ok = SaveDocToPath(doc, path, enc, eol);
    free(path);
    return ok;
}

static void DoChangeEncoding(void) {
    TabDoc* doc = CurrentDoc();
    if (!doc) return;
    Encoding enc = (doc->encoding == ENC_AUTO) ? ENC_UTF8 : doc->encoding;
    EolMode eol = (doc->eol == EOL_AUTO) ? EOL_CRLF : doc->eol;
    if (ShowEncodingEolDialog(g_hwndMain, T(L"dialog_change_encoding", L"Change Encoding and Line Endings"), &enc, &eol, false, false)) {
        doc->encoding = enc;
        doc->eol = eol;
        UpdateStatusBar();
    }
}

static void DoReloadWithEncoding(void) {
    TabDoc* doc = CurrentDoc();
    if (!doc) return;
    if (!doc->path || !doc->path[0]) { DoOpenFile(); return; }
    if (SendMessageW(doc->edit, EM_GETMODIFY, 0, 0)) {
        int r = MessageBoxW(g_hwndMain, T(L"msg_reload_discard", L"Discard unsaved changes and reload?"), T(L"title_confirm", L"Confirm"), MB_OKCANCEL | MB_ICONWARNING);
        if (r != IDOK) return;
    }
    Encoding enc = ENC_AUTO;
    EolMode eol = EOL_AUTO;
    if (!ShowEncodingEolDialog(g_hwndMain, T(L"dialog_reload_options", L"Reload Options"), &enc, &eol, true, true)) return;
    LoadFileIntoTab(doc->path, enc, eol, doc);
}

static bool ConfirmCloseDoc(int index) {
    if (index < 0 || index >= g_docs.count) return true;
    TabDoc* doc = g_docs.data[index];
    if (!doc->edit || !SendMessageW(doc->edit, EM_GETMODIFY, 0, 0)) return true;
    wchar_t* title = DisplayTitleForDocDup(doc);
    size_t n = wcslen(title);
    if (n && title[n - 1] == L'*') title[n - 1] = 0;
    wchar_t msg[1024];
    swprintf(msg, 1024, T(L"msg_save_changes_format", L"Save changes to %s?"), title);
    free(title);
    int r = MessageBoxW(g_hwndMain, msg, T(L"title_confirm", L"Confirm"), MB_YESNOCANCEL | MB_ICONQUESTION);
    if (r == IDCANCEL) return false;
    if (r == IDNO) return true;
    return SaveDocInteractive(doc, false);
}

static void CloseTabAt(int index, bool askSave) {
    if (index < 0 || index >= g_docs.count) return;
    if (askSave && !ConfirmCloseDoc(index)) return;
    TabDoc* doc = g_docs.data[index];
    DestroyWindow(doc->edit);
    FreeTabDoc(doc);
    tabvec_remove_at(&g_docs, index);
    TabCtrl_DeleteItem(g_hwndTab, index);
    if (g_docs.count == 0) { g_current = -1; CreateEmptyTab(); return; }
    int next = index;
    if (next >= g_docs.count) next = g_docs.count - 1;
    g_current = -1;
    SetCurrentTab(next);
}

static bool ConfirmCloseAll(void) {
    for (int i = 0; i < g_docs.count; ++i) {
        SetCurrentTab(i);
        if (!ConfirmCloseDoc(i)) return false;
    }
    return true;
}

static void DoNewWindow(void) {
    wchar_t exe[MAX_PATH];
    GetModuleFileNameW(NULL, exe, MAX_PATH);
    ShellExecuteW(g_hwndMain, L"open", exe, NULL, NULL, SW_SHOWNORMAL);
}

static LONG GetTextLengthForSearch(HWND edit) { return (LONG)GetWindowTextLengthW(edit); }

static DWORD SearchFlags(DWORD dialogFlags, bool down) {
    DWORD flags = 0;
    if (dialogFlags & FR_MATCHCASE) flags |= FR_MATCHCASE;
    if (dialogFlags & FR_WHOLEWORD) flags |= FR_WHOLEWORD;
    if (down) flags |= FR_DOWN;
    return flags;
}

static bool FindInCurrentEditor(const wchar_t* needle, bool down, DWORD dialogFlags, bool showMessage) {
    TabDoc* doc = CurrentDoc();
    if (!doc || !needle || !needle[0]) return false;
    HWND edit = doc->edit;
    CHARRANGE sel;
    SendMessageW(edit, EM_EXGETSEL, 0, (LPARAM)&sel);
    FINDTEXTEXW ft;
    ZeroMemory(&ft, sizeof(ft));
    ft.lpstrText = (LPWSTR)needle;
    DWORD flags = SearchFlags(dialogFlags, down);
    if (down) { ft.chrg.cpMin = sel.cpMax; ft.chrg.cpMax = -1; }
    else { ft.chrg.cpMin = sel.cpMin; ft.chrg.cpMax = 0; }
    LONG pos = (LONG)SendMessageW(edit, EM_FINDTEXTEXW, flags, (LPARAM)&ft);
    if (pos < 0) {
        LONG len = GetTextLengthForSearch(edit);
        ZeroMemory(&ft, sizeof(ft));
        ft.lpstrText = (LPWSTR)needle;
        if (down) { ft.chrg.cpMin = 0; ft.chrg.cpMax = -1; }
        else { ft.chrg.cpMin = len; ft.chrg.cpMax = 0; }
        pos = (LONG)SendMessageW(edit, EM_FINDTEXTEXW, flags, (LPARAM)&ft);
    }
    if (pos >= 0) {
        SendMessageW(edit, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
        SendMessageW(edit, EM_SCROLLCARET, 0, 0);
        SetFocus(edit);
        return true;
    }
    if (showMessage) MessageBoxW(g_hwndMain, T(L"msg_not_found", L"Not found."), T(L"title_find", L"Find"), MB_OK | MB_ICONINFORMATION);
    return false;
}

static wchar_t* GetSelectedTextAlloc(HWND edit) {
    CHARRANGE sel;
    SendMessageW(edit, EM_EXGETSEL, 0, (LPARAM)&sel);
    if (sel.cpMax <= sel.cpMin) return xwcsdup0(L"");
    size_t n = (size_t)(sel.cpMax - sel.cpMin);
    wchar_t* buf = (wchar_t*)xmalloc((n + 2) * sizeof(wchar_t));
    TEXTRANGEW tr;
    tr.chrg = sel;
    tr.lpstrText = buf;
    SendMessageW(edit, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
    buf[n] = 0;
    return buf;
}

static bool StringsEqualForReplace(const wchar_t* a, const wchar_t* b, DWORD flags) {
    if (!a) a = L"";
    if (!b) b = L"";
    if (flags & FR_MATCHCASE) return wcscmp(a, b) == 0;
    return _wcsicmp(a, b) == 0;
}

static void ReplaceCurrentThenFind(const wchar_t* findText, const wchar_t* replaceText, DWORD flags) {
    TabDoc* doc = CurrentDoc();
    if (!doc || !findText || !findText[0]) return;
    HWND edit = doc->edit;
    wchar_t* selected = GetSelectedTextAlloc(edit);
    if (!StringsEqualForReplace(selected, findText, flags)) {
        free(selected);
        FindInCurrentEditor(findText, (flags & FR_DOWN) != 0, flags, true);
        return;
    }
    free(selected);
    SendMessageW(edit, EM_REPLACESEL, TRUE, (LPARAM)(replaceText ? replaceText : L""));
    FindInCurrentEditor(findText, (flags & FR_DOWN) != 0, flags, false);
}

static void ReplaceAllInCurrentEditor(const wchar_t* findText, const wchar_t* replaceText, DWORD dialogFlags) {
    TabDoc* doc = CurrentDoc();
    if (!doc || !findText || !findText[0]) return;
    if (!replaceText) replaceText = L"";
    HWND edit = doc->edit;
    DWORD flags = SearchFlags(dialogFlags, true);
    LONG cursor = 0;
    int count = 0;
    SendMessageW(edit, WM_SETREDRAW, FALSE, 0);
    for (;;) {
        FINDTEXTEXW ft;
        ZeroMemory(&ft, sizeof(ft));
        ft.chrg.cpMin = cursor;
        ft.chrg.cpMax = -1;
        ft.lpstrText = (LPWSTR)findText;
        LONG pos = (LONG)SendMessageW(edit, EM_FINDTEXTEXW, flags, (LPARAM)&ft);
        if (pos < 0) break;
        SendMessageW(edit, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
        SendMessageW(edit, EM_REPLACESEL, TRUE, (LPARAM)replaceText);
        cursor = ft.chrgText.cpMin + (LONG)wcslen(replaceText);
        ++count;
    }
    SendMessageW(edit, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(edit, NULL, TRUE);
    wchar_t msg[128];
    swprintf(msg, 128, T(L"msg_replace_count_format", L"Replaced %d occurrence(s)."), count);
    MessageBoxW(g_hwndMain, msg, T(L"title_replace", L"Replace"), MB_OK | MB_ICONINFORMATION);
}

static void ShowFindReplaceDialog(bool replace) {
    ZeroMemory(&g_findReplace, sizeof(g_findReplace));
    g_findReplace.lStructSize = sizeof(g_findReplace);
    g_findReplace.hwndOwner = g_hwndMain;
    g_findReplace.Flags = FR_DOWN;
    g_findReplace.lpstrFindWhat = g_findBuffer;
    g_findReplace.wFindWhatLen = 256;
    g_findReplace.lpstrReplaceWith = g_replaceBuffer;
    g_findReplace.wReplaceWithLen = 256;
    if (replace) g_hwndFindReplace = ReplaceTextW(&g_findReplace);
    else g_hwndFindReplace = FindTextW(&g_findReplace);
}

static void HandleFindReplaceMessage(LPARAM lParam) {
    LPFINDREPLACEW fr = (LPFINDREPLACEW)lParam;
    if (fr->Flags & FR_DIALOGTERM) { g_hwndFindReplace = NULL; return; }
    if (fr->lpstrFindWhat && fr->lpstrFindWhat[0]) {
        wstr_set(&g_lastFindText, fr->lpstrFindWhat);
        g_lastFindFlags = fr->Flags;
    }
    bool down = (fr->Flags & FR_DOWN) != 0;
    if (fr->Flags & FR_FINDNEXT) FindInCurrentEditor(fr->lpstrFindWhat, down, fr->Flags, true);
    else if (fr->Flags & FR_REPLACE) ReplaceCurrentThenFind(fr->lpstrFindWhat, fr->lpstrReplaceWith ? fr->lpstrReplaceWith : L"", fr->Flags);
    else if (fr->Flags & FR_REPLACEALL) ReplaceAllInCurrentEditor(fr->lpstrFindWhat, fr->lpstrReplaceWith ? fr->lpstrReplaceWith : L"", fr->Flags);
}

static void DoFindNext(bool down) {
    if (!g_lastFindText || !g_lastFindText[0]) { ShowFindReplaceDialog(false); return; }
    FindInCurrentEditor(g_lastFindText, down, g_lastFindFlags, true);
}

static void DoGotoLine(void) {
    TabDoc* doc = CurrentDoc();
    if (!doc) return;
    wchar_t text[64] = L"1";
    if (!ShowInputDialog(g_hwndMain, T(L"title_goto", L"Go To"), T(L"label_line_number", L"Line number:"), text, 64)) return;
    int line = _wtoi(text);
    if (line < 1) line = 1;
    int lineCount = (int)SendMessageW(doc->edit, EM_GETLINECOUNT, 0, 0);
    if (line > lineCount) line = lineCount;
    LONG index = (LONG)SendMessageW(doc->edit, EM_LINEINDEX, (WPARAM)(line - 1), 0);
    if (index >= 0) {
        CHARRANGE cr;
        cr.cpMin = index;
        cr.cpMax = index;
        SendMessageW(doc->edit, EM_EXSETSEL, 0, (LPARAM)&cr);
        SendMessageW(doc->edit, EM_SCROLLCARET, 0, 0);
        SetFocus(doc->edit);
    }
}

static void DoChooseFont(void) {
    EnsureDefaultFont();
    CHOOSEFONTW cf;
    ZeroMemory(&cf, sizeof(cf));
    cf.lStructSize = sizeof(cf);
    cf.hwndOwner = g_hwndMain;
    cf.lpLogFont = &g_logFont;
    cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
    if (ChooseFontW(&cf)) {
        HFONT newFont = CreateFontIndirectW(&g_logFont);
        if (newFont) {
            if (g_hFont) DeleteObject(g_hFont);
            g_hFont = newFont;
            for (int i = 0; i < g_docs.count; ++i) {
                SendMessageW(g_docs.data[i]->edit, WM_SETFONT, (WPARAM)g_hFont, TRUE);
                ApplyThemeToEdit(g_docs.data[i]->edit);
            }
        }
    }
}

static void DoZoom(int delta) {
    TabDoc* doc = CurrentDoc();
    if (!doc) return;
    if (delta == 0) doc->zoomPercent = 100;
    else doc->zoomPercent += delta;
    if (doc->zoomPercent < 10) doc->zoomPercent = 10;
    if (doc->zoomPercent > 500) doc->zoomPercent = 500;
    SetEditorZoom(doc->edit, doc->zoomPercent);
    UpdateStatusBar();
}

static void UpdateStatusBar(void) {
    TabDoc* doc = CurrentDoc();
    if (!g_hwndStatus || !doc) return;
    CHARRANGE sel;
    sel.cpMin = sel.cpMax = 0;
    SendMessageW(doc->edit, EM_EXGETSEL, 0, (LPARAM)&sel);
    LONG line = (LONG)SendMessageW(doc->edit, EM_LINEFROMCHAR, (WPARAM)sel.cpMin, 0);
    LONG lineIndex = (LONG)SendMessageW(doc->edit, EM_LINEINDEX, (WPARAM)line, 0);
    LONG col = (lineIndex >= 0) ? (sel.cpMin - lineIndex) : 0;
    wchar_t* text = GetEditTextAlloc(doc->edit);
    size_t chars = CountStatusChars(text);
    free(text);
    wchar_t status[512];
    swprintf(status, 512, T(L"status_format", L"Ln %ld, Col %ld | %lu chars | %d%% | %s | %s"),
        line + 1, col + 1, (unsigned long)chars, doc->zoomPercent, EolName(doc->eol), EncodingName(doc->encoding));
    wstr_set(&g_statusText, status);
    SetWindowTextW(g_hwndStatus, g_statusText);
    RedrawWindow(g_hwndStatus, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
}

static void StripStatusBarSystemChrome(HWND hwnd) {
    if (!hwnd) return;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    style &= ~(LONG_PTR)(SBARS_SIZEGRIP | WS_BORDER);
    exStyle &= ~(LONG_PTR)(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME);
    SetWindowLongPtrW(hwnd, GWL_STYLE, style);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

static void LayoutChildren(HWND hwnd) {
    if (!g_hwndTab) return;
    RECT rc;
    GetClientRect(hwnd, &rc);
    int clientW = max_int(0, (int)(rc.right - rc.left));
    int clientH = max_int(0, (int)(rc.bottom - rc.top));
    int statusHeight = 0;
    if (g_hwndStatus && g_statusVisible) {
        statusHeight = GetStatusBarHeight(g_hwndStatus);
        StripStatusBarSystemChrome(g_hwndStatus);
        ShowWindow(g_hwndStatus, SW_SHOW);
        MoveWindow(g_hwndStatus, 0, clientH - statusHeight, clientW, statusHeight, TRUE);
        RedrawWindow(g_hwndStatus, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
    } else if (g_hwndStatus) {
        ShowWindow(g_hwndStatus, SW_HIDE);
    }
    int tabHeight = max_int(0, clientH - statusHeight);
    MoveWindow(g_hwndTab, 0, 0, clientW, tabHeight, TRUE);
    RECT tabRc;
    GetClientRect(g_hwndTab, &tabRc);
    TabCtrl_AdjustRect(g_hwndTab, FALSE, &tabRc);
    for (int i = 0; i < g_docs.count; ++i) {
        int editW = max_int(0, (int)(tabRc.right - tabRc.left));
        int editH = max_int(0, (int)(tabRc.bottom - tabRc.top));
        MoveWindow(g_docs.data[i]->edit, tabRc.left, tabRc.top, editW, editH, TRUE);
    }
}

static bool IsPointInTabHeaderGap(POINT screenPt) {
    if (!g_hwndTab) return false;
    RECT tabWndRect;
    if (!GetWindowRect(g_hwndTab, &tabWndRect)) return false;
    if (!PtInRect(&tabWndRect, screenPt)) return false;

    POINT tabPt = screenPt;
    ScreenToClient(g_hwndTab, &tabPt);

    RECT tabClient;
    GetClientRect(g_hwndTab, &tabClient);
    RECT pageRect = tabClient;
    TabCtrl_AdjustRect(g_hwndTab, FALSE, &pageRect);
    int headerBottom = max_int(0, min_int((int)tabClient.bottom, (int)pageRect.top));
    if (tabPt.y < 0 || tabPt.y >= headerBottom) return false;

    TCHITTESTINFO ht;
    ZeroMemory(&ht, sizeof(ht));
    ht.pt = tabPt;
    int tabIndex = TabCtrl_HitTest(g_hwndTab, &ht);
    return tabIndex < 0;
}

static void OnCommand(HWND hwnd, int id) {
    TabDoc* doc = CurrentDoc();
    HWND edit = doc ? doc->edit : NULL;
    if (id >= ID_RECENT_BASE && id < ID_RECENT_BASE + ID_RECENT_MAX) {
        int idx = id - ID_RECENT_BASE;
        if (idx >= 0 && idx < g_recentFiles.count) {
            if (!LoadFileIntoTab(g_recentFiles.data[idx], ENC_AUTO, EOL_AUTO, NULL)) {
                wvec_remove_at(&g_recentFiles, idx);
                SaveRecentFiles();
                UpdateRecentMenu();
            }
        }
        return;
    }
    switch (id) {
    case ID_FILE_NEW_TAB: CreateEmptyTab(); break;
    case ID_FILE_NEW_WINDOW: DoNewWindow(); break;
    case ID_FILE_OPEN: DoOpenFile(); break;
    case ID_FILE_SAVE: if (doc) SaveDocInteractive(doc, false); break;
    case ID_FILE_SAVE_AS: if (doc) SaveDocInteractive(doc, true); break;
    case ID_FILE_CHANGE_ENCODING: DoChangeEncoding(); break;
    case ID_FILE_RELOAD_ENCODING: DoReloadWithEncoding(); break;
    case ID_FILE_CLOSE_TAB: CloseTabAt(g_current, true); break;
    case ID_FILE_CLOSE_WINDOW:
    case ID_FILE_EXIT: SendMessageW(hwnd, WM_CLOSE, 0, 0); break;
    case ID_EDIT_UNDO: if (edit) SendMessageW(edit, EM_UNDO, 0, 0); break;
    case ID_EDIT_REDO: if (edit) SendMessageW(edit, EM_REDO, 0, 0); break;
    case ID_EDIT_CUT: if (edit) SendMessageW(edit, WM_CUT, 0, 0); break;
    case ID_EDIT_COPY: if (edit) SendMessageW(edit, WM_COPY, 0, 0); break;
    case ID_EDIT_PASTE: if (edit) SendMessageW(edit, WM_PASTE, 0, 0); break;
    case ID_EDIT_DELETE: if (edit) SendMessageW(edit, WM_CLEAR, 0, 0); break;
    case ID_EDIT_FIND: ShowFindReplaceDialog(false); break;
    case ID_EDIT_FIND_NEXT: DoFindNext(true); break;
    case ID_EDIT_FIND_PREV: DoFindNext(false); break;
    case ID_EDIT_REPLACE: ShowFindReplaceDialog(true); break;
    case ID_EDIT_GOTO: DoGotoLine(); break;
    case ID_EDIT_SELECT_ALL: if (edit) SendMessageW(edit, EM_SETSEL, 0, -1); break;
    case ID_EDIT_FONT: DoChooseFont(); break;
    case ID_VIEW_ZOOM_IN: DoZoom(+10); break;
    case ID_VIEW_ZOOM_OUT: DoZoom(-10); break;
    case ID_VIEW_ZOOM_RESET: DoZoom(0); break;
    case ID_VIEW_STATUS_BAR: g_statusVisible = !g_statusVisible; LayoutChildren(hwnd); UpdateMenuChecks(GetMenu(hwnd)); break;
    case ID_VIEW_WORD_WRAP: ToggleWordWrap(); UpdateMenuChecks(GetMenu(hwnd)); break;
    case ID_VIEW_THEME_LIGHT: g_themeMode = THEME_LIGHT; ApplyThemeToApp(); break;
    case ID_VIEW_THEME_DARK: g_themeMode = THEME_DARK; ApplyThemeToApp(); break;
    case ID_VIEW_THEME_AUTO: g_themeMode = THEME_AUTO; ApplyThemeToApp(); break;
    case ID_VIEW_LANGUAGE_JA: SetLanguageCode(L"ja"); break;
    case ID_VIEW_LANGUAGE_EN: SetLanguageCode(L"en"); break;
    default: break;
    }
}


static void ApplyLanguage(void) {
    if (!g_hwndMain) return;
    HMENU oldMenu = GetMenu(g_hwndMain);
    SetMenu(g_hwndMain, NULL);
    if (oldMenu) DestroyMenu(oldMenu);
    FreeMenuDrawData();
    HMENU newMenu = CreateMainMenu();
    SetMenu(g_hwndMain, newMenu);
    UpdateRecentMenu();
    UpdateMenuChecks(newMenu);
    ApplyThemeToMenus(newMenu);
    DrawMenuBar(g_hwndMain);
    UpdateStatusBar();
    RedrawWindow(g_hwndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

static void PaintMenuBarBackgroundRemainder(HWND hwnd) {
    if (!hwnd || !ShouldUseDarkTheme()) return;
    HMENU menu = GetMenu(hwnd);
    if (!menu) return;
    MENUBARINFO mbi;
    ZeroMemory(&mbi, sizeof(mbi));
    mbi.cbSize = sizeof(mbi);
    if (!GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi)) return;
    RECT wr;
    GetWindowRect(hwnd, &wr);
    RECT bar = mbi.rcBar;
    OffsetRect(&bar, -wr.left, -wr.top);
    if (bar.right <= bar.left || bar.bottom <= bar.top) return;
    HDC hdc = GetWindowDC(hwnd);
    if (!hdc) return;
    HRGN remainder = CreateRectRgnIndirect(&bar);
    if (remainder) {
        int count = GetMenuItemCount(menu);
        for (int i = 0; i < count; ++i) {
            RECT item;
            if (GetMenuItemRect(hwnd, menu, i, &item)) {
                OffsetRect(&item, -wr.left, -wr.top);
                InflateRect(&item, 1, 1);
                HRGN itemRgn = CreateRectRgnIndirect(&item);
                if (itemRgn) {
                    CombineRgn(remainder, remainder, itemRgn, RGN_DIFF);
                    DeleteObject(itemRgn);
                }
            }
        }
        SelectClipRgn(hdc, remainder);
        FillRect(hdc, &bar, g_hMenuBrush ? g_hMenuBrush : (HBRUSH)(COLOR_MENU + 1));
        SelectClipRgn(hdc, NULL);
        DeleteObject(remainder);
    }
    ReleaseDC(hwnd, hdc);
}

static void PaintMenuTabSeam(HWND hwnd) {
    if (!hwnd || !ShouldUseDarkTheme()) return;
    HMENU menu = GetMenu(hwnd);
    if (!menu) return;
    MENUBARINFO mbi;
    ZeroMemory(&mbi, sizeof(mbi));
    mbi.cbSize = sizeof(mbi);
    if (!GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi)) return;
    RECT wr;
    GetWindowRect(hwnd, &wr);
    RECT bar = mbi.rcBar;
    OffsetRect(&bar, -wr.left, -wr.top);
    if (bar.right <= bar.left || bar.bottom <= bar.top) return;
    HDC hdc = GetWindowDC(hwnd);
    if (!hdc) return;
    RECT seam = bar;
    seam.top = bar.bottom - 1;
    seam.bottom = bar.bottom + 1;
    FillRect(hdc, &seam, g_hTabBrush ? g_hTabBrush : (HBRUSH)(COLOR_BTNFACE + 1));
    ReleaseDC(hwnd, hdc);
}

static void PaintTabTopSeam(HWND hwnd, HDC hdc) {
    if (!hwnd || !hdc || !ShouldUseDarkTheme()) return;
    RECT rc;
    GetClientRect(hwnd, &rc);
    if (rc.right <= rc.left || rc.bottom <= rc.top) return;
    RECT seam = rc;
    seam.bottom = seam.top + 2;
    HRGN seamRgn = CreateRectRgnIndirect(&seam);
    if (!seamRgn) { FillRect(hdc, &seam, g_hTabBrush ? g_hTabBrush : (HBRUSH)(COLOR_BTNFACE + 1)); return; }
    int count = TabCtrl_GetItemCount(hwnd);
    for (int i = 0; i < count; ++i) {
        RECT itemRc;
        if (TabCtrl_GetItemRect(hwnd, i, &itemRc)) {
            HRGN itemRgn = CreateRectRgnIndirect(&itemRc);
            if (itemRgn) { CombineRgn(seamRgn, seamRgn, itemRgn, RGN_DIFF); DeleteObject(itemRgn); }
        }
    }
    SelectClipRgn(hdc, seamRgn);
    FillRect(hdc, &seam, g_hTabBrush ? g_hTabBrush : (HBRUSH)(COLOR_BTNFACE + 1));
    SelectClipRgn(hdc, NULL);
    DeleteObject(seamRgn);
}

static void PaintTabDarkBorders(HWND hwnd, HDC hdc) {
    if (!hwnd || !hdc || !ShouldUseDarkTheme()) return;
    RECT rc;
    GetClientRect(hwnd, &rc);
    if (rc.right <= rc.left || rc.bottom <= rc.top) return;
    HPEN outerPen = CreatePen(PS_SOLID, 1, ThemeTabSubtleBorderColor());
    HPEN pagePen = CreatePen(PS_SOLID, 1, ThemeTabBorderColor());
    if (!outerPen || !pagePen) { if (outerPen) DeleteObject(outerPen); if (pagePen) DeleteObject(pagePen); return; }
    HPEN oldPen = (HPEN)SelectObject(hdc, outerPen);
    MoveToEx(hdc, rc.left, rc.top, NULL); LineTo(hdc, rc.right, rc.top);
    MoveToEx(hdc, rc.left, rc.bottom - 1, NULL); LineTo(hdc, rc.right, rc.bottom - 1);
    MoveToEx(hdc, rc.left, rc.top, NULL); LineTo(hdc, rc.left, rc.bottom);
    MoveToEx(hdc, rc.right - 1, rc.top, NULL); LineTo(hdc, rc.right - 1, rc.bottom);
    RECT pageRc = rc;
    TabCtrl_AdjustRect(hwnd, FALSE, &pageRc);
    if (pageRc.right > pageRc.left && pageRc.bottom > pageRc.top) {
        RECT frame = pageRc;
        InflateRect(&frame, 1, 1);
        SelectObject(hdc, pagePen);
        MoveToEx(hdc, frame.left, frame.top, NULL); LineTo(hdc, frame.right, frame.top);
        MoveToEx(hdc, frame.left, frame.bottom - 1, NULL); LineTo(hdc, frame.right, frame.bottom - 1);
        MoveToEx(hdc, frame.left, frame.top, NULL); LineTo(hdc, frame.left, frame.bottom);
        MoveToEx(hdc, frame.right - 1, frame.top, NULL); LineTo(hdc, frame.right - 1, frame.bottom);
    }
    SelectObject(hdc, oldPen);
    DeleteObject(pagePen);
    DeleteObject(outerPen);
}

static void PaintTabBackgroundRemainder(HWND hwnd, HDC hdc) {
    if (!hwnd || !hdc) return;
    RECT rc;
    GetClientRect(hwnd, &rc);
    HRGN paintRgn = CreateRectRgnIndirect(&rc);
    if (!paintRgn) { FillRect(hdc, &rc, g_hTabBrush ? g_hTabBrush : (HBRUSH)(COLOR_BTNFACE + 1)); return; }
    int count = TabCtrl_GetItemCount(hwnd);
    for (int i = 0; i < count; ++i) {
        RECT itemRc;
        if (TabCtrl_GetItemRect(hwnd, i, &itemRc)) {
            if (!ShouldUseDarkTheme()) InflateRect(&itemRc, 2, 2);
            HRGN itemRgn = CreateRectRgnIndirect(&itemRc);
            if (itemRgn) { CombineRgn(paintRgn, paintRgn, itemRgn, RGN_DIFF); DeleteObject(itemRgn); }
        }
    }
    SelectClipRgn(hdc, paintRgn);
    FillRect(hdc, &rc, g_hTabBrush ? g_hTabBrush : (HBRUSH)(COLOR_BTNFACE + 1));
    SelectClipRgn(hdc, NULL);
    DeleteObject(paintRgn);
}

static void DrawTabOwnerItem(DRAWITEMSTRUCT* dis) {
    if (!dis || dis->CtlType != ODT_TAB || !g_hwndTab) return;
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    int idx = (int)dis->itemID;
    bool selected = (idx == TabCtrl_GetCurSel(g_hwndTab));

    // In dark theme the native tab control can leave a 1px light edge around
    // non-selected tab items.  Paint one pixel outside the item rectangle first
    // so that any native edge is overwritten with our dark tab color.
    RECT paintRc = rc;
    if (ShouldUseDarkTheme() && !selected) InflateRect(&paintRc, 1, 1);

    HBRUSH bg = CreateSolidBrush(ThemeTabItemBg(selected));
    FillRect(hdc, &paintRc, bg);
    DeleteObject(bg);

    HPEN pen = CreatePen(PS_SOLID, 1, selected ? ThemeTabBorderColor() : ThemeInactiveTabBorderColor());
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, paintRc.left, paintRc.top, paintRc.right, paintRc.bottom);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    wchar_t text[260];
    TCITEMW item;
    ZeroMemory(&item, sizeof(item));
    item.mask = TCIF_TEXT;
    item.pszText = text;
    item.cchTextMax = 260;
    text[0] = 0;
    TabCtrl_GetItem(g_hwndTab, idx, &item);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, ThemeTextColor());
    HFONT oldFont = (HFONT)SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
    InflateRect(&rc, -8, 0);
    DrawTextW(hdc, text, -1, &rc, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
    SelectObject(hdc, oldFont);
}

static void PaintAllTabItemsDark(HWND hwnd, HDC hdc) {
    if (!hwnd || !hdc || !ShouldUseDarkTheme()) return;
    int count = TabCtrl_GetItemCount(hwnd);
    int selected = TabCtrl_GetCurSel(hwnd);

    for (int pass = 0; pass < 2; ++pass) {
        for (int i = 0; i < count; ++i) {
            if ((pass == 0 && i == selected) || (pass == 1 && i != selected)) continue;
            RECT itemRc;
            if (!TabCtrl_GetItemRect(hwnd, i, &itemRc)) continue;
            DRAWITEMSTRUCT dis;
            ZeroMemory(&dis, sizeof(dis));
            dis.CtlType = ODT_TAB;
            dis.itemID = (UINT)i;
            dis.hwndItem = hwnd;
            dis.hDC = hdc;
            dis.rcItem = itemRc;
            DrawTabOwnerItem(&dis);
        }
    }
}

static RECT StatusGripRect(HWND hwnd) {
    RECT clientRc;
    GetClientRect(hwnd, &clientRc);
    int cx = max_int(DpiScale(hwnd, 17), GetSystemMetrics(SM_CXVSCROLL));
    int cy = max_int(DpiScale(hwnd, 17), GetSystemMetrics(SM_CYHSCROLL));
    RECT grip = clientRc;
    grip.left = max_int(clientRc.left, clientRc.right - cx);
    grip.top = max_int(clientRc.top, clientRc.bottom - cy);
    return grip;
}

static bool StatusBarPointInResizeGrip(HWND hwnd, POINT ptScreen) {
    if (!hwnd) return false;
    if (IsZoomed(g_hwndMain)) return false;
    POINT pt = ptScreen;
    ScreenToClient(hwnd, &pt);
    RECT grip = StatusGripRect(hwnd);
    return PtInRect(&grip, pt) != 0;
}

static void PaintStatusBarClient(HWND hwnd, HDC hdc) {
    if (!hwnd || !hdc) return;
    RECT rc;
    GetClientRect(hwnd, &rc);
    FillRect(hdc, &rc, g_hStatusBrush ? g_hStatusBrush : (HBRUSH)(COLOR_BTNFACE + 1));
    HPEN pen = CreatePen(PS_SOLID, 1, ThemeStatusSeparatorColor());
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, rc.left, rc.top, NULL);
    LineTo(hdc, rc.right, rc.top);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
    RECT textRc = rc;
    textRc.left += DpiScale(hwnd, 8);
    textRc.right -= DpiScale(hwnd, 8);
    RECT grip = StatusGripRect(hwnd);
    textRc.right = min_int(textRc.right, grip.left - DpiScale(hwnd, 4));
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, ThemeTextColor());
    HFONT oldFont = (HFONT)SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
    DrawTextW(hdc, g_statusText ? g_statusText : L"", -1, &textRc, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
    SelectObject(hdc, oldFont);
    FillRect(hdc, &grip, g_hStatusBrush ? g_hStatusBrush : (HBRUSH)(COLOR_BTNFACE + 1));
    HPEN gripPen = CreatePen(PS_SOLID, 1, ThemeResizeGripColor());
    HPEN oldGripPen = (HPEN)SelectObject(hdc, gripPen);
    for (int i = 0; i < 3; ++i) {
        int offset = 4 + i * 4;
        MoveToEx(hdc, grip.right - offset, grip.bottom - 2, NULL);
        LineTo(hdc, grip.right - 2, grip.bottom - offset);
    }
    SelectObject(hdc, oldGripPen);
    DeleteObject(gripPen);
}


static void UpdateBrightCaret(HWND edit) {
    if (!edit || GetFocus() != edit) return;

    if (!ShouldUseDarkTheme()) {
        DestroyCaret();
        return;
    }

    CHARRANGE cr;
    ZeroMemory(&cr, sizeof(cr));
    SendMessageW(edit, EM_EXGETSEL, 0, (LPARAM)&cr);
    if (cr.cpMin != cr.cpMax) {
        HideCaret(edit);
        return;
    }

    POINT pt;
    ZeroMemory(&pt, sizeof(pt));
    LRESULT pos = SendMessageW(edit, EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)cr.cpMin);
    if (pos == -1) return;

    LONG line = (LONG)SendMessageW(edit, EM_LINEFROMCHAR, (WPARAM)cr.cpMin, 0);
    LONG lineIndex = (LONG)SendMessageW(edit, EM_LINEINDEX, (WPARAM)line, 0);
    int lineHeight = DpiScale(edit, 18);
    if (lineIndex >= 0) {
        POINT pt2;
        ZeroMemory(&pt2, sizeof(pt2));
        if (SendMessageW(edit, EM_POSFROMCHAR, (WPARAM)&pt2, (LPARAM)lineIndex) != -1) {
            LONG nextIndex = (LONG)SendMessageW(edit, EM_LINEINDEX, (WPARAM)(line + 1), 0);
            if (nextIndex >= 0) {
                POINT pt3;
                ZeroMemory(&pt3, sizeof(pt3));
                if (SendMessageW(edit, EM_POSFROMCHAR, (WPARAM)&pt3, (LPARAM)nextIndex) != -1 && pt3.y > pt2.y) {
                    lineHeight = pt3.y - pt2.y;
                }
            }
        }
    }

    HideCaret(edit);
    DestroyCaret();
    CreateCaret(edit, NULL, max_int(2, DpiScale(edit, 2)), max_int(DpiScale(edit, 14), lineHeight));
    SetCaretPos(pt.x, pt.y);
    ShowCaret(edit);
}

static void ShowEditorContextMenu(HWND edit, POINT ptScreen) {
    if (!edit) return;
    if (ptScreen.x == -1 && ptScreen.y == -1) {
        CHARRANGE cr;
        ZeroMemory(&cr, sizeof(cr));
        SendMessageW(edit, EM_EXGETSEL, 0, (LPARAM)&cr);
        POINT pt;
        ZeroMemory(&pt, sizeof(pt));
        if (SendMessageW(edit, EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)cr.cpMin) == -1) {
            pt.x = 8;
            pt.y = 8;
        }
        ClientToScreen(edit, &pt);
        ptScreen = pt;
    }

    HMENU menu = CreatePopupMenu();
    if (!menu) return;

    BOOL canUndo = (BOOL)SendMessageW(edit, EM_CANUNDO, 0, 0);
    BOOL hasSel = FALSE;
    CHARRANGE cr;
    ZeroMemory(&cr, sizeof(cr));
    SendMessageW(edit, EM_EXGETSEL, 0, (LPARAM)&cr);
    hasSel = cr.cpMax > cr.cpMin;
    BOOL canPaste = IsClipboardFormatAvailable(CF_UNICODETEXT) || IsClipboardFormatAvailable(CF_TEXT);

    AppendMenuW(menu, MF_STRING | (canUndo ? MF_ENABLED : MF_GRAYED), ID_EDIT_CONTEXT_UNDO, T(L"context_undo", L"Undo"));
    AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(menu, MF_STRING | (hasSel ? MF_ENABLED : MF_GRAYED), ID_EDIT_CONTEXT_CUT, T(L"context_cut", L"Cut"));
    AppendMenuW(menu, MF_STRING | (hasSel ? MF_ENABLED : MF_GRAYED), ID_EDIT_CONTEXT_COPY, T(L"context_copy", L"Copy"));
    AppendMenuW(menu, MF_STRING | (canPaste ? MF_ENABLED : MF_GRAYED), ID_EDIT_CONTEXT_PASTE, T(L"context_paste", L"Paste"));
    AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(menu, MF_STRING, ID_EDIT_CONTEXT_SELECT_ALL, T(L"context_select_all", L"Select All"));

    int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, ptScreen.x, ptScreen.y, 0, g_hwndMain, NULL);
    DestroyMenu(menu);

    switch (cmd) {
    case ID_EDIT_CONTEXT_UNDO:
        SendMessageW(edit, EM_UNDO, 0, 0);
        break;
    case ID_EDIT_CONTEXT_CUT:
        SendMessageW(edit, WM_CUT, 0, 0);
        break;
    case ID_EDIT_CONTEXT_COPY:
        SendMessageW(edit, WM_COPY, 0, 0);
        break;
    case ID_EDIT_CONTEXT_PASTE:
        SendMessageW(edit, WM_PASTE, 0, 0);
        break;
    case ID_EDIT_CONTEXT_SELECT_ALL:
        SendMessageW(edit, EM_SETSEL, 0, -1);
        break;
    default:
        break;
    }
    UpdateBrightCaret(edit);
}

static LRESULT CALLBACK ThemedEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_SETFOCUS: {
        LRESULT r = g_oldEditProc ? CallWindowProcW(g_oldEditProc, hwnd, msg, wParam, lParam) : DefWindowProcW(hwnd, msg, wParam, lParam);
        UpdateBrightCaret(hwnd);
        return r;
    }
    case WM_KILLFOCUS:
        DestroyCaret();
        break;
    case WM_KEYUP:
    case WM_LBUTTONUP:
        if (ShouldUseDarkTheme()) {
            LRESULT r = g_oldEditProc ? CallWindowProcW(g_oldEditProc, hwnd, msg, wParam, lParam) : DefWindowProcW(hwnd, msg, wParam, lParam);
            UpdateBrightCaret(hwnd);
            return r;
        }
        break;
    case WM_CONTEXTMENU: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ShowEditorContextMenu(hwnd, pt);
        return 0;
    }
    }
    return g_oldEditProc ? CallWindowProcW(g_oldEditProc, hwnd, msg, wParam, lParam) : DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK ThemedTabProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if ((msg == WM_COMMAND || msg == WM_NOTIFY) && g_hwndMain) {
        return SendMessageW(g_hwndMain, msg, wParam, lParam);
    }
    if (msg == WM_ERASEBKGND) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect((HDC)wParam, &rc, g_hTabBrush ? g_hTabBrush : (HBRUSH)(COLOR_BTNFACE + 1));
        return 1;
    }
    if (msg == WM_NCPAINT && ShouldUseDarkTheme()) return 0;
    if (msg == WM_PAINT) {
        LRESULT result = g_oldTabProc ? CallWindowProcW(g_oldTabProc, hwnd, msg, wParam, lParam) : DefWindowProcW(hwnd, msg, wParam, lParam);
        HDC hdc = GetDC(hwnd);
        if (hdc) {
            PaintTabBackgroundRemainder(hwnd, hdc);
            PaintAllTabItemsDark(hwnd, hdc);
            PaintTabTopSeam(hwnd, hdc);
            PaintTabDarkBorders(hwnd, hdc);
            ReleaseDC(hwnd, hdc);
        }
        return result;
    }
    return g_oldTabProc ? CallWindowProcW(g_oldTabProc, hwnd, msg, wParam, lParam) : DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK ThemedStatusProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ERASEBKGND: {
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect((HDC)wParam, &rc, g_hStatusBrush ? g_hStatusBrush : (HBRUSH)(COLOR_BTNFACE + 1));
        return 1;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (hdc) PaintStatusBarClient(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_NCPAINT: {
        if (ShouldUseDarkTheme()) {
            HDC hdc = GetWindowDC(hwnd);
            if (hdc) {
                RECT wr;
                GetWindowRect(hwnd, &wr);
                OffsetRect(&wr, -wr.left, -wr.top);
                FillRect(hdc, &wr, g_hStatusBrush ? g_hStatusBrush : (HBRUSH)(COLOR_BTNFACE + 1));
                ReleaseDC(hwnd, hdc);
            }
            return 0;
        }
        break;
    }
    case WM_NCHITTEST: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (StatusBarPointInResizeGrip(hwnd, pt)) return HTBOTTOMRIGHT;
        break;
    }
    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ClientToScreen(hwnd, &pt);
        if (StatusBarPointInResizeGrip(hwnd, pt)) {
            SendMessageW(g_hwndMain, WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, MAKELPARAM(pt.x, pt.y));
            return 0;
        }
        break;
    }
    }
    return g_oldStatusProc ? CallWindowProcW(g_oldStatusProc, hwnd, msg, wParam, lParam) : DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void ApplyThemeToApp(void) {
    RebuildThemeBrushes();
    ApplyThemeToAllEditors();
    ApplyDarkWindowAttributes(g_hwndMain);
    bool dark = ShouldUseDarkTheme();
    if (g_hwndTab) {
        if (dark) SetWindowTheme(g_hwndTab, L"", L"");
        else SetWindowTheme(g_hwndTab, L"Explorer", NULL);
        SendMessageW(g_hwndTab, TCM_SETBKCOLOR, 0, (LPARAM)ThemeTabBg());
        SendMessageW(g_hwndTab, TCM_SETTEXTCOLOR, 0, (LPARAM)ThemeTextColor());
        RedrawWindow(g_hwndTab, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
    }
    if (g_hwndStatus) {
        if (dark) SetWindowTheme(g_hwndStatus, L"", L"");
        else SetWindowTheme(g_hwndStatus, L"Explorer", NULL);
        StripStatusBarSystemChrome(g_hwndStatus);
        UpdateStatusBar();
        RedrawWindow(g_hwndStatus, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
    }
    if (g_hwndMain) {
        ApplyThemeToMenus(GetMenu(g_hwndMain));
        DrawMenuBar(g_hwndMain);
        RedrawWindow(g_hwndMain, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
        PaintMenuBarBackgroundRemainder(g_hwndMain);
        PaintMenuTabSeam(g_hwndMain);
    }
}

static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == g_findMsg && g_findMsg != 0) {
        HandleFindReplaceMessage(lParam);
        return 0;
    }
    switch (msg) {
    case WM_CREATE: {
        g_hwndMain = hwnd;
        LoadRecentFiles();
        UpdateRecentMenu();
        EnsureDefaultFont();
        g_hwndTab = CreateWindowExW(0, WC_TABCONTROLW, L"",
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TCS_OWNERDRAWFIXED,
            0, 0, 100, 100, hwnd, NULL, g_hInst, NULL);
        SendMessageW(g_hwndTab, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        g_oldTabProc = (WNDPROC)SetWindowLongPtrW(g_hwndTab, GWLP_WNDPROC, (LONG_PTR)ThemedTabProc);
        g_hwndStatus = CreateWindowExW(0, STATUSCLASSNAMEW, L"",
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CCS_NODIVIDER,
            0, 0, 0, 0, hwnd, NULL, g_hInst, NULL);
        SendMessageW(g_hwndStatus, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
        g_oldStatusProc = (WNDPROC)SetWindowLongPtrW(g_hwndStatus, GWLP_WNDPROC, (LONG_PTR)ThemedStatusProc);
        StripStatusBarSystemChrome(g_hwndStatus);
        ApplyThemeToApp();
        if (g_initialFiles.count == 0) {
            CreateEmptyTab();
        } else {
            bool opened = false;
            for (int i = 0; i < g_initialFiles.count; ++i) {
                if (LoadFileIntoTab(g_initialFiles.data[i], ENC_AUTO, EOL_AUTO, NULL)) opened = true;
            }
            if (!opened) CreateEmptyTab();
        }
        LayoutChildren(hwnd);
        PaintMenuTabSeam(hwnd);
        UpdateStatusBar();
        return 0;
    }
    case WM_SIZE:
        LayoutChildren(hwnd);
        PaintMenuTabSeam(hwnd);
        if (g_hwndStatus) RedrawWindow(g_hwndStatus, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
        return 0;
    case WM_NCHITTEST: {
        LRESULT hit = DefWindowProcW(hwnd, msg, wParam, lParam);
        if (hit == HTCLIENT) {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            if (IsPointInTabHeaderGap(pt)) return HTCAPTION;
        }
        return hit;
    }
    case WM_DPICHANGED: {
        RECT* suggested = (RECT*)lParam;
        if (suggested) {
            SetWindowPos(hwnd, NULL, suggested->left, suggested->top,
                suggested->right - suggested->left, suggested->bottom - suggested->top,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        LayoutChildren(hwnd);
        ApplyThemeToApp();
        return 0;
    }
    case WM_NOTIFY: {
        NMHDR* nm = (NMHDR*)lParam;
        if (nm && nm->hwndFrom == g_hwndTab && nm->code == NM_CUSTOMDRAW) {
            NMCUSTOMDRAW* cd = (NMCUSTOMDRAW*)lParam;
            if (cd->dwDrawStage == CDDS_PREPAINT) {
                RECT rc;
                GetClientRect(g_hwndTab, &rc);
                FillRect(cd->hdc, &rc, g_hTabBrush ? g_hTabBrush : (HBRUSH)(COLOR_BTNFACE + 1));
                return CDRF_NOTIFYITEMDRAW;
            }
            if (cd->dwDrawStage == CDDS_ITEMPREPAINT) {
                int idx = (int)cd->dwItemSpec;
                DRAWITEMSTRUCT dis;
                ZeroMemory(&dis, sizeof(dis));
                dis.CtlType = ODT_TAB;
                dis.itemID = (UINT)idx;
                dis.hwndItem = g_hwndTab;
                dis.hDC = cd->hdc;
                TabCtrl_GetItemRect(g_hwndTab, idx, &dis.rcItem);
                DrawTabOwnerItem(&dis);
                return CDRF_SKIPDEFAULT;
            }
        }
        if (nm && nm->hwndFrom == g_hwndStatus && nm->code == NM_CUSTOMDRAW) {
            NMCUSTOMDRAW* cd = (NMCUSTOMDRAW*)lParam;
            if (cd->dwDrawStage == CDDS_PREPAINT) {
                PaintStatusBarClient(g_hwndStatus, cd->hdc);
                return CDRF_SKIPDEFAULT;
            }
        }
        if (nm && nm->hwndFrom == g_hwndTab && nm->code == TCN_SELCHANGE) {
            int idx = TabCtrl_GetCurSel(g_hwndTab);
            SetCurrentTab(idx);
            return 0;
        }
        TabDoc* doc = CurrentDoc();
        if (doc && nm && nm->hwndFrom == doc->edit) {
            if (nm->code == EN_SELCHANGE) { UpdateStatusBar(); UpdateBrightCaret(doc->edit); return 0; }
        }
        break;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);
        HWND from = (HWND)lParam;
        TabDoc* doc = CurrentDoc();
        if (doc && from == doc->edit && code == EN_CHANGE) {
            UpdateTabTitle(g_current);
            UpdateStatusBar();
            return 0;
        }
        OnCommand(hwnd, id);
        return 0;
    }
    case WM_INITMENUPOPUP:
        UpdateMenuChecks(GetMenu(hwnd));
        ApplyThemeToMenus(GetMenu(hwnd));
        DrawMenuBar(hwnd);
        PaintMenuBarBackgroundRemainder(hwnd);
        PaintMenuTabSeam(hwnd);
        return 0;
    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
        if (dis && dis->CtlType == ODT_MENU) { DrawMenuItem(dis); return TRUE; }
        if (dis && dis->CtlType == ODT_TAB && dis->hwndItem == g_hwndTab) { DrawTabOwnerItem(dis); return TRUE; }
        break;
    }
    case WM_MEASUREITEM: {
        MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
        if (mis && mis->CtlType == ODT_MENU) { MeasureMenuItem(mis); return TRUE; }
        if (mis && mis->CtlType == ODT_TAB) {
            mis->itemWidth = (UINT)DpiScale(hwnd, 180);
            mis->itemHeight = (UINT)max_int(DpiScale(hwnd, 24), GetSystemMetrics(SM_CYMENU) + 2);
            return TRUE;
        }
        break;
    }
    case WM_ERASEBKGND: {
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect((HDC)wParam, &rc, g_hMainBrush ? g_hMainBrush : (HBRUSH)(COLOR_BTNFACE + 1));
        return 1;
    }
    case WM_NCPAINT: {
        LRESULT r = DefWindowProcW(hwnd, msg, wParam, lParam);
        PaintMenuBarBackgroundRemainder(hwnd);
        PaintMenuTabSeam(hwnd);
        return r;
    }
    case WM_NCACTIVATE: {
        LRESULT r = DefWindowProcW(hwnd, msg, wParam, lParam);
        PaintMenuBarBackgroundRemainder(hwnd);
        PaintMenuTabSeam(hwnd);
        return r;
    }
    case WM_SETTINGCHANGE:
        if (g_themeMode == THEME_AUTO) ApplyThemeToApp();
        return 0;
    case WM_THEMECHANGED:
    case WM_SYSCOLORCHANGE:
        ApplyThemeToApp();
        return 0;
    case WM_CLOSE:
        if (ConfirmCloseAll()) DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        SaveRecentFiles();
        for (int i = 0; i < g_docs.count; ++i) {
            if (g_docs.data[i]->edit) DestroyWindow(g_docs.data[i]->edit);
            FreeTabDoc(g_docs.data[i]);
        }
        free(g_docs.data); g_docs.data = NULL; g_docs.count = g_docs.cap = 0;
        if (g_hFont) { DeleteObject(g_hFont); g_hFont = NULL; }
        if (g_hMainBrush) { DeleteObject(g_hMainBrush); g_hMainBrush = NULL; }
        if (g_hPanelBrush) { DeleteObject(g_hPanelBrush); g_hPanelBrush = NULL; }
        if (g_hMenuBrush) { DeleteObject(g_hMenuBrush); g_hMenuBrush = NULL; }
        if (g_hTabBrush) { DeleteObject(g_hTabBrush); g_hTabBrush = NULL; }
        if (g_hStatusBrush) { DeleteObject(g_hStatusBrush); g_hStatusBrush = NULL; }
        FreeMenuDrawData();
        wvec_clear(&g_recentFiles);
        wvec_clear(&g_initialFiles);
        free(g_iniPath); g_iniPath = NULL;
        free(g_statusText); g_statusText = NULL;
        free(g_lastFindText); g_lastFindText = NULL;
        if (g_hAccel) { DestroyAcceleratorTable(g_hAccel); g_hAccel = NULL; }
        if (g_hRichEdit) { FreeLibrary(g_hRichEdit); g_hRichEdit = NULL; }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool RegisterWindowClasses(void) {
    WNDCLASSW wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = g_hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"PlainEditorMainWindow";
    if (!RegisterClassW(&wc)) return false;
    WNDCLASSW od;
    ZeroMemory(&od, sizeof(od));
    od.lpfnWndProc = OptionDialogProc;
    od.hInstance = g_hInst;
    od.hCursor = LoadCursor(NULL, IDC_ARROW);
    od.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    od.lpszClassName = L"PlainEditorOptionDialog";
    if (!RegisterClassW(&od)) return false;
    WNDCLASSW id;
    ZeroMemory(&id, sizeof(id));
    id.lpfnWndProc = InputDialogProc;
    id.hInstance = g_hInst;
    id.hCursor = LoadCursor(NULL, IDC_ARROW);
    id.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    id.lpszClassName = L"PlainEditorInputDialog";
    if (!RegisterClassW(&id)) return false;
    return true;
}

static void CreateAccelerators(void) {
    ACCEL accels[] = {
        { FVIRTKEY | FCONTROL, 'T', ID_FILE_NEW_TAB },
        { FVIRTKEY | FCONTROL, 'N', ID_FILE_NEW_WINDOW },
        { FVIRTKEY | FCONTROL, 'O', ID_FILE_OPEN },
        { FVIRTKEY | FCONTROL, 'S', ID_FILE_SAVE },
        { FVIRTKEY | FCONTROL | FSHIFT, 'S', ID_FILE_SAVE_AS },
        { FVIRTKEY | FCONTROL, 'W', ID_FILE_CLOSE_TAB },
        { FVIRTKEY | FCONTROL, 'Z', ID_EDIT_UNDO },
        { FVIRTKEY | FCONTROL, 'Y', ID_EDIT_REDO },
        { FVIRTKEY | FCONTROL, 'X', ID_EDIT_CUT },
        { FVIRTKEY | FCONTROL, 'C', ID_EDIT_COPY },
        { FVIRTKEY | FCONTROL, 'V', ID_EDIT_PASTE },
        { FVIRTKEY | FCONTROL, 'F', ID_EDIT_FIND },
        { FVIRTKEY, VK_F3, ID_EDIT_FIND_NEXT },
        { FVIRTKEY | FSHIFT, VK_F3, ID_EDIT_FIND_PREV },
        { FVIRTKEY | FCONTROL, 'H', ID_EDIT_REPLACE },
        { FVIRTKEY | FCONTROL, 'G', ID_EDIT_GOTO },
        { FVIRTKEY | FCONTROL, 'A', ID_EDIT_SELECT_ALL },
        { FVIRTKEY | FCONTROL, VK_OEM_PLUS, ID_VIEW_ZOOM_IN },
        { FVIRTKEY | FCONTROL, VK_ADD, ID_VIEW_ZOOM_IN },
        { FVIRTKEY | FCONTROL, VK_OEM_MINUS, ID_VIEW_ZOOM_OUT },
        { FVIRTKEY | FCONTROL, VK_SUBTRACT, ID_VIEW_ZOOM_OUT },
        { FVIRTKEY | FCONTROL, '0', ID_VIEW_ZOOM_RESET }
    };
    g_hAccel = CreateAcceleratorTableW(accels, (int)(sizeof(accels) / sizeof(accels[0])));
}

static void ParseInitialFiles(void) {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return;
    for (int i = 1; i < argc; ++i) {
        if (argv[i] && argv[i][0]) wvec_push_dup(&g_initialFiles, argv[i]);
    }
    LocalFree(argv);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;
    g_hInst = hInstance;
    EnableDpiAwareness();
    BuildIniPath();
    LoadAppSettings();
    ParseInitialFiles();
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_TAB_CLASSES | ICC_BAR_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icc);
    g_hRichEdit = LoadLibraryW(L"Msftedit.dll");
    if (g_hRichEdit) g_richEditClass = RICHEDIT50W_CLASS_NAME;
    else {
        g_hRichEdit = LoadLibraryW(L"Riched20.dll");
        g_richEditClass = RICHEDIT20W_CLASS_NAME;
    }
    if (!g_hRichEdit) {
        MessageBoxW(NULL, T(L"msg_richedit_failed", L"Could not load the RichEdit DLL."), T(L"title_error", L"Error"), MB_OK | MB_ICONERROR);
        return 1;
    }
    g_findMsg = RegisterWindowMessageW(FINDMSGSTRINGW);
    ZeroMemory(g_findBuffer, sizeof(g_findBuffer));
    ZeroMemory(g_replaceBuffer, sizeof(g_replaceBuffer));
    EnsureDefaultFont();
    if (!RegisterWindowClasses()) {
        MessageBoxW(NULL, T(L"msg_register_class_failed", L"Could not register the window class."), T(L"title_error", L"Error"), MB_OK | MB_ICONERROR);
        return 1;
    }
    CreateAccelerators();
    RebuildThemeBrushes();
    HMENU menu = CreateMainMenu();
    DWORD mainStyle = WS_OVERLAPPEDWINDOW & ~(DWORD)WS_CAPTION;
    HWND hwnd = CreateWindowExW(0, L"PlainEditorMainWindow", T(L"app_title", L"Win32 Tabbed Plain Text Editor"),
        mainStyle, CW_USEDEFAULT, CW_USEDEFAULT, DpiScale(NULL, 920), DpiScale(NULL, 680),
        NULL, menu, hInstance, NULL);
    if (!hwnd) {
        MessageBoxW(NULL, T(L"msg_create_window_failed", L"Could not create the main window."), T(L"title_error", L"Error"), MB_OK | MB_ICONERROR);
        return 1;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (g_hwndFindReplace && IsDialogMessageW(g_hwndFindReplace, &msg)) continue;
        if (g_hAccel && TranslateAcceleratorW(hwnd, g_hAccel, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}
