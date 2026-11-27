/* SPDX-License-Identifier: GPL-2.0 */
/* ashmem_lkpi_shim.h - FreeBSD linuxkpi gaps for the Android ashmem port.
 * Force-included into the kmod sources via the Makefile. */

#ifndef _ASHMEM_LKPI_SHIM_H_
#define _ASHMEM_LKPI_SHIM_H_

#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/seq_file.h>
#include <linux/shmem_fs.h>
#include <sys/systm.h>
#include <sys/errno.h>

#ifndef PROT_READ
#define	PROT_READ	VM_PROT_READ
#define	PROT_WRITE	VM_PROT_WRITE
#define	PROT_EXEC	VM_PROT_EXECUTE
#endif

/* linuxkpi shrinker passes shrink_control with an uninitialized gfp_mask;
 * treat the __GFP_FS probe in ashmem_shrink_scan as always-true. */
#ifndef __GFP_FS
#define	__GFP_FS	~0UL
#endif

typedef unsigned long vm_flags_t;

/* linuxkpi mm.h defines VM_MAYWRITE (1 << 13) only */
#ifndef VM_MAYREAD
#define	VM_MAYREAD	(1 << 12)
#define	VM_MAYEXEC	(1 << 14)
#endif

#ifndef FMODE_LSEEK
#define	FMODE_LSEEK	0x4000
#endif

#ifndef _calc_vm_trans
static inline unsigned long
_calc_vm_trans(unsigned long prot, unsigned long bit1, unsigned long bit2)
{
	if ((bit1 & bit2) != 0)
		return (0);
	if ((prot & bit1) == 0)
		return (0);
	return (bit2);
}
#endif

#ifndef calc_vm_prot_bits
#define	calc_vm_prot_bits(prot, legacy)					\
	((((prot) & PROT_READ) ? VM_READ : 0) |				\
	 (((prot) & PROT_WRITE) ? VM_WRITE : 0) |			\
	 (((prot) & PROT_EXEC) ? VM_EXEC : 0))
#endif

#ifndef vma_set_anonymous
static inline void
vma_set_anonymous(struct vm_area_struct *vma)
{
	/* No anonymous-vma concept on FreeBSD linuxkpi; the pager object
	 * set up by shmem_zero_setup() is always the backing store. */
}
#endif

#ifndef strncpy_from_user
static inline long
strncpy_from_user(char *dst, const char __user *src, long count)
{
	size_t copied;
	int error;

	error = copyinstr(src, dst, count, &copied);
	if (error == ENAMETOOLONG)
		return (-E2BIG);
	if (error != 0)
		return (-EFAULT);
	return ((long)copied);
}
#endif

#ifndef vfs_llseek
static inline loff_t
vfs_llseek(struct linux_file *file, loff_t offset, int whence)
{
	if (file->f_op != NULL && file->f_op->llseek != NULL)
		return (file->f_op->llseek(file, offset, whence));
	return (-ESPIPE);
}
#endif

#ifndef generic_file_open
static inline int
generic_file_open(struct inode *inode, struct linux_file *filp)
{
	return (0);
}
#endif

#ifndef device_initcall
#define	device_initcall(fn)	module_init(fn)
#endif

int is_ashmem_file(struct linux_file *file);

int shmem_zero_setup(struct vm_area_struct *vma);
void ashmem_punch_hole(struct linux_file *f, loff_t start, loff_t len);

#endif /* _ASHMEM_LKPI_SHIM_H_ */
