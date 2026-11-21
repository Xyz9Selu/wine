# Local fixes on top of wine-11.22

Branch `fixes-11.22` = upstream tag `wine-11.22` plus two fixes. Neither is upstream as of 2026-11-21.

| Commit | Problem | First bad release |
|---|---|---|
| `win32u: Don't hold the menu lock while recomputing the menu bar layout.` | App with an owner-drawn menu bar dies with `user_check_not_lock: Assertion '0' failed` | wine-11.21 (commit 5b0db6dc) |
| `wininet: Abort a synchronous HttpSendRequest() when its handle is closed.` | App that closes a request handle on its own timeout crashes later, because Wine lets the request finish instead of cancelling it | long-standing |

Each commit message has the analysis and the limits of the fix.

## Reproduce the wininet difference

`closetest/closetest.c` sends a request on a worker thread and closes the handle from the main thread after 1 s.

    i686-w64-mingw32-gcc -O1 -o closetest.exe closetest.c -lwininet
    python3 slowserver.py 8765 5 &      # answers after 5 s
    python3 stallserver.py 8766 &       # accepts TCP, never answers (TLS handshake stalls)
    closetest.exe 127.0.0.1 8765 1000   # Windows / fixed Wine: fails at ~1000 ms with 12017
    closetest.exe 127.0.0.1 8766 1000 s # same, in the TLS handshake phase
    closetest.exe 127.0.0.1 8765 7000   # close after completion: request must still succeed

## Reproduce the menu assertion

`menutest/menutest.c`: owner-drawn menu bar, one item appended, then `GetMenuItemRect()` before the bar is redrawn.

    i686-w64-mingw32-gcc -o menutest.exe menutest.c -luser32
    wine menutest.exe    # 11.21/11.22: user_check_not_lock assertion; Windows and this branch: prints the rect

## Build

`build/Dockerfile` (Ubuntu 24.04 + mingw) and `build/build-in-docker.sh`:

    docker build -t wine-build build/
    docker run --rm -u $(id -u):$(id -g) -v $WORK:/w wine-build /w/src/wine/local-fixes/build/build-in-docker.sh

A full build takes about an hour on 4 cores. After a change to one DLL, `make dlls/<name>/all` in `/w/obj`
and copy the two `.dll` files into `out/wine-fixes/lib/wine/{i386,x86_64}-windows/`.

## Status

- Menu fix: the application starts and stays up (2+ minutes) on the patched build.
- wininet fix: matches Windows in `closetest`; the real application logged in 9 times out of 9 (4 through the timeout path), against 5 crashes in 12 logins without it.
