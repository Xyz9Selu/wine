# HTTP server that answers /slow after DELAY seconds (default 5). usage: slowserver.py [PORT] [DELAY]
import sys, time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
port = int(sys.argv[1]) if len(sys.argv) > 1 else 8765; delay = float(sys.argv[2]) if len(sys.argv) > 2 else 5
class H(BaseHTTPRequestHandler):
    def do_GET(self):
        t = time.time(); time.sleep(delay)
        try:
            self.send_response(200); self.send_header('Content-Length', '2'); self.end_headers(); self.wfile.write(b'ok')
            print(f'answered after {time.time()-t:.1f}s', flush=True)
        except Exception as e: print(f'client gone after {time.time()-t:.1f}s: {type(e).__name__}', flush=True)
    def log_message(self, *a): pass
ThreadingHTTPServer(('0.0.0.0', port), H).serve_forever()
