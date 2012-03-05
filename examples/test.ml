let read_done result =
  let buffer = Aio.result result in
  let i64 = Aio.Buffer.get_int64 buffer 0 in
  let i32 = Aio.Buffer.get_int32 buffer 8 in
  let i31 = Aio.Buffer.get_int31 buffer 12 in
  let i16 = Aio.Buffer.get_int16 buffer 16 in
  let u16 = Aio.Buffer.get_uint16 buffer 18 in
  let i8 = Aio.Buffer.get_int8 buffer 20 in
  let u8 = Aio.Buffer.get_uint8 buffer 21
  in
    Printf.printf " int64 = %Lx\n" i64;
    Printf.printf " int32 = %lx\n" i32;
    Printf.printf " int31 = %x\n" i31;
    Printf.printf " int16 = %d\n" i16;
    Printf.printf "uint16 = %d\n" u16;
    Printf.printf " int8  = %d\n" i8;
    Printf.printf "uint8  = %d\n" u8;
    flush_all ()

let write_done ctx fd result =
  let buffer = Aio.result result
  in
    Printf.printf "write done\n";
    flush_all ();
    Aio.Buffer.clear buffer;
    Printf.printf "reading\n";
    flush_all ();
    Aio.read ctx fd Int64.zero buffer read_done

let _ = at_exit Gc.full_major

let _ =
  let fd = Unix.openfile "testfile" [Unix.O_RDWR; Unix.O_CREAT] 0o664 in
  let buffer = Aio.Buffer.create (Aio.Buffer.page_size ()) in
  let ctx = Aio.context 32
  in
    Aio.Buffer.clear buffer;
    Aio.Buffer.set_int64 buffer 0 0x123456789abcdefL;
    Aio.Buffer.set_int32 buffer 8 0x12345678l;
    Aio.Buffer.set_int31 buffer 12 0x12345678;
    Aio.Buffer.set_int16 buffer 16 (-12345);
    Aio.Buffer.set_uint16 buffer 18 12345;
    Aio.Buffer.set_int8 buffer 20 (-23);
    Aio.Buffer.set_uint8 buffer 21 42;

    Printf.printf "writing\n";
    flush_all ();
    Aio.write ctx fd Int64.zero buffer (write_done ctx fd);
    Aio.run ctx;
    (* Create error of pending IO on exit *)
    Aio.read ctx fd Int64.zero buffer read_done
