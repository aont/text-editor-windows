#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "comctl32.lib")

#ifndef EM_SETBKGNDCOLOR
#define EM_SETBKGNDCOLOR 0x0443
#endif
#ifndef EM_REDO
#define EM_REDO 0x0454
#endif

#define IDC_TAB 1001
#define IDC_EDIT 1002
#define IDC_STATUS 1003
#define IDC_MENU_FILE 1004
#define IDC_MENU_EDIT 1005
#define IDC_MENU_VIEW 1006
#define IDC_ENC_COMBO 2001
#define IDC_EOL_COMBO 2002

#define ID_FILE_NEW_TAB 40001
#define ID_FILE_NEW_WINDOW 40002
#define ID_FILE_OPEN 40003
#define ID_FILE_SAVE 40004
#define ID_FILE_SAVE_AS 40005
#define ID_FILE_CHANGE_ENCODING 40006
#define ID_FILE_RELOAD_ENCODING 40007
#define ID_FILE_CLOSE_TAB 40008
#define ID_FILE_CLOSE_WINDOW 40009
#define ID_FILE_EXIT 40010
#define ID_EDIT_UNDO 40101
#define ID_EDIT_REDO 40102
#define ID_EDIT_CUT 40103
#define ID_EDIT_COPY 40104
#define ID_EDIT_PASTE 40105
#define ID_EDIT_DELETE 40106
#define ID_EDIT_FIND 40107
#define ID_EDIT_FIND_NEXT 40108
#define ID_EDIT_FIND_PREV 40109
#define ID_EDIT_REPLACE 40110
#define ID_EDIT_GOTO 40111
#define ID_EDIT_SELECT_ALL 40112
#define ID_EDIT_FONT 40113
#define ID_VIEW_ZOOM_IN 40201
#define ID_VIEW_ZOOM_OUT 40202
#define ID_VIEW_ZOOM_RESET 40203
#define ID_VIEW_STATUSBAR 40204
#define ID_VIEW_WORDWRAP 40205
#define ID_THEME_LIGHT 40206
#define ID_THEME_DARK 40207
#define ID_THEME_AUTO 40208

typedef enum { THEME_LIGHT, THEME_DARK, THEME_AUTO } ThemeMode;
typedef enum { ENC_UTF8, ENC_UTF16LE, ENC_ANSI } Encoding;
typedef enum { EOL_CRLF, EOL_LF, EOL_CR } LineEnding;

typedef struct {
    char* text;
    char path[MAX_PATH];
    Encoding enc;
    LineEnding eol;
} TabDoc;

static HINSTANCE g_hInst;
static HWND g_hwndTab, g_hwndEdit, g_hwndStatus;
static HWND g_hwndMenuFile, g_hwndMenuEdit, g_hwndMenuView;
static HMENU g_popupFile, g_popupEdit, g_popupView;
static ThemeMode g_theme = THEME_AUTO;
static BOOL g_dark = FALSE;
static HFONT g_font;
static int g_zoom = 100;
static TabDoc g_tabs[64];
static int g_tabCount = 0, g_activeTab = -1;
static int g_dpi = 96;

static COLORREF bg() { return g_dark ? RGB(30,30,30) : RGB(255,255,255); }
static COLORREF fg() { return g_dark ? RGB(230,230,230) : RGB(20,20,20); }
static COLORREF panel() { return g_dark ? RGB(40,40,40) : RGB(245,245,245); }

static BOOL IsSystemDark() {
    DWORD v=1,s=sizeof(v); HKEY k;
    if (RegOpenKeyExA(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",0,KEY_READ,&k)==ERROR_SUCCESS){
        RegQueryValueExA(k,"AppsUseLightTheme",NULL,NULL,(LPBYTE)&v,&s); RegCloseKey(k);
    }
    return v==0;
}

static void ApplyTheme(HWND hwnd){
    g_dark = (g_theme==THEME_DARK) || (g_theme==THEME_AUTO && IsSystemDark());
    SendMessage(g_hwndEdit, EM_SETBKGNDCOLOR, 0, bg());
    InvalidateRect(hwnd,NULL,TRUE);
    InvalidateRect(g_hwndTab,NULL,TRUE);
    InvalidateRect(g_hwndStatus,NULL,TRUE);
}

static int TabBarHeight(){ return 32; }
static int MenuBarHeight(){ return 32; }

static void ShowTopMenu(HWND hwnd, HWND btn, HMENU menu){
    RECT rc;
    GetWindowRect(btn, &rc);
    TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, rc.left, rc.bottom, 0, hwnd, NULL);
}

static void UpdateStatus(){
    int len=GetWindowTextLengthA(g_hwndEdit);
    DWORD sel=SendMessage(g_hwndEdit,EM_GETSEL,0,0);
    int caret=LOWORD(sel); int line=SendMessage(g_hwndEdit,EM_LINEFROMCHAR,caret,0)+1;
    int col=caret-SendMessage(g_hwndEdit,EM_LINEINDEX,line-1,0)+1;
    const char* eol = g_tabs[g_activeTab].eol==EOL_LF?"Unix (LF)":(g_tabs[g_activeTab].eol==EOL_CR?"Classic Mac (CR)":"Windows (CRLF)");
    const char* enc = g_tabs[g_activeTab].enc==ENC_UTF16LE?"UTF-16 LE":(g_tabs[g_activeTab].enc==ENC_ANSI?"ANSI":"UTF-8");
    char b[256]; snprintf(b,sizeof(b),"Line %d, Column %d | %d characters | %d%% | %s | %s",line,col,len,g_zoom,eol,enc);
    SetWindowTextA(g_hwndStatus,b);
}

static void SetTabText(int i){ TCITEMA it={0}; it.mask=TCIF_TEXT; char t[64]; snprintf(t,sizeof(t),"%s", g_tabs[i].path[0]?strrchr(g_tabs[i].path,'\\')?strrchr(g_tabs[i].path,'\\')+1:g_tabs[i].path:"Untitled"); it.pszText=t; TabCtrl_SetItem(g_hwndTab,i,&it); }

static void NewTab(){
    if(g_tabCount>=64)return;
    g_tabs[g_tabCount]=(TabDoc){_strdup(""),"",ENC_UTF8,EOL_LF};
    TCITEMA it={0}; it.mask=TCIF_TEXT; it.pszText="Untitled"; TabCtrl_InsertItem(g_hwndTab,g_tabCount,&it);
    g_activeTab=g_tabCount++; TabCtrl_SetCurSel(g_hwndTab,g_activeTab);
    SetWindowTextA(g_hwndEdit,""); UpdateStatus();
}

static void SaveCurrentText(){
    int len=GetWindowTextLengthA(g_hwndEdit);
    free(g_tabs[g_activeTab].text); g_tabs[g_activeTab].text=(char*)malloc(len+1);
    GetWindowTextA(g_hwndEdit,g_tabs[g_activeTab].text,len+1);
}

static void SwitchTab(int i){ if(i<0||i>=g_tabCount)return; SaveCurrentText(); g_activeTab=i; SetWindowTextA(g_hwndEdit,g_tabs[i].text?g_tabs[i].text:""); UpdateStatus(); }

static UINT_PTR CALLBACK FileHook(HWND hdlg, UINT msg, WPARAM w, LPARAM l){
    if(msg==WM_INITDIALOG){
        CreateWindowA("STATIC","Encoding:",WS_CHILD|WS_VISIBLE,8,8,60,20,hdlg,NULL,g_hInst,NULL);
        HWND c1=CreateWindowA("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,72,6,120,160,hdlg,(HMENU)IDC_ENC_COMBO,g_hInst,NULL);
        SendMessageA(c1,CB_ADDSTRING,0,(LPARAM)"UTF-8"); SendMessageA(c1,CB_ADDSTRING,0,(LPARAM)"UTF-16 LE"); SendMessageA(c1,CB_ADDSTRING,0,(LPARAM)"ANSI"); SendMessage(c1,CB_SETCURSEL,0,0);
        CreateWindowA("STATIC","Line endings:",WS_CHILD|WS_VISIBLE,200,8,74,20,hdlg,NULL,g_hInst,NULL);
        HWND c2=CreateWindowA("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,278,6,130,160,hdlg,(HMENU)IDC_EOL_COMBO,g_hInst,NULL);
        SendMessageA(c2,CB_ADDSTRING,0,(LPARAM)"CRLF"); SendMessageA(c2,CB_ADDSTRING,0,(LPARAM)"LF"); SendMessageA(c2,CB_ADDSTRING,0,(LPARAM)"CR"); SendMessage(c2,CB_SETCURSEL,1,0);
    }
    return 0;
}

static void DoOpenSave(BOOL save){
    char f[MAX_PATH]=""; OPENFILENAMEA ofn={0}; ofn.lStructSize=sizeof(ofn); ofn.hwndOwner=GetParent(g_hwndEdit); ofn.lpstrFilter="Text Files\0*.txt\0All Files\0*.*\0"; ofn.lpstrFile=f; ofn.nMaxFile=MAX_PATH; ofn.Flags=OFN_EXPLORER|OFN_ENABLEHOOK|OFN_HIDEREADONLY|(save?OFN_OVERWRITEPROMPT:OFN_FILEMUSTEXIST); ofn.lpfnHook=FileHook;
    if((save?GetSaveFileNameA:GetOpenFileNameA)(&ofn)){
        HWND p=GetParent(g_hwndEdit); HWND c1=GetDlgItem(ofn.hwndOwner,IDC_ENC_COMBO),c2=GetDlgItem(ofn.hwndOwner,IDC_EOL_COMBO);
        int e=(int)SendMessage(c1,CB_GETCURSEL,0,0),l=(int)SendMessage(c2,CB_GETCURSEL,0,0);
        g_tabs[g_activeTab].enc=(Encoding)e; g_tabs[g_activeTab].eol=(LineEnding)l; strncpy(g_tabs[g_activeTab].path,f,MAX_PATH-1);
        if(save){
            SaveCurrentText(); FILE* fp=fopen(f,"wb"); if(!fp)return; const char* s=g_tabs[g_activeTab].text?g_tabs[g_activeTab].text:"";
            for(const char* p=s; *p; ++p){ if(*p=='\n'){ if(l==EOL_CRLF)fputs("\r\n",fp); else if(l==EOL_CR)fputc('\r',fp); else fputc('\n',fp);} else if(*p!='\r')fputc(*p,fp);} fclose(fp);
        } else {
            FILE* fp=fopen(f,"rb"); if(!fp)return; fseek(fp,0,SEEK_END); long sz=ftell(fp); fseek(fp,0,SEEK_SET); char* b=(char*)malloc(sz+1); fread(b,1,sz,fp); b[sz]=0; fclose(fp);
            free(g_tabs[g_activeTab].text); g_tabs[g_activeTab].text=b; SetWindowTextA(g_hwndEdit,b);
        }
        SetTabText(g_activeTab); UpdateStatus(); InvalidateRect(p,NULL,TRUE);
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l){
    switch(msg){
    case WM_CREATE:{
        g_dpi=GetDpiForWindow(hwnd);
        g_hwndTab=CreateWindowExA(0,WC_TABCONTROLA,"",WS_CHILD|WS_VISIBLE|TCS_FIXEDWIDTH,0,0,100,TabBarHeight(),hwnd,(HMENU)IDC_TAB,g_hInst,NULL);
        g_hwndMenuFile=CreateWindowExA(0,"BUTTON","File",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,8,TabBarHeight()+4,70,24,hwnd,(HMENU)IDC_MENU_FILE,g_hInst,NULL);
        g_hwndMenuEdit=CreateWindowExA(0,"BUTTON","Edit",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,86,TabBarHeight()+4,70,24,hwnd,(HMENU)IDC_MENU_EDIT,g_hInst,NULL);
        g_hwndMenuView=CreateWindowExA(0,"BUTTON","View",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,164,TabBarHeight()+4,70,24,hwnd,(HMENU)IDC_MENU_VIEW,g_hInst,NULL);
        g_hwndEdit=CreateWindowExA(0,"EDIT","",WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,0,TabBarHeight()+MenuBarHeight(),100,100,hwnd,(HMENU)IDC_EDIT,g_hInst,NULL);
        g_hwndStatus=CreateWindowExA(0,"STATIC","",WS_CHILD|WS_VISIBLE|SS_LEFT,0,0,100,24,hwnd,(HMENU)IDC_STATUS,g_hInst,NULL);
        SendMessage(g_hwndEdit,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),TRUE);
        NewTab(); ApplyTheme(hwnd);
    } break;
    case WM_SIZE:{ int wdt=LOWORD(l), h=HIWORD(l); int top=TabBarHeight()+MenuBarHeight(); MoveWindow(g_hwndTab,0,0,wdt,TabBarHeight(),TRUE); MoveWindow(g_hwndStatus,0,h-24,wdt,24,TRUE); MoveWindow(g_hwndEdit,0,top,wdt,h-top-24,TRUE);} break;
    case WM_NOTIFY: if(((LPNMHDR)l)->idFrom==IDC_TAB && ((LPNMHDR)l)->code==TCN_SELCHANGE) SwitchTab(TabCtrl_GetCurSel(g_hwndTab)); break;
    case WM_COMMAND:{
        switch(LOWORD(w)){
        case IDC_MENU_FILE: ShowTopMenu(hwnd, g_hwndMenuFile, g_popupFile); break;
        case IDC_MENU_EDIT: ShowTopMenu(hwnd, g_hwndMenuEdit, g_popupEdit); break;
        case IDC_MENU_VIEW: ShowTopMenu(hwnd, g_hwndMenuView, g_popupView); break;
        case ID_FILE_NEW_TAB: NewTab(); break; case ID_FILE_OPEN: DoOpenSave(FALSE); break; case ID_FILE_SAVE: DoOpenSave(TRUE); break; case ID_FILE_SAVE_AS: DoOpenSave(TRUE); break;
        case ID_FILE_CLOSE_TAB: if(g_tabCount>1){ free(g_tabs[g_activeTab].text); TabCtrl_DeleteItem(g_hwndTab,g_activeTab); for(int i=g_activeTab;i<g_tabCount-1;i++)g_tabs[i]=g_tabs[i+1]; g_tabCount--; SwitchTab(max(0,g_activeTab-1)); } break;
        case ID_FILE_NEW_WINDOW: ShellExecuteA(NULL,"open",GetCommandLineA(),NULL,NULL,SW_SHOWNORMAL); break; case ID_FILE_CLOSE_WINDOW: DestroyWindow(hwnd); break; case ID_FILE_EXIT: PostQuitMessage(0); break;
        case ID_EDIT_UNDO: SendMessage(g_hwndEdit,WM_UNDO,0,0); break; case ID_EDIT_REDO: SendMessage(g_hwndEdit,EM_REDO,0,0); break; case ID_EDIT_CUT: SendMessage(g_hwndEdit,WM_CUT,0,0); break;
        case ID_EDIT_COPY: SendMessage(g_hwndEdit,WM_COPY,0,0); break; case ID_EDIT_PASTE: SendMessage(g_hwndEdit,WM_PASTE,0,0); break; case ID_EDIT_DELETE: SendMessage(g_hwndEdit,WM_CLEAR,0,0); break;
        case ID_EDIT_SELECT_ALL: SendMessage(g_hwndEdit,EM_SETSEL,0,-1); break;
        case ID_VIEW_WORDWRAP:{ LONG s=GetWindowLong(g_hwndEdit,GWL_STYLE); s ^= WS_HSCROLL|ES_AUTOHSCROLL; SetWindowLong(g_hwndEdit,GWL_STYLE,s); InvalidateRect(g_hwndEdit,NULL,TRUE);} break;
        case ID_VIEW_STATUSBAR: ShowWindow(g_hwndStatus, IsWindowVisible(g_hwndStatus)?SW_HIDE:SW_SHOW); { RECT rc; GetClientRect(hwnd,&rc); SendMessage(hwnd,WM_SIZE,0,MAKELPARAM(rc.right,rc.bottom)); } break;
        case ID_VIEW_ZOOM_IN: g_zoom+=10; break; case ID_VIEW_ZOOM_OUT: g_zoom=max(10,g_zoom-10); break; case ID_VIEW_ZOOM_RESET: g_zoom=100; break;
        case ID_THEME_LIGHT: g_theme=THEME_LIGHT; ApplyTheme(hwnd); break; case ID_THEME_DARK: g_theme=THEME_DARK; ApplyTheme(hwnd); break; case ID_THEME_AUTO: g_theme=THEME_AUTO; ApplyTheme(hwnd); break;
        }
        UpdateStatus();
    } break;
    case WM_CTLCOLOREDIT: case WM_CTLCOLORSTATIC:{ HDC dc=(HDC)w; SetTextColor(dc,fg()); SetBkColor(dc,(HWND)l==g_hwndStatus?panel():bg()); static HBRUSH b1,b2; if(b1)DeleteObject(b1); if(b2)DeleteObject(b2); b1=CreateSolidBrush(bg()); b2=CreateSolidBrush(panel()); return (LRESULT)((HWND)l==g_hwndStatus?b2:b1);} break;
    case WM_CONTEXTMENU: if((HWND)w==g_hwndEdit){ HMENU m=CreatePopupMenu(); AppendMenuA(m,MF_STRING,ID_EDIT_UNDO,"Undo"); AppendMenuA(m,MF_STRING,ID_EDIT_CUT,"Cut"); AppendMenuA(m,MF_STRING,ID_EDIT_COPY,"Copy"); AppendMenuA(m,MF_STRING,ID_EDIT_PASTE,"Paste"); AppendMenuA(m,MF_STRING,ID_EDIT_SELECT_ALL,"Select All"); TrackPopupMenu(m,TPM_RIGHTBUTTON,GET_X_LPARAM(l),GET_Y_LPARAM(l),0,hwnd,NULL); DestroyMenu(m);} break;
    case WM_NCHITTEST:{
        LRESULT ht=DefWindowProc(hwnd,msg,w,l);
        if(ht==HTCLIENT){
            POINT p={GET_X_LPARAM(l),GET_Y_LPARAM(l)};
            ScreenToClient(hwnd,&p);
            if(p.y < TabBarHeight() + MenuBarHeight()){
                HWND c=ChildWindowFromPoint(hwnd,p);
                if(c==g_hwndTab || c==hwnd) return HTCAPTION;
                if(c!=g_hwndMenuFile && c!=g_hwndMenuEdit && c!=g_hwndMenuView) return HTCAPTION;
            }
        }
        return ht;
    }
    case WM_DESTROY: PostQuitMessage(0); break;
    }
    return DefWindowProc(hwnd,msg,w,l);
}

static HMENU BuildMenu(){
    HMENU m=CreateMenu(),f=CreateMenu(),e=CreateMenu(),v=CreateMenu(),z=CreateMenu(),t=CreateMenu(),r=CreateMenu();
    AppendMenuA(f,MF_STRING,ID_FILE_NEW_TAB,"New Tab"); AppendMenuA(f,MF_STRING,ID_FILE_NEW_WINDOW,"New Window"); AppendMenuA(f,MF_STRING,ID_FILE_OPEN,"Open");
    AppendMenuA(f,MF_POPUP,(UINT_PTR)r,"Recently Opened Files"); AppendMenuA(f,MF_STRING,ID_FILE_SAVE,"Save"); AppendMenuA(f,MF_STRING,ID_FILE_SAVE_AS,"Save As");
    AppendMenuA(f,MF_STRING,ID_FILE_CHANGE_ENCODING,"Change Encoding"); AppendMenuA(f,MF_STRING,ID_FILE_RELOAD_ENCODING,"Reload with Specified Encoding");
    AppendMenuA(f,MF_STRING,ID_FILE_CLOSE_TAB,"Close Tab"); AppendMenuA(f,MF_STRING,ID_FILE_CLOSE_WINDOW,"Close Window"); AppendMenuA(f,MF_STRING,ID_FILE_EXIT,"Exit");
    AppendMenuA(e,MF_STRING,ID_EDIT_UNDO,"Undo"); AppendMenuA(e,MF_STRING,ID_EDIT_REDO,"Redo"); AppendMenuA(e,MF_STRING,ID_EDIT_CUT,"Cut"); AppendMenuA(e,MF_STRING,ID_EDIT_COPY,"Copy");
    AppendMenuA(e,MF_STRING,ID_EDIT_PASTE,"Paste"); AppendMenuA(e,MF_STRING,ID_EDIT_DELETE,"Delete"); AppendMenuA(e,MF_STRING,ID_EDIT_FIND,"Find"); AppendMenuA(e,MF_STRING,ID_EDIT_FIND_NEXT,"Find Next");
    AppendMenuA(e,MF_STRING,ID_EDIT_FIND_PREV,"Find Previous"); AppendMenuA(e,MF_STRING,ID_EDIT_REPLACE,"Replace"); AppendMenuA(e,MF_STRING,ID_EDIT_GOTO,"Go To"); AppendMenuA(e,MF_STRING,ID_EDIT_SELECT_ALL,"Select All"); AppendMenuA(e,MF_STRING,ID_EDIT_FONT,"Font");
    AppendMenuA(z,MF_STRING,ID_VIEW_ZOOM_IN,"Zoom In"); AppendMenuA(z,MF_STRING,ID_VIEW_ZOOM_OUT,"Zoom Out"); AppendMenuA(z,MF_STRING,ID_VIEW_ZOOM_RESET,"Restore Default Zoom");
    AppendMenuA(t,MF_STRING,ID_THEME_LIGHT,"Light"); AppendMenuA(t,MF_STRING,ID_THEME_DARK,"Dark"); AppendMenuA(t,MF_STRING,ID_THEME_AUTO,"Auto");
    AppendMenuA(v,MF_POPUP,(UINT_PTR)z,"Zoom"); AppendMenuA(v,MF_STRING,ID_VIEW_STATUSBAR,"Status Bar"); AppendMenuA(v,MF_STRING,ID_VIEW_WORDWRAP,"Word Wrap"); AppendMenuA(v,MF_POPUP,(UINT_PTR)t,"App Theme");
    AppendMenuA(m,MF_POPUP,(UINT_PTR)f,"File"); AppendMenuA(m,MF_POPUP,(UINT_PTR)e,"Edit"); AppendMenuA(m,MF_POPUP,(UINT_PTR)v,"View");
    return m;
}

static void InitMenus(){
    HMENU m=BuildMenu();
    g_popupFile = GetSubMenu(m, 0);
    g_popupEdit = GetSubMenu(m, 1);
    g_popupView = GetSubMenu(m, 2);
}

int WINAPI WinMain(HINSTANCE h,HINSTANCE p,LPSTR cmd,int n){
    (void)p;(void)cmd; INITCOMMONCONTROLSEX ic={sizeof(ic),ICC_TAB_CLASSES}; InitCommonControlsEx(&ic); SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    g_hInst=h; InitMenus(); WNDCLASSA wc={0}; wc.lpfnWndProc=WndProc; wc.hInstance=h; wc.hCursor=LoadCursor(NULL,IDC_ARROW); wc.hbrBackground=CreateSolidBrush(RGB(30,30,30)); wc.lpszClassName="PlainEditorWnd";
    RegisterClassA(&wc);
    HWND hwnd=CreateWindowExA(0,"PlainEditorWnd","Plain Text Editor",WS_POPUP|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SIZEBOX,100,100,1100,700,NULL,NULL,h,NULL);
    ShowWindow(hwnd,n); UpdateWindow(hwnd); MSG msg; while(GetMessage(&msg,NULL,0,0)){ TranslateMessage(&msg); DispatchMessage(&msg);} return 0;
}
