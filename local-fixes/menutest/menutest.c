/* GetMenuItemRect() on an owner-drawn menu bar whose layout is invalid.
 * Windows: prints the rect. Wine 11.21/11.22: user_check_not_lock assertion (WM_MEASUREITEM sent under the user lock). */
#include <windows.h>
#include <stdio.h>

static int measure_calls;

static LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_MEASUREITEM)
    {
        MEASUREITEMSTRUCT *mis = (MEASUREITEMSTRUCT *)lp;
        mis->itemWidth = 60; mis->itemHeight = 20; measure_calls++;
        return TRUE;
    }
    if (msg == WM_DRAWITEM) return TRUE;
    return DefWindowProcA(hwnd, msg, wp, lp);
}

int main(void)
{
    WNDCLASSA wc = {0}; HWND hwnd; HMENU menu; RECT rc; BOOL ret; int before;

    wc.lpfnWndProc = wndproc; wc.hInstance = GetModuleHandleA(NULL); wc.lpszClassName = "menutest";
    RegisterClassA(&wc);
    menu = CreateMenu();
    AppendMenuA(menu, MF_OWNERDRAW, 1, (const char *)1);
    AppendMenuA(menu, MF_OWNERDRAW, 2, (const char *)2);
    hwnd = CreateWindowA("menutest", "menutest", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 400, 200, NULL, menu, wc.hInstance, NULL);
    UpdateWindow(hwnd);
    printf("window created, WM_MEASUREITEM so far: %d\n", measure_calls); fflush(stdout);

    AppendMenuA(menu, MF_OWNERDRAW, 3, (const char *)3);   /* changes the menu: its bar layout is now invalid */
    before = measure_calls;
    printf("calling GetMenuItemRect on the modified, not yet redrawn menu bar...\n"); fflush(stdout);
    ret = GetMenuItemRect(hwnd, menu, 2, &rc);
    printf("GetMenuItemRect returned %d, rect (%ld,%ld)-(%ld,%ld), WM_MEASUREITEM during the call: %d\n",
           ret, rc.left, rc.top, rc.right, rc.bottom, measure_calls - before);
    DestroyWindow(hwnd);
    return 0;
}
