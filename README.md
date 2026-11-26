# nekoos-gpl

All GPL-licensed parts of NekoOS live here, kept apart from the permissive userspace in
[`manikineko/nekoos`](https://github.com/manikineko/nekoos), which may be closed-source.

| Path | What | License |
|---|---|---|
| `kernel/` | Linux 7.3.6 (pinned tarball + SHA-256), NekoOS config fragments, patches | GPL-2.0-only |

## Isolation rules

- Nothing in this repo is linked into `nekoos` userspace. Userspace talks to the kernel only through
  syscalls, `/dev`, `/sys` and `/proc`.
- Userspace builds use the kernel's **UAPI headers** (`make headers_install`). They are
  `GPL-2.0 WITH Linux-syscall-note`, which explicitly allows non-GPL programs to use them.
- Any other GPL code that NekoOS ever needs (firmware loaders, drivers, tools) goes here too, never in `nekoos`.
- Binary releases ship this repo's source alongside the kernel image, as GPL-2.0 requires.

## Kernel

The kernel itself is maintained by Qoder. Their changes go in `kernel/patches/` (listed in `kernel/patches/series`).

```sh
# on Linux or WSL2 (Ubuntu): apt install clang-19 lld-19 llvm-19 make flex bison bc libelf-dev libssl-dev cpio
cd kernel
LLVM=-19 ./build.sh x86_64      # Intel/AMD desktops, laptops and tablets
LLVM=-19 ./build.sh arm64       # aarch64 phones, tablets and desktops (+ dtbs)
```

`build.sh` fetches and verifies `linux-$(cat VERSION).tar.xz`, merges
`configs/nekoos.config` + `configs/nekoos-<arch>.config` (+ any extra fragments given as arguments)
over the arch defconfig, builds it, and writes to `kernel/out/<arch>/`:

- `bzImage` / `Image.gz`: an EFI-stub kernel that boots directly from UEFI, or gets wrapped into an
  Android `boot.img` by the userspace tooling
- `modules/`, `dtbs/` (arm64), `headers/` (UAPI, used as the userspace sysroot)

Build on the WSL2 ext4 filesystem (`~/...`), not under `/mnt/c`, because case sensitivity and
symlinks break there.

`configs/nekoos.config` enables what the userspace needs: binder + binderfs, memfd, PSI, seccomp,
namespaces and cgroup v2, FUSE, overlayfs and erofs (for the Android container), DRM/simpledrm/evdev,
and the EFI stub with initramfs.
