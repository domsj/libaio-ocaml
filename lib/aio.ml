(* aio.ml: Linux async I/O bindings for libaio-ocaml
 * Copyright (C) 2009 Goswin von Brederlow
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
 *)

module Buffer = struct
  include Aio_buffer
end

type result =
    Result of Buffer.t
  | Errno of int
  | Partial of Buffer.t * int

let call_result cont buffer =
  cont (Result buffer)

let call_error cont err =
  cont (Errno err)

let call_partial cont buffer len =
  cont (Partial (buffer, len))

let _ = Callback.register "caml_aio_call_result" call_result
let _ = Callback.register "caml_aio_call_error" call_error
let _ = Callback.register "caml_aio_call_partial" call_partial

exception Error of int
exception Incomplete of Buffer.t * int

let result = function
    (* FIXME: convert unix errno to exception from Unix module *)
    Errno x -> raise (Error x)
  | Partial (buf, len) -> raise (Incomplete (buf, len))
  | Result buf -> buf


type context

external context: int -> context = "caml_aio_context"

external read : context -> Unix.file_descr -> int64 -> Buffer.t -> (result -> unit) -> unit = "caml_aio_read"
external read_multiple : context ->
                         (Unix.file_descr * int64 * Buffer.t * (result -> unit)) array ->
                         unit =
  "caml_aio_read_multiple"

external write : context -> Unix.file_descr -> int64 -> Buffer.t -> (result -> unit) -> unit = "caml_aio_write"

external poll : context -> Unix.file_descr -> int -> (Unix.file_descr -> unit) -> unit = "caml_aio_poll"
external run : context -> unit = "caml_aio_run"
external process : context -> unit = "caml_aio_process"

external fd : context -> Unix.file_descr = "caml_aio_fd"
external get_pending : context -> int = "caml_aio_get_pending"

external sync_read : Unix.file_descr -> int64 -> Buffer.t -> unit = "caml_aio_sync_read"
external sync_write : Unix.file_descr -> int64 -> Buffer.t -> unit = "caml_aio_sync_write"
