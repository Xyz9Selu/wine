#!/bin/bash
# Runs inside the container (see Dockerfile). Mount a work dir at /w that contains:
#   /w/src/wine          this branch, checked out
#   /w/src/wine-staging  wine-staging at the matching tag (optional; skipped if missing)
# Result: /w/out/wine-fixes  (wow64 build: 32-bit Windows apps on 64-bit host libraries only)
set -eux
cd /w
rm -rf tree && cp -a src/wine tree
if [ -d src/wine-staging ]; then
  python3 src/wine-staging/staging/patchinstall.py --destdir=/w/tree --all --no-autoconf
  (cd tree && ./tools/make_requests && autoreconf -f)
fi
rm -rf obj && mkdir obj && cd obj
/w/tree/configure --prefix=/w/out/wine-fixes --enable-archs=i386,x86_64 --disable-tests
make -j"$(nproc)"
rm -rf /w/out && make install
