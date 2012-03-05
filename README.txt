...............................................................................
libaio-ocaml - Linux kernel AIO access library for ocaml
...............................................................................

libaio-ocaml are OCaml bindings enabling ocaml programs to use Linux
kernel asynchronous I/O system calls, important for the performance of
databases and other advanced applications.

libaio-ocaml is made of a single, independent, module and distributed under
the GNU Lesser General Public License.

Project home page : http://forge.ocamlcore.org/projects/libaio-ocaml/
Contact : goswin-v-b@web.de

...............................................................................

You can compile the example using:

ocamlopt -I /usr/lib/ocaml/aio -o test unix.cmxa bigarray.cmxa aio.cmxa examples/test.ml
