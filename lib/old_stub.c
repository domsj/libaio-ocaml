CAMLprim value caml_aio_read_bytecode(value *argv, int argn) {
    return caml_aio_read_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
}


char * alloc_aligned_string(char **raw, size_t len) {
    *raw = malloc(2 * sizeof(value) + len + PAGESIZE);
    intptr_t res = (intptr_t)(*raw);
    if (*raw == NULL) return NULL;
    res += sizeof(value);
    res = (res + PAGESIZE - 1) & (~(PAGESIZE - 1));

    // DANGER: ocaml internal representation
    value result = (value)res;
    mlsize_t offset_index;
    mlsize_t wosize = (len + sizeof (value)) / sizeof (value);
    ((value*)res)[-1] = (wosize << 10) + String_tag;
    Field (result, wosize - 1) = 0;
    offset_index = Bsize_wsize (wosize) - 1;
    Byte (result, offset_index) = offset_index - len;

    //fprintf(stderr, "raw = %p, res = %p\n", *raw, (char*)res);
    return (char*)res;
}

/* unsafe_string : fun buffer -> string
external unsafe_string: buffer -> string = "caml_aio_unsafe_string"
*/
CAMLprim value caml_aio_unsafe_string(value ml_buffer) {
    CAMLparam1(ml_buffer);
    //fprintf(stderr, "### caml_aio_unsafe_string()\n");
    Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
    //fprintf(stderr, "buf = %p, buf->buf = %p\n", buf, buf->buf);

    CAMLreturn((value)buf->buf);
}

/* copy_string : fun buffer -> string
external copy_string: buffer -> string = "caml_aio_copy_string"
*/
CAMLprim value caml_aio_copy_string(value ml_buffer) {
    CAMLparam1(ml_buffer);
    CAMLlocal1(ml_str);
    //fprintf(stderr, "### caml_aio_copy_string()\n");
    Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
    ml_str = caml_alloc_string(buf->len);

    memcpy(String_val(ml_str), buf->buf, buf->len);
    
    CAMLreturn(ml_str);
}

/* get_string: fun buffer len -> string
external get_string: buffer -> int -> string = "caml_aio_get_string"
*/
CAMLprim value caml_aio_get_string(value ml_buffer, value ml_len) {
  CAMLparam2(ml_buffer, ml_len);
  CAMLlocal1(ml_str);
  //fprintf(stderr, "### caml_aio_get_string()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  size_t len = Int_val(ml_len);
  ml_str = caml_alloc_string(len);

  // FIXME: throw exception
  assert(buf->pos + len <= buf->len);

  memcpy(String_val(ml_str), &buf->buf[buf->pos], len);
  buf->pos += len;

  CAMLreturn(ml_str);
}

/* put_string: fun buffer string -> unit
external put_string: buffer -> string -> unit = "caml_aio_put_string"
*/
CAMLprim value caml_aio_put_string(value ml_buffer, value ml_str) {
  CAMLparam2(ml_buffer, ml_str);
  //fprintf(stderr, "### caml_aio_put_string()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  size_t len = string_length(ml_str);

  // FIXME: throw exception
  assert(buf->pos + len <= buf->len);

  memcpy(&buf->buf[buf->pos], String_val(ml_str), len);
  buf->pos += len;

  CAMLreturn(Val_unit);
}

/* get_string_at: fun buffer off len -> string
external get_string_at: buffer -> int -> int -> string = "caml_aio_get_string_at"
*/
CAMLprim value caml_aio_get_string_at(value ml_buffer, value ml_off, value ml_len) {
  CAMLparam3(ml_buffer, ml_off, ml_len);
  CAMLlocal1(ml_str);
  //fprintf(stderr, "### caml_aio_get_string()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  size_t off = Int_val(ml_off);
  size_t len = Int_val(ml_len);
  ml_str = caml_alloc_string(len);

  // FIXME: throw exception
  assert(off + len <= buf->len);

  memcpy(String_val(ml_str), &buf->buf[off], len);

  CAMLreturn(ml_str);
}

/* put_string_at: fun buffer off string -> unit
external put_string_at: buffer -> int -> string -> unit = "caml_aio_put_string_at"
*/
CAMLprim value caml_aio_put_string_at(value ml_buffer, value ml_off, value ml_str) {
  CAMLparam3(ml_buffer, ml_off , ml_str);
  //fprintf(stderr, "### caml_aio_put_string_at()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  size_t off = Int_val(ml_off);
  size_t len = string_length(ml_str);

  // FIXME: throw exception
  assert(off + len <= buf->len);

  memcpy(&buf->buf[off], String_val(ml_str), len);

  CAMLreturn(Val_unit);
}

/* get_int: fun buffer -> int
external get_int: buffer -> int = "caml_aio_get_int"
*/
CAMLprim value caml_aio_get_int(value ml_buffer) {
  CAMLparam1(ml_buffer);
  //fprintf(stderr, "### caml_aio_get_int()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  int32_t i;

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&i, &buf->buf[buf->pos], sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_int(ntohl(i)));
}

/* put_int: fun buffer -> int -> unit
external put_int: buffer -> int -> unit = "caml_aio_put_int"
*/
CAMLprim value caml_aio_put_int(value ml_buffer, value ml_i) {
  CAMLparam2(ml_buffer, ml_i);
  //fprintf(stderr, "### caml_aio_put_int()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  int32_t i = htonl(Int_val(ml_i));

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&buf->buf[buf->pos], &i, sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_unit);
}

/* get_int8: fun buffer -> int
external get_int8: buffer -> int = "caml_aio_get_int8"
*/
CAMLprim value caml_aio_get_int8(value ml_buffer) {
  CAMLparam1(ml_buffer);
  //fprintf(stderr, "### caml_aio_get_int8()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  int8_t i;

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&i, &buf->buf[buf->pos], sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_int(i));
}

/* put_int8: fun buffer -> int -> unit
external put_int8: buffer -> int -> unit = "caml_aio_put_int8"
*/
CAMLprim value caml_aio_put_int8(value ml_buffer, value ml_i) {
  CAMLparam2(ml_buffer, ml_i);
  //fprintf(stderr, "### caml_aio_put_int8()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  int8_t i = Int_val(ml_i);

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&buf->buf[buf->pos], &i, sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_unit);
}

/* get_uint8: fun buffer -> int
external get_uint8: buffer -> int = "caml_aio_get_uint8"
*/
CAMLprim value caml_aio_get_uint8(value ml_buffer) {
  CAMLparam1(ml_buffer);
  //fprintf(stderr, "### caml_aio_get_uint8()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  uint8_t i;

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&i, &buf->buf[buf->pos], sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_int(i));
}

/* put_uint8: fun buffer -> int -> unit
external put_uint8: buffer -> int -> unit = "caml_aio_put_uint8"
*/
CAMLprim value caml_aio_put_uint8(value ml_buffer, value ml_i) {
  CAMLparam2(ml_buffer, ml_i);
  //fprintf(stderr, "### caml_aio_put_uint8()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  uint8_t i = Int_val(ml_i);

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&buf->buf[buf->pos], &i, sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_unit);
}

/* get_int16: fun buffer -> int
external get_int16: buffer -> int = "caml_aio_get_int16"
*/
CAMLprim value caml_aio_get_int16(value ml_buffer) {
  CAMLparam1(ml_buffer);
  //fprintf(stderr, "### caml_aio_get_int16()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  int16_t i;

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&i, &buf->buf[buf->pos], sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_int(ntohs(i)));
}

/* put_int16: fun buffer -> int -> unit
external put_int16: buffer -> int -> unit = "caml_aio_put_int16"
*/
CAMLprim value caml_aio_put_int16(value ml_buffer, value ml_i) {
  CAMLparam2(ml_buffer, ml_i);
  //fprintf(stderr, "### caml_aio_put_int16()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  int16_t i = htons(Int_val(ml_i));

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&buf->buf[buf->pos], &i, sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_unit);
}

/* get_uint16: fun buffer -> int
external get_uint16: buffer -> int = "caml_aio_get_uint16"
*/
CAMLprim value caml_aio_get_uint16(value ml_buffer) {
  CAMLparam1(ml_buffer);
  //fprintf(stderr, "### caml_aio_get_uint16()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  uint16_t i;

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&i, &buf->buf[buf->pos], sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_int(ntohs(i)));
}

/* put_uint16: fun buffer -> int -> unit
external put_uint16: buffer -> int -> unit = "caml_aio_put_uint16"
*/
CAMLprim value caml_aio_put_uint16(value ml_buffer, value ml_i) {
  CAMLparam2(ml_buffer, ml_i);
  //fprintf(stderr, "### caml_aio_put_uint16()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  uint16_t i = htons(Int_val(ml_i));

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&buf->buf[buf->pos], &i, sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_unit);
}

/* get_int32: fun buffer -> int
external get_int32: buffer -> int = "caml_aio_get_int32"
*/
CAMLprim value caml_aio_get_int32(value ml_buffer) {
  CAMLparam1(ml_buffer);
  CAMLlocal1(ml_i);
  //fprintf(stderr, "### caml_aio_get_int32()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  int32_t i;

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&i, &buf->buf[buf->pos], sizeof(i));
  buf->pos += sizeof(i);

  ml_i = caml_copy_int32(i);

  CAMLreturn(ml_i);
}

/* put_int32: fun buffer -> int -> unit
external put_int32: buffer -> int -> unit = "caml_aio_put_int32"
*/
CAMLprim value caml_aio_put_int32(value ml_buffer, value ml_i) {
  CAMLparam2(ml_buffer, ml_i);
  //fprintf(stderr, "### caml_aio_put_int32()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  int32_t i = htons(Int32_val(ml_i));

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  memcpy(&buf->buf[buf->pos], &i, sizeof(i));
  buf->pos += sizeof(i);

  CAMLreturn(Val_unit);
}

/* get_uint56: fun buffer -> int
external get_int56: buffer -> int = "caml_aio_get_uint56"
*/
CAMLprim value caml_aio_get_uint56(value ml_buffer) {
  CAMLparam1(ml_buffer);
  CAMLlocal1(ml_i);
  //fprintf(stderr, "### caml_aio_get_uint56()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  uint64_t i;

  // FIXME: throw exception
  assert(buf->pos + 7 <= buf->len);

  i  = (uint64_t)buf->buf[buf->pos++] << 48;
  i += (uint64_t)buf->buf[buf->pos++] << 40;
  i += (uint64_t)buf->buf[buf->pos++] << 32;
  i += (uint64_t)buf->buf[buf->pos++] << 24;
  i += (uint64_t)buf->buf[buf->pos++] << 16;
  i += (uint64_t)buf->buf[buf->pos++] <<  8;
  i += (uint64_t)buf->buf[buf->pos++] <<  0;

  ml_i = caml_copy_int64(i);

  CAMLreturn(ml_i);
}

/* put_uint56: fun buffer -> int -> unit
external put_uint56: buffer -> int -> unit = "caml_aio_put_uint56"
*/
CAMLprim value caml_aio_put_uint56(value ml_buffer, value ml_i) {
  CAMLparam2(ml_buffer, ml_i);
  //fprintf(stderr, "### caml_aio_put_uint56()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  uint64_t i = Int64_val(ml_i);

  // FIXME: throw exception
  assert(buf->pos + 7 <= buf->len);

  buf->buf[buf->pos++] = i >> 48;
  buf->buf[buf->pos++] = i >> 40;
  buf->buf[buf->pos++] = i >> 32;
  buf->buf[buf->pos++] = i >> 24;
  buf->buf[buf->pos++] = i >> 16;
  buf->buf[buf->pos++] = i >>  8;
  buf->buf[buf->pos++] = i >>  0;

  CAMLreturn(Val_unit);
}

/* get_int64: fun buffer -> int
external get_int64: buffer -> int = "caml_aio_get_int64"
*/
CAMLprim value caml_aio_get_int64(value ml_buffer) {
  CAMLparam1(ml_buffer);
  CAMLlocal1(ml_i);
  //fprintf(stderr, "### caml_aio_get_int64()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  int64_t i;

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  i  = (uint64_t)buf->buf[buf->pos++] << 56;
  i += (uint64_t)buf->buf[buf->pos++] << 48;
  i += (uint64_t)buf->buf[buf->pos++] << 40;
  i += (uint64_t)buf->buf[buf->pos++] << 32;
  i += (uint64_t)buf->buf[buf->pos++] << 24;
  i += (uint64_t)buf->buf[buf->pos++] << 16;
  i += (uint64_t)buf->buf[buf->pos++] <<  8;
  i += (uint64_t)buf->buf[buf->pos++] <<  0;

  ml_i = caml_copy_int64(i);

  CAMLreturn(ml_i);
}

/* put_int64: fun buffer -> int -> unit
external put_int64: buffer -> int -> unit = "caml_aio_put_int64"
*/
CAMLprim value caml_aio_put_int64(value ml_buffer, value ml_i) {
  CAMLparam2(ml_buffer, ml_i);
  //fprintf(stderr, "### caml_aio_put_int64()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  int64_t i = Int64_val(ml_i);

  // FIXME: throw exception
  assert(buf->pos + sizeof(i) <= buf->len);

  buf->buf[buf->pos++] = i >> 56;
  buf->buf[buf->pos++] = i >> 48;
  buf->buf[buf->pos++] = i >> 40;
  buf->buf[buf->pos++] = i >> 32;
  buf->buf[buf->pos++] = i >> 24;
  buf->buf[buf->pos++] = i >> 16;
  buf->buf[buf->pos++] = i >>  8;
  buf->buf[buf->pos++] = i >>  0;

  CAMLreturn(Val_unit);
}

/* rewind: fun buffer -> unit
external rewind: buffer -> unit = "caml_aio_rewind"
*/
CAMLprim value caml_aio_rewind(value ml_buffer) {
  CAMLparam1(ml_buffer);
  //fprintf(stderr, "### caml_aio_rewind()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  buf->pos = 0;

  CAMLreturn(Val_unit);
}

/* seek: fun buffer -> offset -> unit
external seek: buffer -> offset -> unit = "caml_aio_seek"
*/
CAMLprim value caml_aio_seek(value ml_buffer, value ml_offset) {
  CAMLparam2(ml_buffer, ml_offset);
  //fprintf(stderr, "### caml_aio_seek()\n");
  Buffer *buf = (Buffer*)Data_custom_val(ml_buffer);
  buf->pos = Int_val(ml_offset);

  CAMLreturn(Val_unit);
}
