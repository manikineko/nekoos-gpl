#!/bin/sh
# Cross-build the NekoOS kernel with LLVM (works on Linux and WSL2).
#   ./build.sh x86_64|arm64 [extra fragment ...]
# Output: out/<arch>/{boot image, modules/, dtbs/, headers/}
set -eu
here=$(cd "$(dirname "$0")" && pwd)
arch=${1:?usage: build.sh x86_64|arm64 [fragment ...]}
shift
case "$arch" in
	x86_64|amd64) karch=x86_64; image=arch/x86/boot/bzImage; targets=all; defconfig=x86_64_defconfig ;;
	arm64|aarch64) karch=arm64; image=arch/arm64/boot/Image.gz; targets="all Image.gz"; defconfig=defconfig ;;
	*) echo "unknown arch $arch" >&2; exit 1 ;;
esac
llvm=${LLVM:-1}
jobs=${JOBS:-$(nproc)}
ksrc=$("$here/fetch.sh" | tail -n1)
out="$here/out/$karch"
obj="$out/obj"
mkdir -p "$obj"
mk() { make -C "$ksrc" O="$obj" ARCH="$karch" LLVM="$llvm" -j"$jobs" "$@"; }

mk "$defconfig"
frags="$here/configs/nekoos.config $here/configs/nekoos-$karch.config"
for f in "$@"; do
	case "$f" in /*) ;; *) f=$PWD/$f ;; esac
	frags="$frags $f"
done
(cd "$obj" && ARCH="$karch" "$ksrc/scripts/kconfig/merge_config.sh" -m -O "$obj" .config $frags)
mk olddefconfig
for f in $frags; do
	grep -E '^CONFIG_[A-Z0-9_]+=' "$f" | while IFS= read -r line; do
		grep -qx "$line" "$obj/.config" || echo "warning: $line not set (missing dependency or unavailable on $karch)" >&2
	done
done
mk $targets
rm -rf "$out/modules" "$out/headers" "$out/dtbs"
mk INSTALL_MOD_PATH="$out/modules" INSTALL_MOD_STRIP=1 modules_install
mk INSTALL_HDR_PATH="$out/headers" headers_install
if [ "$karch" = arm64 ]; then mk INSTALL_DTBS_PATH="$out/dtbs" dtbs_install; fi
cp "$obj/$image" "$out/"
rm -f "$out/vmlinuz.efi"
if grep -qx CONFIG_EFI_ZBOOT=y "$obj/.config"; then cp "$obj/arch/arm64/boot/vmlinuz.efi" "$out/"; fi
cp "$obj/.config" "$out/config"
echo "kernel: $out/$(basename "$image")"
