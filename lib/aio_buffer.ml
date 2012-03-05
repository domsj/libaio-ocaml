(* buffer.ml: Buffer module for libaio-ocaml
 * Copyright (C) 2009 Goswin von Brederlow
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
 *)

open Bigarray

type t = (int, int8_unsigned_elt, c_layout) Array1.t

exception Unaligned

external page_size : unit -> int = "caml_aio_buffer_page_size" "noalloc"
external create : int -> t = "caml_aio_buffer_create"

let clear (buf : t) =
  Array1.fill (buf :> (int, int8_unsigned_elt, c_layout) Array1.t) 0

let length (buf : t) =
  Array1.dim (buf :> (int, int8_unsigned_elt, c_layout) Array1.t)

let check (buf : t) off size =
  if off mod size <> 0
  then raise Unaligned
  else if (off < 0) || (length buf < off + size)
  then raise (Invalid_argument "Index out of bounds")
  else ()

let unsafe_get_uint8 (buf : t) off =
  Array1.unsafe_get (buf :> (int, int8_unsigned_elt, c_layout) Array1.t) off

let unsafe_set_uint8 (buf : t) off x =
  Array1.unsafe_set (buf :> (int, int8_unsigned_elt, c_layout) Array1.t) off x

(* Fails in bytecode
let unsafe_get_int8 (buf : t) off =
  let buf = ((Obj.magic buf) : (int, int8_signed_elt, c_layout) Array1.t) 
  in
    Array1.unsafe_get buf off

let unsafe_set_int8 (buf : t) off x =
  let buf = ((Obj.magic buf) : (int, int8_signed_elt, c_layout) Array1.t)
  in
    Array1.unsafe_set buf off x
*)

external unsafe_get_int8 : t -> int -> int = "caml_aio_buffer_get_int8" "noalloc"
external unsafe_set_int8 : t -> int -> int -> unit = "caml_aio_buffer_set_int8" "noalloc"
external unsafe_get_uint16 : t -> int -> int = "caml_aio_buffer_get_uint16" "noalloc"
external unsafe_set_uint16 : t -> int -> int -> unit = "caml_aio_buffer_set_uint16" "noalloc"
external unsafe_get_int16 : t -> int -> int = "caml_aio_buffer_get_int16" "noalloc"
external unsafe_set_int16 : t -> int -> int -> unit = "caml_aio_buffer_set_int16" "noalloc"
external unsafe_get_int31 : t -> int -> int = "caml_aio_buffer_get_int31" "noalloc"
external unsafe_set_int31 : t -> int -> int -> unit = "caml_aio_buffer_set_int31" "noalloc"
external unsafe_get_int32 : t -> int -> int32 = "caml_aio_buffer_get_int32"
external unsafe_set_int32 : t -> int -> int32 -> unit = "caml_aio_buffer_set_int32" "noalloc"
external unsafe_get_int64 : t -> int -> int64 = "caml_aio_buffer_get_int64"
external unsafe_set_int64 : t -> int -> int64 -> unit = "caml_aio_buffer_set_int64" "noalloc"

let get_uint8 (buf : t) off =
  check buf off 1;
  unsafe_get_uint8 buf off

let get_int8 buf off =
  check buf off 1;
  unsafe_get_int8 buf off

let get_uint16 buf off =
  check buf off 2;
  unsafe_get_uint16 buf off

let get_int16 buf off =
  check buf off 2;
  unsafe_get_int16 buf off

let get_int31 buf off =
  check buf off 4;
  unsafe_get_int31 buf off

let get_int32 buf off =
  check buf off 4;
  unsafe_get_int32 buf off

let get_int64 buf off =
  check buf off 8;
  unsafe_get_int64 buf off


let set_uint8 (buf : t) off x =
  check buf off 1;
  unsafe_set_uint8 buf off x

let set_int8 buf off x =
  check buf off 1;
  unsafe_set_int8 buf off x

let set_uint16 buf off x =
  check buf off 2;
  unsafe_set_uint16 buf off x

let set_int16 buf off x =
  check buf off 2;
  unsafe_set_int16 buf off x

let set_int31 buf off x =
  check buf off 4;
  unsafe_set_int31 buf off x

let set_int32 buf off x =
  check buf off 4;
  unsafe_set_int32 buf off x

let set_int64 buf off x =
  check buf off 8;
  unsafe_set_int64 buf off x

external unsafe_get_substr : t -> int -> int -> string = "caml_aio_buf_unsafe_get_substr_stub"

let get_substr buf off len =
  let dim = Array1.dim buf
  in
    if dim < off
    then raise (Invalid_argument "Index out of bound.");
    if dim - off < len
    then raise (Invalid_argument "Index out of bound.");
    unsafe_get_substr buf off len

let get_str buf =
  unsafe_get_substr buf 0 (Array1.dim buf)

external unsafe_set_substr : t -> int -> string -> unit = "caml_aio_buf_unsafe_set_substr_stub"

let set_substr buf off str =
  let dim = Array1.dim buf in
  let len = String.length str
  in
    if dim < off
    then raise (Invalid_argument "Index out of bound.");
    if dim - off < len
    then raise (Invalid_argument "Index out of bound.");
    unsafe_set_substr buf off str

let set_str str =
  let len = String.length str in
  let buf = create len
  in
    unsafe_set_substr buf 0 str;
    buf


(* Big endian byte order *)
external unsafe_get_be_int16 : t -> int -> int = "caml_aio_buffer_get_be_int16" "noalloc"
external unsafe_set_be_int16 : t -> int -> int -> unit = "caml_aio_buffer_set_be_int16" "noalloc"
external unsafe_get_be_uint16 : t -> int -> int = "caml_aio_buffer_get_be_uint16" "noalloc"
external unsafe_set_be_uint16 : t -> int -> int -> unit = "caml_aio_buffer_set_be_uint16" "noalloc"
external unsafe_get_be_int31 : t -> int -> int = "caml_aio_buffer_get_be_int31" "noalloc"
external unsafe_set_be_int31 : t -> int -> int -> unit = "caml_aio_buffer_set_be_int31" "noalloc"
external unsafe_get_be_int32 : t -> int -> int32 = "caml_aio_buffer_get_be_int32"
external unsafe_set_be_int32 : t -> int -> int32 -> unit = "caml_aio_buffer_set_be_int32" "noalloc"
external unsafe_get_be_int64 : t -> int -> int64 = "caml_aio_buffer_get_be_int64"
external unsafe_set_be_int64 : t -> int -> int64 -> unit = "caml_aio_buffer_set_be_int64" "noalloc"

let get_be_int16 buf off =
  check buf off 2;
  unsafe_get_be_int16 buf off

let get_be_uint16 buf off =
  check buf off 2;
  unsafe_get_be_uint16 buf off

let get_be_int31 buf off =
  check buf off 4;
  unsafe_get_be_int31 buf off

let get_be_int32 buf off =
  check buf off 4;
  unsafe_get_be_int32 buf off

let get_be_int64 buf off =
  check buf off 8;
  unsafe_get_be_int64 buf off

let set_be_int16 buf off x =
  check buf off 2;
  unsafe_set_be_int16 buf off x

let set_be_uint16 buf off x =
  check buf off 2;
  unsafe_set_be_int16 buf off x

let set_be_int31 buf off x =
  check buf off 4;
  unsafe_set_be_int31 buf off x

let set_be_int32 buf off x =
  check buf off 4;
  unsafe_set_be_int32 buf off x

let set_be_int64 buf off x =
  check buf off 8;
  unsafe_set_be_int64 buf off x


(* Little endian byte order *)
external unsafe_get_le_int16 : t -> int -> int = "caml_aio_buffer_get_le_int16" "noalloc"
external unsafe_set_le_int16 : t -> int -> int -> unit = "caml_aio_buffer_set_le_int16" "noalloc"
external unsafe_get_le_uint16 : t -> int -> int = "caml_aio_buffer_get_le_uint16" "noalloc"
external unsafe_set_le_uint16 : t -> int -> int -> unit = "caml_aio_buffer_set_le_uint16" "noalloc"
external unsafe_get_le_int31 : t -> int -> int = "caml_aio_buffer_get_le_int31" "noalloc"
external unsafe_set_le_int31 : t -> int -> int -> unit = "caml_aio_buffer_set_le_int31" "noalloc"
external unsafe_get_le_int32 : t -> int -> int32 = "caml_aio_buffer_get_le_int32"
external unsafe_set_le_int32 : t -> int -> int32 -> unit = "caml_aio_buffer_set_le_int32" "noalloc"
external unsafe_get_le_int64 : t -> int -> int64 = "caml_aio_buffer_get_le_int64"
external unsafe_set_le_int64 : t -> int -> int64 -> unit = "caml_aio_buffer_set_le_int64" "noalloc"

let get_le_int16 buf off =
  check buf off 2;
  unsafe_get_le_int16 buf off

let get_le_uint16 buf off =
  check buf off 2;
  unsafe_get_le_uint16 buf off

let get_le_int31 buf off =
  check buf off 4;
  unsafe_get_le_int31 buf off

let get_le_int32 buf off =
  check buf off 4;
  unsafe_get_le_int32 buf off

let get_le_int64 buf off =
  check buf off 8;
  unsafe_get_le_int64 buf off

let set_le_int16 buf off x =
  check buf off 2;
  unsafe_set_le_int16 buf off x

let set_le_uint16 buf off x =
  check buf off 2;
  unsafe_set_le_int16 buf off x

let set_le_int31 buf off x =
  check buf off 4;
  unsafe_set_le_int31 buf off x

let set_le_int32 buf off x =
  check buf off 4;
  unsafe_set_le_int32 buf off x

let set_le_int64 buf off x =
  check buf off 8;
  unsafe_set_le_int64 buf off x


(* Network byte order is big endian *)
let unsafe_get_net_int16 = unsafe_get_be_int16
let unsafe_set_net_int16 = unsafe_set_be_int16
let unsafe_get_net_uint16 = unsafe_get_be_uint16
let unsafe_set_net_uint16 = unsafe_set_be_uint16
let unsafe_get_net_int31 = unsafe_get_be_int31
let unsafe_set_net_int31 = unsafe_set_be_int31
let unsafe_get_net_int32 = unsafe_get_be_int32
let unsafe_set_net_int32 = unsafe_set_be_int32
let unsafe_get_net_int64 = unsafe_get_be_int64
let unsafe_set_net_int64 = unsafe_set_be_int64
let get_net_int16 = get_be_int16
let set_net_int16 = set_be_int16
let get_net_uint16 = get_be_uint16
let set_net_uint16 = set_be_uint16
let get_net_int31 = get_be_int31
let set_net_int31 = set_be_int31
let get_net_int32 = get_be_int32
let set_net_int32 = set_be_int32
let get_net_int64 = get_be_int64
let set_net_int64 = set_be_int64
