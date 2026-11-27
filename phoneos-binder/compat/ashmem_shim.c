/* SPDX-License-Identifier: GPL-2.0 */
/* ashmem_shim.c - FreeBSD linuxkpi shmem backend for the Android ashmem port.
 *
 * The linuxkpi cdev pager object (keyed by the ashmem backing file pointer)
 * is used directly as the region's backing store: pages are zero-filled on
 * first fault, inserted into that object and shared by every mapping of the
 * same ashmem region (cdev_pager returns the same vm_object for the same
 * handle). Punch-hole (purge) removes pages from it; later faults re-allocate
 * zeroed pages, matching ashmem's ASHMEM_WAS_PURGED semantics. */

#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/shmem_fs.h>

#include <vm/vm.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/pmap.h>

static int
ashmem_shmem_fault(struct vm_fault *vmf)
{
	struct vm_area_struct *vma = vmf->vma;
	struct linux_file *f = vma->vm_file;
	vm_object_t obj = vma->vm_obj;
	vm_pindex_t pidx;
	vm_page_t m;
	vm_object_t old;

	if (obj == NULL || f == NULL)
		return (VM_FAULT_SIGBUS);
	pidx = OFF_TO_IDX((vm_offset_t)(uintptr_t)vmf->virtual_address);

	VM_OBJECT_WLOCK(obj);
	m = vm_page_grab(obj, pidx, VM_ALLOC_NORMAL | VM_ALLOC_ZERO);
	if (m == NULL) {
		VM_OBJECT_WUNLOCK(obj);
		return (VM_FAULT_OOM);
	}
	if ((m->flags & PG_ZERO) == 0)
		pmap_zero_page(m);
	vm_page_valid(m);

	/* Remember the storage object so punch-hole (purge) can reach it.
	 * All faults of one region serialize on the object lock here. */
	old = NULL;
	if (f->f_shmem != obj) {
		vm_object_reference_locked(obj);
		old = f->f_shmem;
		f->f_shmem = obj;
	}
	VM_OBJECT_WUNLOCK(obj);

	if (old != NULL)
		vm_object_deallocate(old);

	vma->vm_pfn_first = pidx;
	vma->vm_pfn_count = 1;
	return (VM_FAULT_NOPAGE);
}

static void
ashmem_vm_open(struct vm_area_struct *vma)
{
}

static void
ashmem_vm_close(struct vm_area_struct *vma)
{
}

static const struct vm_operations_struct ashmem_shmem_vmops = {
	.open = ashmem_vm_open,
	.close = ashmem_vm_close,
	.fault = ashmem_shmem_fault,
};

int
shmem_zero_setup(struct vm_area_struct *vma)
{
	vma->vm_ops = &ashmem_shmem_vmops;
	return (0);
}

void
ashmem_punch_hole(struct linux_file *f, loff_t start, loff_t len)
{
	vm_object_t obj = f->f_shmem;

	if (obj == NULL || len <= 0)
		return;
	VM_OBJECT_WLOCK(obj);
	vm_object_page_remove(obj, OFF_TO_IDX(start),
	    OFF_TO_IDX(start + len) - 1, 0);
	VM_OBJECT_WUNLOCK(obj);
}
