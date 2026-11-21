# TCP server that accepts and never answers (client stalls in the TLS handshake). usage: stallserver.py [PORT]
import socket, sys, time, threading
s = socket.socket(); s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1); s.bind(('0.0.0.0', int(sys.argv[1]) if len(sys.argv) > 1 else 8766)); s.listen(8)
def h(c):
    t = time.time()
    try:
        while c.recv(4096): pass
    except Exception: pass
    print(f'client closed after {time.time()-t:.1f}s', flush=True)
while True:
    c, _ = s.accept(); threading.Thread(target=h, args=(c,), daemon=True).start()
