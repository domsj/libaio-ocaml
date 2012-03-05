/* buffer_stubs.c: C bindings for Buffer module
 * Copyright (C) 2010 Goswin von Brederlow
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * Under Debian a copy can be found in /usr/share/common-licenses/LGPL-3.
 */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <endian.h>
#include <stdint.h>

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/bigarray.h>

#define PAGE_SIZE (sysconf(_SC_PAGESIZE))

value caml_aio_buffer_page_size(void) {
    return Val_int(PAGE_SIZE);
}

CAMLprim value caml_aio_buffer_create(value ml_size) {
    CAMLparam1(ml_size);
    size_t size = Int_val(ml_size);

    if (size % PAGE_SIZE != 0) {
	caml_invalid_argument("Buffer.create: Size not multiple of PAGE_SIZE.");
    }

    void *mem;
    int res = posix_memalign(&mem, PAGE_SIZE, size);
    switch(res) {
    case EINVAL: caml_failwith("Buffer.create: Alignment error. This should never happen.");
    case ENOMEM: caml_failwith("Buffer.create: Out of memory.");
    case 0: break;
    default: caml_failwith("Buffer.create: Unknown error.");
    }

    CAMLreturn(caml_ba_alloc_dims(CAML_BA_UINT8 | CAML_BA_C_LAYOUT | CAML_BA_MANAGED, 1, mem, size));
}

value caml_aio_buffer_get_int8(value ml_buf, value ml_off) {
    int8_t *buf = (int8_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    return Val_int(buf[off]);
}

value caml_aio_buffer_set_int8(value ml_buf, value ml_off, value ml_x) {
    int8_t *buf = (int8_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    uint8_t x = Int_val(ml_x);
    buf[off] = x;
    return Val_unit;
}

value caml_aio_buffer_get_uint16(value ml_buf, value ml_off) {
    uint16_t *buf = (uint16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    return Val_int(buf[off / 2]);
}

value caml_aio_buffer_set_uint16(value ml_buf, value ml_off, value ml_x) {
    uint16_t *buf = (uint16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    uint16_t x = Int_val(ml_x);
    buf[off / 2] = x;
    return Val_unit;
}

value caml_aio_buffer_get_int16(value ml_buf, value ml_off) {
    int16_t *buf = (int16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    return Val_int(buf[off / 2]);
}

value caml_aio_buffer_set_int16(value ml_buf, value ml_off, value ml_x) {
    int16_t *buf = (int16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int16_t x = Int_val(ml_x);
    buf[off / 2] = x;
    return Val_unit;
}

value caml_aio_buffer_get_int31(value ml_buf, value ml_off) {
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    return Val_int(buf[off / 4]);
}

value caml_aio_buffer_set_int31(value ml_buf, value ml_off, value ml_x) {
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int32_t x = Int_val(ml_x);
    buf[off / 4] = x;
    return Val_unit;
}

CAMLprim value caml_aio_buffer_get_int32(value ml_buf, value ml_off) {
    CAMLparam2(ml_buf, ml_off);
    CAMLlocal1(ml_res);
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    CAMLreturn(caml_copy_int32(buf[off / 4]));
}

value caml_aio_buffer_set_int32(value ml_buf, value ml_off, value ml_x) {
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int32_t x = Int32_val(ml_x);
    buf[off / 4] = x;
    return Val_unit;
}

CAMLprim value caml_aio_buffer_get_int64(value ml_buf, value ml_off) {
    CAMLparam2(ml_buf, ml_off);
    CAMLlocal1(ml_res);
    int64_t *buf = (int64_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    CAMLreturn(caml_copy_int64(buf[off / 8]));
}

value caml_aio_buffer_set_int64(value ml_buf, value ml_off, value ml_x) {
    int64_t *buf = (int64_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int64_t x = Int64_val(ml_x);
    buf[off / 8] = x;
    return Val_unit;
}

// external unsafe_get_substr : t -> int -> int -> string = "caml_aio_buf_unsafe_get_substr_stub"
value caml_aio_buf_unsafe_get_substr_stub(value ml_buf, value ml_off, value ml_len) {
    CAMLparam3(ml_buf, ml_off, ml_len);
    CAMLlocal1(ml_str);
    
    const char *src = Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    size_t len = Int_val(ml_len);
    ml_str = caml_alloc_string(len);
    char *dst = String_val(ml_str);
    memcpy(dst, &src[off], len);

    CAMLreturn(ml_str);
}

// external unsafe_set_substr : t -> int -> int -> string = "caml_aio_buf_unsafe_set_substr_stub"
value caml_aio_buf_unsafe_set_substr_stub(value ml_buf, value ml_off, value ml_str) {
    CAMLparam3(ml_buf, ml_off, ml_str);
    
    char *dst = Data_bigarray_val(ml_buf);
    const char *src = String_val(ml_str);
    size_t off = Int_val(ml_off);
    size_t len = caml_string_length(ml_str);
    memcpy(&dst[off], src, len);

    CAMLreturn(Val_unit);
}

// Big endian byte order
value caml_aio_buffer_get_be_int16(value ml_buf, value ml_off) {
    int16_t *buf = (int16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    return Val_int(be16toh(buf[off / 2]));
}

value caml_aio_buffer_set_be_int16(value ml_buf, value ml_off, value ml_x) {
    int16_t *buf = (int16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int16_t x = Int_val(ml_x);
    buf[off / 2] = htobe16(x);
    return Val_unit;
}

value caml_aio_buffer_get_be_uint16(value ml_buf, value ml_off) {
    uint16_t *buf = (uint16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    return Val_int(be16toh(buf[off / 2]));
}

value caml_aio_buffer_set_be_uint16(value ml_buf, value ml_off, value ml_x) {
    uint16_t *buf = (uint16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    uint16_t x = Int_val(ml_x);
    buf[off / 2] = htobe16(x);
    return Val_unit;
}

value caml_aio_buffer_get_be_int31(value ml_buf, value ml_off) {
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    return Val_int(be32toh(buf[off / 4]));
}

value caml_aio_buffer_set_be_int31(value ml_buf, value ml_off, value ml_x) {
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int32_t x = Int_val(ml_x);
    buf[off / 4] = htobe32(x);
    return Val_unit;
}

CAMLprim value caml_aio_buffer_get_be_int32(value ml_buf, value ml_off) {
    CAMLparam2(ml_buf, ml_off);
    CAMLlocal1(ml_res);
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    CAMLreturn(caml_copy_int32(be32toh(buf[off / 4])));
}

value caml_aio_buffer_set_be_int32(value ml_buf, value ml_off, value ml_x) {
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int32_t x = Int32_val(ml_x);
    buf[off / 4] = htobe32(x);
    return Val_unit;
}

CAMLprim value caml_aio_buffer_get_be_int64(value ml_buf, value ml_off) {
    CAMLparam2(ml_buf, ml_off);
    CAMLlocal1(ml_res);
    int64_t *buf = (int64_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    CAMLreturn(caml_copy_int64(be64toh(buf[off / 8])));
}
#include <stdio.h>
value caml_aio_buffer_set_be_int64(value ml_buf, value ml_off, value ml_x) {
    int64_t *buf = (int64_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int64_t x = Int64_val(ml_x);
    buf[off / 8] = htobe64(x);
    return Val_unit;
}


// Little endian byte order
value caml_aio_buffer_get_le_int16(value ml_buf, value ml_off) {
    int16_t *buf = (int16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    return Val_int(le16toh(buf[off / 2]));
}

value caml_aio_buffer_set_le_int16(value ml_buf, value ml_off, value ml_x) {
    int16_t *buf = (int16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int16_t x = Int_val(ml_x);
    buf[off / 2] = htole16(x);
    return Val_unit;
}

value caml_aio_buffer_get_le_uint16(value ml_buf, value ml_off) {
    uint16_t *buf = (uint16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    return Val_int(le16toh(buf[off / 2]));
}

value caml_aio_buffer_set_le_uint16(value ml_buf, value ml_off, value ml_x) {
    uint16_t *buf = (uint16_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    uint16_t x = Int_val(ml_x);
    buf[off / 2] = htole16(x);
    return Val_unit;
}

value caml_aio_buffer_get_le_int31(value ml_buf, value ml_off) {
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    return Val_int(le32toh(buf[off / 4]));
}

value caml_aio_buffer_set_le_int31(value ml_buf, value ml_off, value ml_x) {
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int32_t x = Int_val(ml_x);
    buf[off / 4] = htole32(x);
    return Val_unit;
}

CAMLprim value caml_aio_buffer_get_le_int32(value ml_buf, value ml_off) {
    CAMLparam2(ml_buf, ml_off);
    CAMLlocal1(ml_res);
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    CAMLreturn(caml_copy_int32(le32toh(buf[off / 4])));
}

value caml_aio_buffer_set_le_int32(value ml_buf, value ml_off, value ml_x) {
    int32_t *buf = (int32_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int32_t x = Int32_val(ml_x);
    buf[off / 4] = htole32(x);
    return Val_unit;
}

CAMLprim value caml_aio_buffer_get_le_int64(value ml_buf, value ml_off) {
    CAMLparam2(ml_buf, ml_off);
    CAMLlocal1(ml_res);
    int64_t *buf = (int64_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    CAMLreturn(caml_copy_int64(le64toh(buf[off / 8])));
}
#include <stdio.h>
value caml_aio_buffer_set_le_int64(value ml_buf, value ml_off, value ml_x) {
    int64_t *buf = (int64_t*)Data_bigarray_val(ml_buf);
    size_t off = Int_val(ml_off);
    int64_t x = Int64_val(ml_x);
    buf[off / 8] = htole64(x);
    return Val_unit;
}
