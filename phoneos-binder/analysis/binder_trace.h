/* SPDX-License-Identifier: GPL-2.0-only */
/* FreeBSD port: tracepoints stubbed out (no TRACE_EVENT infrastructure
 * on linuxkpi). */

#ifndef _BINDER_TRACE_H
#define _BINDER_TRACE_H

#define trace_binder_ioctl(...)				do { } while (0)
#define trace_binder_ioctl_done(...)			do { } while (0)
#define trace_binder_command(...)			do { } while (0)
#define trace_binder_return(...)			do { } while (0)
#define trace_binder_read_done(...)			do { } while (0)
#define trace_binder_write_done(...)			do { } while (0)
#define trace_binder_wait_for_work(...)			do { } while (0)
#define trace_binder_set_priority(...)			do { } while (0)
#define trace_binder_transaction(...)			do { } while (0)
#define trace_binder_transaction_alloc_buf(...)		do { } while (0)
#define trace_binder_transaction_buffer_release(...)	do { } while (0)
#define trace_binder_transaction_failed_buffer_release(...) do { } while (0)
#define trace_binder_transaction_fd_recv(...)		do { } while (0)
#define trace_binder_transaction_fd_send(...)		do { } while (0)
#define trace_binder_transaction_node_to_ref(...)	do { } while (0)
#define trace_binder_transaction_received(...)		do { } while (0)
#define trace_binder_transaction_ref_to_node(...)	do { } while (0)
#define trace_binder_transaction_ref_to_ref(...)	do { } while (0)
#define trace_binder_alloc_lru_end(...)			do { } while (0)
#define trace_binder_alloc_lru_start(...)		do { } while (0)
#define trace_binder_alloc_page_end(...)		do { } while (0)
#define trace_binder_alloc_page_start(...)		do { } while (0)
#define trace_binder_free_lru_end(...)			do { } while (0)
#define trace_binder_free_lru_start(...)		do { } while (0)
#define trace_binder_unmap_kernel_end(...)		do { } while (0)
#define trace_binder_unmap_kernel_start(...)		do { } while (0)
#define trace_binder_unmap_user_end(...)		do { } while (0)
#define trace_binder_unmap_user_start(...)		do { } while (0)
#define trace_binder_update_page_range(...)		do { } while (0)

#endif /* _BINDER_TRACE_H */
