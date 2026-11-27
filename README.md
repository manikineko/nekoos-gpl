# nekoos-gpl

NekoOS GPL components, isolated from the permissive userspace in
[`manikineko/nekoos`](https://github.com/manikineko/nekoos). Everything in
this repo is GPL-2.0 (see `LICENSE`).

## Contents

| Path | What | License |
|---|---|---|
| `linux/` | Linux kernel 6.18.65 source (NekoOS kernel build base) | GPL-2.0 WITH Linux-syscall-note |
| `phoneos-binder/` | Android binder/ashmem kernel sources and the GPL linuxkpi shims used by the phoneOS binder-kmod port (permissive shims/scripts stay in `nekoos/phoneos/build/binder-kmod/`) | GPL-2.0 |
| `patches/kernel_samsung_kanas/` | Patches against `kernel_samsung_kanas` (LineageOS 15.1 / Linux 3.10.108): tree repairs + power/OOM fixes for the Galaxy Core 2 | GPL-2.0 |
