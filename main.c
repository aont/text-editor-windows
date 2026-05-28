#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <uxtheme.h>
#include <stdio.h>
#include <wchar.h>
#include <windowsx.h>

#pragma comment(lib, "comctl32.lib")

#define MAX_TABS 32
#define TAB_BAR_HEIGHT 34
#define MENU_BAR_HEIGHT 26
#define STATUS_BAR_HEIGHT 24
#define TAB_WIDTH 160

#define IDM_FILE_NEW_TAB 1001
#define IDM_FILE_NEW_WINDOW 1002
#define IDM_FILE_OPEN 1003
#define IDM_FILE_RECENT 1004
#define IDM_FILE_SAVE 1005
#define IDM_FILE_SAVE_AS 1006
#define IDM_FILE_CHANGE_ENCODING 1007
#define IDM_FILE_RELOAD_ENCODING 1008
#define IDM_FILE_CLOSE_TAB 1009
#define IDM_FILE_CLOSE_WINDOW 1010
#define IDM_FILE_EXIT 1011

#define IDM_EDIT_UNDO 1101
#define IDM_EDIT_REDO 1102
#define IDM_EDIT_CUT 1103
#define IDM_EDIT_COPY 1104
#define IDM_EDIT_PASTE 1105
#define IDM_EDIT_DELETE 1106
#define IDM_EDIT_FIND 1107
#define IDM_EDIT_FIND_NEXT 1108
#define IDM_EDIT_FIND_PREV 1109
#define IDM_EDIT_REPLACE 1110
#define IDM_EDIT_GOTO 1111
#define IDM_EDIT_SELECT_ALL 1112
#define IDM_EDIT_FONT 1113

#define IDM_VIEW_ZOOM_IN 1201
#define IDM_VIEW_ZOOM_OUT 1202
#define IDM_VIEW_ZOOM_DEFAULT 1203
#define IDM_VIEW_STATUS_BAR 1204
#define IDM_VIEW_WORD_WRAP 1205
#define IDM_VIEW_THEME_LIGHT 1206
#define IDM_VIEW_THEME_DARK 1207
#define IDM_VIEW_THEME_AUTO 1208

typedef enum ThemeMode { THEME_LIGHT, THEME_DARK, THEME_AUTO } ThemeMode;
typedef enum FileEncoding { ENC_UTF8, ENC_UTF16LE, ENC_ANSI } FileEncoding;
typedef enum LineEnding { LE_CRLF, LE_LF, LE_CR } LineEnding;

typedef struct TabDoc {
    WCHAR title[MAX_PATH];
    WCHAR path[MAX_PATH];
    BOOL dirty;
    FileEncoding enc;
    LineEnding le;
} TabDoc;

static HINSTANCE g_hInst;
static HWND g_hwndMain, g_hwndEdit, g_hwndStatus;
static HMENU g_menuMain;
static TabDoc g_tabs[MAX_TABS];
static int g_tabCount = 0;
static int g_activeTab = 0;
static ThemeMode g_themeMode = THEME_AUTO;
static BOOL g_themeDark = FALSE;
static int g_zoom = 100;
static HFONT g_fontEdit;

static const WCHAR* EncToText(FileEncoding e){ return e==ENC_UTF8?L"UTF-8":(e==ENC_UTF16LE?L"UTF-16 LE":L"ANSI"); }
static const WCHAR* LEToText(LineEnding le){ return le==LE_LF?L"Unix (LF)":(le==LE_CR?L"Classic Mac (CR)":L"Windows (CRLF)"); }

static BOOL SystemPrefersDark(void){
    HKEY hKey; DWORD v=1, sz=sizeof(v);
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",0,KEY_READ,&hKey)==ERROR_SUCCESS){
        RegQueryValueExW(hKey,L"AppsUseLightTheme",NULL,NULL,(LPBYTE)&v,&sz);
        RegCloseKey(hKey);
    }
    return v==0;
}

static void UpdateTheme(HWND hwnd){
    g_themeDark = (g_themeMode==THEME_DARK) || (g_themeMode==THEME_AUTO && SystemPrefersDark());
    COLORREF bg = g_themeDark?RGB(30,30,30):RGB(250,250,250);
    (void)bg;
    InvalidateRect(hwnd,NULL,TRUE);
}

static void EnsureDefaultTab(void){
    if(g_tabCount==0){
        wcscpy(g_tabs[0].title,L"Untitled"); g_tabs[0].path[0]=0;
        g_tabs[0].enc=ENC_UTF8; g_tabs[0].le=LE_LF; g_tabs[0].dirty=FALSE;
        g_tabCount=1; g_activeTab=0;
    }
}

static void UpdateStatus(void){
    int len = GetWindowTextLengthW(g_hwndEdit);
    DWORD sel=0; SendMessageW(g_hwndEdit, EM_GETSEL, (WPARAM)&sel, (LPARAM)&sel);
    int caret = LOWORD(sel);
    int line = (int)SendMessageW(g_hwndEdit, EM_LINEFROMCHAR, caret, 0) + 1;
    int lineStart = (int)SendMessageW(g_hwndEdit, EM_LINEINDEX, line-1, 0);
    int col = caret - lineStart + 1;
    WCHAR buf[256];
    swprintf(buf,256,L"Line %d, Column %d | %d characters | %d%% | %s | %s", line,col,len,g_zoom,LEToText(g_tabs[g_activeTab].le),EncToText(g_tabs[g_activeTab].enc));
    SetWindowTextW(g_hwndStatus,buf);
}

static void Layout(HWND hwnd){
    RECT rc; GetClientRect(hwnd,&rc);
    int top = TAB_BAR_HEIGHT + MENU_BAR_HEIGHT;
    MoveWindow(g_hwndEdit,0,top,rc.right,rc.bottom-top-STATUS_BAR_HEIGHT,TRUE);
    MoveWindow(g_hwndStatus,0,rc.bottom-STATUS_BAR_HEIGHT,rc.right,STATUS_BAR_HEIGHT,TRUE);
}

static void SwitchTab(int idx){ if(idx<0||idx>=g_tabCount)return; g_activeTab=idx; SetWindowTextW(g_hwndEdit,L""); UpdateStatus(); InvalidateRect(g_hwndMain,NULL,TRUE);} // demo
static void AddTab(void){ if(g_tabCount>=MAX_TABS)return; TabDoc *t=&g_tabs[g_tabCount++]; wcscpy(t->title,L"Untitled"); t->path[0]=0; t->dirty=FALSE; t->enc=ENC_UTF8; t->le=LE_LF; SwitchTab(g_tabCount-1);} 

static UINT_PTR CALLBACK FileDlgHook(HWND hdlg, UINT msg, WPARAM wp, LPARAM lp){
    static HWND hEnc, hLe;
    if(msg==WM_INITDIALOG){
        CreateWindowW(L"STATIC",L"Encoding:",WS_CHILD|WS_VISIBLE,10,230,64,20,hdlg,NULL,g_hInst,NULL);
        hEnc=CreateWindowW(L"COMBOBOX",NULL,WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,74,226,120,120,hdlg,(HMENU)201,g_hInst,NULL);
        SendMessageW(hEnc,CB_ADDSTRING,0,(LPARAM)L"UTF-8");SendMessageW(hEnc,CB_ADDSTRING,0,(LPARAM)L"UTF-16 LE");SendMessageW(hEnc,CB_ADDSTRING,0,(LPARAM)L"ANSI");SendMessageW(hEnc,CB_SETCURSEL,0,0);
        CreateWindowW(L"STATIC",L"Line Endings:",WS_CHILD|WS_VISIBLE,210,230,80,20,hdlg,NULL,g_hInst,NULL);
        hLe=CreateWindowW(L"COMBOBOX",NULL,WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,294,226,120,120,hdlg,(HMENU)202,g_hInst,NULL);
        SendMessageW(hLe,CB_ADDSTRING,0,(LPARAM)L"CRLF");SendMessageW(hLe,CB_ADDSTRING,0,(LPARAM)L"LF");SendMessageW(hLe,CB_ADDSTRING,0,(LPARAM)L"CR");SendMessageW(hLe,CB_SETCURSEL,1,0);
    }
    return 0;
}

static void DoOpenSave(BOOL save){
    OPENFILENAMEW ofn={0}; WCHAR path[MAX_PATH]=L"";
    ofn.lStructSize=sizeof(ofn); ofn.hwndOwner=g_hwndMain; ofn.lpstrFile=path; ofn.nMaxFile=MAX_PATH;
    ofn.lpstrFilter=L"Text Files\0*.txt\0All Files\0*.*\0";
    ofn.Flags=OFN_EXPLORER|OFN_ENABLESIZING|OFN_ENABLEHOOK;
    ofn.lpfnHook=FileDlgHook;
    if((save?GetSaveFileNameW(&ofn):GetOpenFileNameW(&ofn))){
        wcsncpy(g_tabs[g_activeTab].path,path,MAX_PATH-1);
        const WCHAR *name = wcsrchr(path,L'\\');
        wcsncpy(g_tabs[g_activeTab].title, name?name+1:path, MAX_PATH-1);
        InvalidateRect(g_hwndMain,NULL,TRUE);
    }
}

static HMENU BuildMenu(void){
    HMENU m=CreateMenu(), file=CreatePopupMenu(), edit=CreatePopupMenu(), view=CreatePopupMenu(), zoom=CreatePopupMenu(), theme=CreatePopupMenu();
    AppendMenuW(file,MF_STRING,IDM_FILE_NEW_TAB,L"New Tab");AppendMenuW(file,MF_STRING,IDM_FILE_NEW_WINDOW,L"New Window");AppendMenuW(file,MF_STRING,IDM_FILE_OPEN,L"Open");AppendMenuW(file,MF_STRING,IDM_FILE_RECENT,L"Recently Opened Files");AppendMenuW(file,MF_STRING,IDM_FILE_SAVE,L"Save");AppendMenuW(file,MF_STRING,IDM_FILE_SAVE_AS,L"Save As");AppendMenuW(file,MF_STRING,IDM_FILE_CHANGE_ENCODING,L"Change Encoding");AppendMenuW(file,MF_STRING,IDM_FILE_RELOAD_ENCODING,L"Reload with Specified Encoding");AppendMenuW(file,MF_STRING,IDM_FILE_CLOSE_TAB,L"Close Tab");AppendMenuW(file,MF_STRING,IDM_FILE_CLOSE_WINDOW,L"Close Window");AppendMenuW(file,MF_STRING,IDM_FILE_EXIT,L"Exit");
    AppendMenuW(edit,MF_STRING,IDM_EDIT_UNDO,L"Undo");AppendMenuW(edit,MF_STRING,IDM_EDIT_REDO,L"Redo");AppendMenuW(edit,MF_STRING,IDM_EDIT_CUT,L"Cut");AppendMenuW(edit,MF_STRING,IDM_EDIT_COPY,L"Copy");AppendMenuW(edit,MF_STRING,IDM_EDIT_PASTE,L"Paste");AppendMenuW(edit,MF_STRING,IDM_EDIT_DELETE,L"Delete");AppendMenuW(edit,MF_STRING,IDM_EDIT_FIND,L"Find");AppendMenuW(edit,MF_STRING,IDM_EDIT_FIND_NEXT,L"Find Next");AppendMenuW(edit,MF_STRING,IDM_EDIT_FIND_PREV,L"Find Previous");AppendMenuW(edit,MF_STRING,IDM_EDIT_REPLACE,L"Replace");AppendMenuW(edit,MF_STRING,IDM_EDIT_GOTO,L"Go To");AppendMenuW(edit,MF_STRING,IDM_EDIT_SELECT_ALL,L"Select All");AppendMenuW(edit,MF_STRING,IDM_EDIT_FONT,L"Font");
    AppendMenuW(zoom,MF_STRING,IDM_VIEW_ZOOM_IN,L"Zoom In");AppendMenuW(zoom,MF_STRING,IDM_VIEW_ZOOM_OUT,L"Zoom Out");AppendMenuW(zoom,MF_STRING,IDM_VIEW_ZOOM_DEFAULT,L"Restore Default Zoom");
    AppendMenuW(theme,MF_STRING,IDM_VIEW_THEME_LIGHT,L"Light");AppendMenuW(theme,MF_STRING,IDM_VIEW_THEME_DARK,L"Dark");AppendMenuW(theme,MF_STRING,IDM_VIEW_THEME_AUTO,L"Auto");
    AppendMenuW(view,MF_POPUP,(UINT_PTR)zoom,L"Zoom");AppendMenuW(view,MF_STRING|MF_CHECKED,IDM_VIEW_STATUS_BAR,L"Status Bar");AppendMenuW(view,MF_STRING,IDM_VIEW_WORD_WRAP,L"Word Wrap");AppendMenuW(view,MF_POPUP,(UINT_PTR)theme,L"App Theme");
    AppendMenuW(m,MF_POPUP,(UINT_PTR)file,L"File");AppendMenuW(m,MF_POPUP,(UINT_PTR)edit,L"Edit");AppendMenuW(m,MF_POPUP,(UINT_PTR)view,L"View");
    return m;
}


static void EnableDpiAwareness(void){
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if(!user32) return;

    typedef BOOL (WINAPI *SetDpiAwarenessContextFn)(HANDLE);
    SetDpiAwarenessContextFn setCtx = (SetDpiAwarenessContextFn)GetProcAddress(user32, "SetProcessDpiAwarenessContext");
    if(setCtx){
        setCtx((HANDLE)-4); /* DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 */
    }
}

static void PaintCustomBars(HWND hwnd, HDC hdc){
    RECT rc; GetClientRect(hwnd,&rc);
    HBRUSH b = CreateSolidBrush(g_themeDark?RGB(24,24,24):RGB(245,245,245));
    RECT tabRc={0,0,rc.right,TAB_BAR_HEIGHT}; FillRect(hdc,&tabRc,b);
    RECT menuRc={0,TAB_BAR_HEIGHT,rc.right,TAB_BAR_HEIGHT+MENU_BAR_HEIGHT}; FillRect(hdc,&menuRc,b);
    DeleteObject(b);
    SetBkMode(hdc,TRANSPARENT); SetTextColor(hdc,g_themeDark?RGB(230,230,230):RGB(30,30,30));
    for(int i=0;i<g_tabCount;i++){
        RECT tr={8+i*TAB_WIDTH,4,8+(i+1)*TAB_WIDTH-4,TAB_BAR_HEIGHT-4};
        HBRUSH tb = CreateSolidBrush(i==g_activeTab?(g_themeDark?RGB(50,50,50):RGB(220,220,220)):(g_themeDark?RGB(35,35,35):RGB(235,235,235)));
        FillRect(hdc,&tr,tb); DeleteObject(tb);
        DrawTextW(hdc,g_tabs[i].title,-1,&tr,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
    case WM_CREATE:{
        g_hwndMain=hwnd; EnsureDefaultTab();
        g_hwndEdit=CreateWindowExW(0,L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,0,0,0,0,hwnd,(HMENU)1,g_hInst,NULL);
        g_hwndStatus=CreateWindowW(L"STATIC",L"",WS_CHILD|WS_VISIBLE|SS_LEFT,0,0,0,0,hwnd,(HMENU)2,g_hInst,NULL);
        g_menuMain=BuildMenu(); SetMenu(hwnd,g_menuMain);
        NONCLIENTMETRICSW ncm={.cbSize=sizeof(ncm)}; SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,sizeof(ncm),&ncm,0); g_fontEdit=CreateFontIndirectW(&ncm.lfMessageFont);
        SendMessageW(g_hwndEdit,WM_SETFONT,(WPARAM)g_fontEdit,TRUE);
        UpdateTheme(hwnd); Layout(hwnd); UpdateStatus();
    }return 0;
    case WM_SIZE: Layout(hwnd); return 0;
    case WM_DPICHANGED:{ RECT* r=(RECT*)lParam; SetWindowPos(hwnd,NULL,r->left,r->top,r->right-r->left,r->bottom-r->top,SWP_NOZORDER|SWP_NOACTIVATE); Layout(hwnd);} return 0;
    case WM_COMMAND:{
        switch(LOWORD(wParam)){
            case IDM_FILE_NEW_TAB: AddTab(); break; case IDM_FILE_OPEN: DoOpenSave(FALSE); break; case IDM_FILE_SAVE_AS: DoOpenSave(TRUE); break;
            case IDM_FILE_EXIT: DestroyWindow(hwnd); break;
            case IDM_VIEW_THEME_LIGHT: g_themeMode=THEME_LIGHT; UpdateTheme(hwnd); break;
            case IDM_VIEW_THEME_DARK: g_themeMode=THEME_DARK; UpdateTheme(hwnd); break;
            case IDM_VIEW_THEME_AUTO: g_themeMode=THEME_AUTO; UpdateTheme(hwnd); break;
            case IDM_EDIT_UNDO: SendMessageW(g_hwndEdit,WM_UNDO,0,0); break;
            case IDM_EDIT_CUT: SendMessageW(g_hwndEdit,WM_CUT,0,0); break;
            case IDM_EDIT_COPY: SendMessageW(g_hwndEdit,WM_COPY,0,0); break;
            case IDM_EDIT_PASTE: SendMessageW(g_hwndEdit,WM_PASTE,0,0); break;
            case IDM_EDIT_DELETE: SendMessageW(g_hwndEdit,WM_CLEAR,0,0); break;
            case IDM_EDIT_SELECT_ALL: SendMessageW(g_hwndEdit,EM_SETSEL,0,-1); break;
        }
        UpdateStatus();
    }return 0;
    case WM_CONTEXTMENU:
        if((HWND)wParam==g_hwndEdit){
            HMENU c=CreatePopupMenu(); AppendMenuW(c,MF_STRING,IDM_EDIT_UNDO,L"Undo"); AppendMenuW(c,MF_SEPARATOR,0,NULL); AppendMenuW(c,MF_STRING,IDM_EDIT_CUT,L"Cut"); AppendMenuW(c,MF_STRING,IDM_EDIT_COPY,L"Copy"); AppendMenuW(c,MF_STRING,IDM_EDIT_PASTE,L"Paste"); AppendMenuW(c,MF_SEPARATOR,0,NULL); AppendMenuW(c,MF_STRING,IDM_EDIT_SELECT_ALL,L"Select All");
            TrackPopupMenu(c,TPM_RIGHTBUTTON,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam),0,hwnd,NULL); DestroyMenu(c);
        } return 0;
    case WM_MOUSEMOVE: UpdateStatus(); return 0;
    case WM_LBUTTONDOWN:{
        int x=GET_X_LPARAM(lParam), y=GET_Y_LPARAM(lParam);
        if(y<TAB_BAR_HEIGHT){
            for(int i=0;i<g_tabCount;i++){ RECT tr={8+i*TAB_WIDTH,4,8+(i+1)*TAB_WIDTH-4,TAB_BAR_HEIGHT-4}; if(x>=tr.left&&x<=tr.right&&y>=tr.top&&y<=tr.bottom){SwitchTab(i); return 0;}}
            SendMessageW(hwnd,WM_NCLBUTTONDOWN,HTCAPTION,0); return 0;
        }
    } break;
    case WM_NCHITTEST:{
        LRESULT ht = DefWindowProcW(hwnd,msg,wParam,lParam);
        POINT pt={GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)}; ScreenToClient(hwnd,&pt);
        if(pt.y<TAB_BAR_HEIGHT){
            for(int i=0;i<g_tabCount;i++){RECT tr={8+i*TAB_WIDTH,4,8+(i+1)*TAB_WIDTH-4,TAB_BAR_HEIGHT-4}; if(PtInRect(&tr,pt)) return HTCLIENT;}
            return HTCAPTION;
        }
        if(pt.y>=TAB_BAR_HEIGHT && pt.y<TAB_BAR_HEIGHT+MENU_BAR_HEIGHT){
            int count=GetMenuItemCount(g_menuMain); RECT mr;
            for(int i=0;i<count;i++){ if(GetMenuItemRect(hwnd,g_menuMain,i,&mr)){ ScreenToClient(hwnd,(POINT*)&mr.left); ScreenToClient(hwnd,(POINT*)&mr.right); if(pt.x>=mr.left&&pt.x<=mr.right&&pt.y>=mr.top&&pt.y<=mr.bottom) return ht; }}
            return HTCAPTION;
        }
        return ht;
    }
    case WM_PAINT:{ PAINTSTRUCT ps; HDC hdc=BeginPaint(hwnd,&ps); PaintCustomBars(hwnd,hdc); EndPaint(hwnd,&ps);} return 0;
    case WM_CTLCOLOREDIT: case WM_CTLCOLORSTATIC:{
        HDC hdc=(HDC)wParam; SetTextColor(hdc,g_themeDark?RGB(230,230,230):RGB(20,20,20)); SetBkColor(hdc,g_themeDark?RGB(30,30,30):RGB(250,250,250)); static HBRUSH b=NULL; if(b)DeleteObject(b); b=CreateSolidBrush(g_themeDark?RGB(30,30,30):RGB(250,250,250)); return (INT_PTR)b; }
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd,msg,wParam,lParam);
}

int WINAPI wWinMain(HINSTANCE hInst,HINSTANCE p,LPWSTR c,int n){
    (void)p;(void)c;
    g_hInst=hInst;
    EnableDpiAwareness();
    INITCOMMONCONTROLSEX ic={sizeof(ic),ICC_STANDARD_CLASSES}; InitCommonControlsEx(&ic);
    WNDCLASSW wc={0}; wc.lpfnWndProc=WndProc; wc.hInstance=hInst; wc.hCursor=LoadCursor(NULL,IDC_ARROW); wc.lpszClassName=L"PlainEditorWin32";
    RegisterClassW(&wc);
    DWORD style=WS_POPUP|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
    HWND hwnd=CreateWindowExW(0,wc.lpszClassName,L"",style,CW_USEDEFAULT,CW_USEDEFAULT,1100,760,NULL,NULL,hInst,NULL);
    ShowWindow(hwnd,n);
    MSG msg; while(GetMessageW(&msg,NULL,0,0)){ TranslateMessage(&msg); DispatchMessageW(&msg);} return (int)msg.wParam;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE p, LPSTR c, int n){
    (void)c;
    return wWinMain(hInst, p, GetCommandLineW(), n);
}
