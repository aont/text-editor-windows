# Win32 Tabbed Plain Text Editor - C版 修正版

Win32 API / RichEdit / MinGW-w64 向けの C 言語版タブ付きテキストエディターです。

## 今回の修正

- UI言語を日本語 / 英語で切り替えられるようにしました。
  - `表示(&V)` → `言語(&L)` から `日本語` / `英語` を選択できます。
  - 選択した言語は `tabedit.exe.ini` の `[App] Language=ja|en` に保存されます。
- 各言語の表示テキストをソースコードから分離しました。
  - 日本語: `lang_ja.ini`
  - 英語: `lang_en.ini`
  - 実行時は `tabedit.exe` と同じフォルダーにある `lang_<language>.ini` を読み込みます。
- ファイル選択ダイアログの追加オプション、メニュー、メッセージ、ステータスバー、右クリックメニューを言語ファイルから取得するようにしました。

## 言語ファイル

言語ファイルは UTF-16 LE BOM 付き INI 形式です。

```ini
[Strings]
menu_file=ファイル(&F)
menu_view_language=言語(&L)
```

新しい文言を追加・変更する場合は、`lang_ja.ini` と `lang_en.ini` の同じキーを編集してください。

## ビルド

MSYS2 MinGW64 / UCRT64 で:

```bash
mingw32-make
```

直接ビルド:

```bash
windres --codepage=65001 -O coff -i resource.rc -o resource.o
gcc -std=c11 -O2 -Wall -Wextra -municode -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0601 -finput-charset=UTF-8 -o tabedit.exe main.c resource.o -mwindows -municode -lcomctl32 -lcomdlg32 -lshell32 -luser32 -lgdi32 -luxtheme -ladvapi32 -lkernel32
```

## 配布時の注意

`tabedit.exe` と同じフォルダーに以下を置いてください。

- `lang_ja.ini`
- `lang_en.ini`
