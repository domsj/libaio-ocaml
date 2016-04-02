/* aio_stubs.c: C bindings for Aio module
 * Copyright (C) 2010 Goswin von Brederlow
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * Under Debian a copy can be found in /usr/share/common-licenses/LGPL-2.1.
 */

#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <libaio.h>
#include <sys/eventfd.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/signals.h>
#include <caml/custom.h>
#include <caml/bigarray.h>

typedef struct Context {
  io_context_t ctx;
  int max_ios;
  int pending;
  int fd;
  struct iocb *iocbs[0];
} Context;

CAMLprim value caml_aio_run(value context);

void caml_aio_context_finalize(value v) {
  Context *ctx = (Context*)Data_custom_val(v);
  //fprintf(stderr, "### caml_aio_context_finalize()\n");
  if (ctx->pending > 0) {
    fprintf(stderr, "Error: pending io on Aio.context.\n");
    fflush(stderr);
  }
  close(ctx->fd);
  // FIXME: Can't throw exception. What's to do?
  assert(io_queue_release(ctx->ctx) == 0);
}

static struct custom_operations caml_aio_context_ops = {
  "vonbrederlow.de.aio.context",
  caml_aio_context_finalize,
  custom_compare_default,
  custom_hash_default,
  custom_serialize_default,
  custom_deserialize_default,
  custom_compare_ext_default,
};

/* context: fun max_ios -> context
external context: int -> context = "caml_aio_context"
*/
CAMLprim value caml_aio_context(value ml_max_ios) {
  CAMLparam1(ml_max_ios);
  CAMLlocal2(ml_ctx, ml_context);
  int max_ios = Int_val(ml_max_ios);
  intptr_t i;
  //fprintf(stderr, "### caml_aio_context(%d)\n", max_ios);

  /*
   * context
   * callback
   * ...
   */
  ml_context = caml_alloc_custom(&caml_aio_context_ops, sizeof(Context) + 2 * max_ios * sizeof(struct iocb*), 0, 1);
  ml_ctx = caml_alloc_tuple(2 * max_ios + 1);
  Store_field(ml_ctx, 0, ml_context);
  for(i = 1; i <= 2 * max_ios; ++i) {
    Store_field(ml_ctx, i, Val_unit);
  }
  Context *context = (Context*)Data_custom_val(ml_context);
  memset(context, 0, sizeof(Context) + max_ios * sizeof(struct iocb*));
  for(i = 0; i < max_ios; ++i) {
    context->iocbs[i] = malloc(sizeof(struct iocb));
    // FIXME: throw exception
    assert(context->iocbs[i]);
    memset(context->iocbs[i], 0, sizeof(struct iocb));
    context->iocbs[i]->data = (void*)(2 * i + 1);
  }

  // FIXME: throw exception
  assert(io_queue_init(max_ios, &context->ctx) == 0);
  context->max_ios = max_ios;
  context->pending = 0;
  context->fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  // FIXME: throw exception
  assert(context->fd != -1);

  CAMLreturn(ml_ctx);
}


/* read: fun ctx fd fd_off buf fn -> ()
external read : context -> Unix.file_descr -> int64 -> Buffer.t ->
                (result -> unit) -> unit = "caml_aio_read"
*/
CAMLprim value caml_aio_read(value ml_ctx, value ml_fd, value ml_fd_off, value ml_buffer, value ml_fn) {
  CAMLparam5(ml_ctx, ml_fd, ml_fd_off, ml_buffer, ml_fn);
  //fprintf(stderr, "### caml_aio_read()\n");
  Context *ctx = (Context*)Data_custom_val(Field(ml_ctx, 0));
  // FIXME: throw exception
  assert(ctx->pending < ctx->max_ios);
  int fd = Int_val(ml_fd);
  uint64_t fd_off = Int64_val(ml_fd_off);
  void *buf = Data_bigarray_val(ml_buffer);
  size_t len = Bigarray_val(ml_buffer)->dim[0];
  struct iocb **iocbs = &ctx->iocbs[ctx->pending];
  struct iocb *iocb = iocbs[0];
  intptr_t slot = (intptr_t)iocb->data;

  memset(iocb, 0, sizeof(struct iocb));
  io_prep_pread(iocb, fd, buf, len, fd_off);
  io_set_eventfd(iocb, ctx->fd);

  iocb->data = (void*)slot;
  Store_field(ml_ctx, slot, ml_fn);
  Store_field(ml_ctx, slot + 1, ml_buffer);

  // FIXME: throw exception
  assert(io_submit(ctx->ctx, 1, iocbs) == 1);
  ++ctx->pending;

  CAMLreturn(Val_unit);
}

/* write: fun ctx fd fd_off buf fn -> ()
external write : context -> Unix.file_descr -> int64 -> buffer ->
                 (result -> unit) -> unit = "caml_aio_write"
*/
CAMLprim value caml_aio_write(value ml_ctx, value ml_fd, value ml_fd_off, value ml_buffer, value ml_fn) {
  CAMLparam5(ml_ctx, ml_fd, ml_fd_off, ml_buffer, ml_fn);
  //fprintf(stderr, "### caml_aio_write()\n");
  Context *ctx = (Context*)Data_custom_val(Field(ml_ctx, 0));
  // FIXME: throw exception
  assert(ctx->pending < ctx->max_ios);
  int fd = Int_val(ml_fd);
  uint64_t fd_off = Int64_val(ml_fd_off);
  void *buf = Data_bigarray_val(ml_buffer);
  size_t len = Bigarray_val(ml_buffer)->dim[0];
  struct iocb **iocbs = &ctx->iocbs[ctx->pending];
  struct iocb *iocb = iocbs[0];
  intptr_t slot = (intptr_t)iocb->data;

  memset(iocb, 0, sizeof(struct iocb));
  io_prep_pwrite(iocb, fd, buf, len, fd_off);
  io_set_eventfd(iocb, ctx->fd);
  iocb->data = (void*)slot;

  Store_field(ml_ctx, slot, ml_fn);
  Store_field(ml_ctx, slot + 1, ml_buffer);

  // FIXME: throw exception
  assert(io_submit(ctx->ctx, 1, iocbs) == 1);
  ++ctx->pending;

  CAMLreturn(Val_unit);
}

/* poll: fun ctx fd events fn -> ()
external poll : context -> Unix.file_descr -> int -> (Unix.file_descr -> unit) -> unit = "caml_aio_poll"
*/
CAMLprim value caml_aio_poll(value ml_ctx, value ml_fd, value ml_events, value ml_fn) {
  CAMLparam4(ml_ctx, ml_fd, ml_events, ml_fn);
  //fprintf(stderr, "### caml_aio_poll()\n");
  Context *ctx = (Context*)Data_custom_val(Field(ml_ctx, 0));
  // FIXME: throw exception
  assert(ctx->pending < ctx->max_ios);
  int fd = Int_val(ml_fd);
  int events = Int_val(ml_events);
  struct iocb **iocbs = &ctx->iocbs[ctx->pending];
  struct iocb *iocb = iocbs[0];
  intptr_t slot = (intptr_t)iocb->data;

  memset(iocb, 0, sizeof(struct iocb));
  io_prep_poll(iocb, fd, events);
  io_set_eventfd(iocb, ctx->fd);
  iocb->data = (void*)slot;
  Store_field(ml_ctx, slot, ml_fn);
  Store_field(ml_ctx, slot + 1, Val_unit);

  // FIXME: throw exception
  //assert(io_submit(ctx->ctx, 1, iocbs) == 1);
  int res = io_submit(ctx->ctx, 1, iocbs);
  fprintf(stderr, "res = %d\n", res);
  perror("poll");
  ++ctx->pending;

  CAMLreturn(Val_unit);
}

/* run: fun ctx -> ()
external run : context -> unit = "caml_aio_run"
*/
CAMLprim value caml_aio_run(value ml_ctx) {
  CAMLparam1(ml_ctx);
  CAMLlocal2(ml_fn, ml_buf);
  static value * call_result  = NULL;
  static value * call_error   = NULL;
  static value * call_partial = NULL;
  //fprintf(stderr, "### caml_aio_run()\n");
  Context *ctx = (Context*)Data_custom_val(Field(ml_ctx, 0));
  uint64_t num;

  while(ctx->pending > 0) {
    struct io_event events[ctx->pending];
    struct io_event *ep;
    int n;

    // Wait for at least one event
    n = io_getevents(ctx->ctx, 1, ctx->pending, events, NULL);
    //fprintf(stderr, "### caml_aio_run(): n = %d\n", n);
    if (n <= 0) break;

    // process callbacks
    for(ep = events; n-- > 0; ep++) {
      struct iocb *iocb = ep->obj;
      intptr_t slot = (intptr_t)iocb->data;
      //fprintf(stderr, "### caml_aio_run(): slot = %"PRIdPTR"\n", slot);

      // Get callback and buffer
      ml_fn = Field(ml_ctx, slot);
      ml_buf = Field(ml_ctx, slot + 1);

      // Remove callback and buffer and free iocb
      --ctx->pending;
      Store_field(ml_ctx, slot, Val_unit);
      Store_field(ml_ctx, slot + 1, Val_unit);
      ctx->iocbs[ctx->pending] = iocb;

      // Execute callback
      if (ep->res2 != 0) {
	  if (call_error == NULL) {
	      /* First time around, look up by name */
	      call_error = caml_named_value("caml_aio_call_error");
	  }
	  caml_callback2(*call_error, ml_fn, Val_int(ep->res2));
      } else if (ep->res != (size_t)Bigarray_val(ml_buf)->dim[0]) {
	  if (call_partial == NULL) {
	      /* First time around, look up by name */
	      call_partial = caml_named_value("caml_aio_call_partial");
	  }
	  caml_callback3(*call_partial, ml_fn, ml_buf, Val_int(ep->res));
      } else {
	  if (call_result == NULL) {
	      /* First time around, look up by name */
	      call_result = caml_named_value("caml_aio_call_result");
	  }
	  caml_callback2(*call_result, ml_fn, ml_buf);
      }
    }
  }
  // Clear eventfd
  // FIXME: throw exception
  (void)read(ctx->fd, &num, sizeof(num));

  //fprintf(stderr, "### caml_aio_run(): done\n");
  CAMLreturn(Val_unit);
}

/* process: fun ctx -> ()
external process : context -> unit = "caml_aio_process"
*/
CAMLprim value caml_aio_process(value ml_ctx) {
  CAMLparam1(ml_ctx);
  CAMLlocal2(ml_fn, ml_buf);
  static value * call_result  = NULL;
  static value * call_error   = NULL;
  static value * call_partial = NULL;
  //fprintf(stderr, "### caml_aio_process()\n");
  Context *ctx = (Context*)Data_custom_val(Field(ml_ctx, 0));
  uint64_t num;

  // FIXME: throw exception
  assert(read(ctx->fd, &num, sizeof(num)) == sizeof(num));
  if (num == 0) CAMLreturn(Val_unit);

  struct io_event events[num];
  struct io_event *ep;
  int n;

  // Wait for at least one event (we know we have num events :)
  n = io_getevents(ctx->ctx, 1, num, events, NULL);
  //fprintf(stderr, "### caml_aio_process(): n = %d\n", n);
  //FIXME: throw exception
  assert(n > 0);

  // process callbacks
  for(ep = events; n-- > 0; ep++) {
    struct iocb *iocb = ep->obj;
    intptr_t slot = (intptr_t)iocb->data;
    //fprintf(stderr, "### caml_aio_process(): slot = %"PRIdPTR"\n", slot);

    // Get callback and buffer
    ml_fn = Field(ml_ctx, slot);
    ml_buf = Field(ml_ctx, slot + 1);

    // Remove callback and buffer and free iocb
    --ctx->pending;
    Store_field(ml_ctx, slot, Val_unit);
    Store_field(ml_ctx, slot + 1, Val_unit);
    ctx->iocbs[ctx->pending] = iocb;

    // Execute callback
    if (ep->res2 != 0) {
	if (call_error == NULL) {
	    /* First time around, look up by name */
	    call_error = caml_named_value("caml_aio_call_error");
	}
	caml_callback2(*call_error, ml_fn, Val_int(ep->res2));
    } else if (ep->res != (size_t)Bigarray_val(ml_buf)->dim[0]) {
	if (call_partial == NULL) {
	    /* First time around, look up by name */
	    call_partial = caml_named_value("caml_aio_call_partial");
	}
	caml_callback3(*call_partial, ml_fn, ml_buf, Val_int(ep->res));
    } else {
	if (call_result == NULL) {
	    /* First time around, look up by name */
	    call_result = caml_named_value("caml_aio_call_result");
	}
	caml_callback2(*call_result, ml_fn, ml_buf);
    }
  }

  //fprintf(stderr, "### caml_aio_process(): done\n");
  CAMLreturn(Val_unit);
}

/* fd: fun ctx -> Unix.file_descr
external fd : context -> fd = "caml_aio_fd"
*/
CAMLprim value caml_aio_fd(value ml_ctx) {
  CAMLparam1(ml_ctx);
  Context *ctx = (Context*)Data_custom_val(Field(ml_ctx, 0));    
  CAMLreturn(Val_int(ctx->fd));
}

/* sync_read: fun fd fd_off buf -> unit
external sync_read : Unix.file_descr -> int64 -> buffer -> unit = "caml_aio_sync_read"
*/
CAMLprim value caml_aio_sync_read(value ml_fd, value ml_fd_off, value ml_buffer) {
  CAMLparam3(ml_fd, ml_fd_off, ml_buffer);
  //fprintf(stderr, "### caml_aio_sync_read()\n");
  int fd = Int_val(ml_fd);
  uint64_t fd_off = Int64_val(ml_fd_off);
  void *buf = Data_bigarray_val(ml_buffer);
  size_t len = Bigarray_val(ml_buffer)->dim[0];

  ssize_t res = pread(fd, buf, len, fd_off);
  // FIXME: throw exception on error
  assert(res >= 0 && (size_t)res == len);
  CAMLreturn(Val_unit);
}

/* sync_write: fun fd fd_off buf buf_off buf_len -> int
external sync_write : Unix.file_descr -> int64 -> buffer -> int -> int -> int = "caml_aio_sync_write"
*/
CAMLprim value caml_aio_sync_write(value ml_fd, value ml_fd_off, value ml_buffer, value ml_buf_off, value ml_buf_len) {
  CAMLparam5(ml_fd, ml_fd_off, ml_buffer, ml_buf_off, ml_buf_len);
  //fprintf(stderr, "### caml_aio_sync_write()\n");
  int fd = Int_val(ml_fd);
  uint64_t fd_off = Int64_val(ml_fd_off);
  void *buf = Data_bigarray_val(ml_buffer);
  size_t len = Bigarray_val(ml_buffer)->dim[0];

  ssize_t res = pwrite(fd, buf, len, fd_off);
  // FIXME: throw exception on error
  assert(res >= 0 && (size_t)res == len);
  CAMLreturn(Val_int(len));
}
