#!/bin/sh
# Fetch and verify the pinned Linux release, then apply patches/series.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
ver=$(cat "$here/VERSION")
major=${ver%%.*}
src=${SRC_DIR:-$here/src}
tarball="linux-$ver.tar.xz"
mkdir -p "$src"
src=$(cd "$src" && pwd)
cd "$src"
if [ ! -f "$tarball" ]; then
	curl -fL --retry 3 -o "$tarball.part" "https://cdn.kernel.org/pub/linux/kernel/v$major.x/$tarball"
	mv "$tarball.part" "$tarball"
fi
sha256sum -c "$here/$tarball.sha256"
stamp=$(cd "$here/patches" && cat series $(grep -Ev '^(#|$)' series) | sha256sum | cut -d' ' -f1)
if [ -d "linux-$ver" ] && [ "$(cat "linux-$ver/.nekoos-patches" 2>/dev/null)" != "$stamp" ]; then
	echo "patch set changed, re-extracting linux-$ver" >&2
	rm -rf "linux-$ver"
fi
if [ ! -d "linux-$ver" ]; then
	tar xf "$tarball"
	cd "linux-$ver"
	while read -r p; do
		case "$p" in ''|'#'*) continue ;; esac
		echo "applying $p" >&2
		patch -p1 < "$here/patches/$p" >&2
	done < "$here/patches/series"
	echo "$stamp" > .nekoos-patches
	cd "$src"
fi
echo "$src/linux-$ver"
