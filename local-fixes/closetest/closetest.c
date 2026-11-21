/* Does InternetCloseHandle() abort a synchronous HttpSendRequest() that is in flight on another thread?
 * usage: closetest.exe HOST PORT [CLOSE_AFTER_MS]   (server must delay its response, see slowserver.py) */
#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include <stdlib.h>

static HINTERNET req;
static DWORD t0;

static DWORD WINAPI worker(void *arg)
{
    BOOL ok = HttpSendRequestA(req, NULL, 0, NULL, 0);
    DWORD err = GetLastError(), t = GetTickCount() - t0;
    printf("[worker] HttpSendRequest returned %d, GetLastError=%lu, at %lu ms\n", ok, ok ? 0 : err, t);
    if (ok)
    {
        DWORD status = 0, len = sizeof(status);
        BOOL q = HttpQueryInfoA(req, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &len, NULL);
        printf("[worker] HttpQueryInfo(status) returned %d, status=%lu, GetLastError=%lu\n", q, status, q ? 0 : GetLastError());
    }
    fflush(stdout);
    return 0;
}

int main(int argc, char **argv)
{
    const char *host = argc > 1 ? argv[1] : "127.0.0.1";
    int port = argc > 2 ? atoi(argv[2]) : 8765;
    DWORD close_after = argc > 3 ? atoi(argv[3]) : 1000, t;
    BOOL secure = argc > 4 && argv[4][0] == 's';   /* https: test the TLS-handshake phase */
    HINTERNET ses, con; HANDLE th; BOOL r;

    ses = InternetOpenA("closetest", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    con = InternetConnectA(ses, host, (INTERNET_PORT)port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    req = HttpOpenRequestA(con, "GET", "/slow", NULL, NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | (secure ? INTERNET_FLAG_SECURE : 0), 0);
    if (!ses || !con || !req) { printf("setup failed %lu\n", GetLastError()); return 2; }
    t0 = GetTickCount();
    th = CreateThread(NULL, 0, worker, NULL, 0, NULL);
    Sleep(close_after);
    t = GetTickCount();
    r = InternetCloseHandle(req);
    printf("[main] InternetCloseHandle at %lu ms returned %d (took %lu ms)\n", t - t0, r, GetTickCount() - t);
    fflush(stdout);
    if (WaitForSingleObject(th, 20000) == WAIT_TIMEOUT) printf("[main] worker still running after 20 s\n");
    printf("[main] worker finished at %lu ms\n", GetTickCount() - t0);
    InternetCloseHandle(con); InternetCloseHandle(ses);
    return 0;
}
