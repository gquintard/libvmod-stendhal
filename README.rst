----------------------
Varnish Stendhal Module
----------------------

DESCRIPTION
===========

``vmod_stendhal`` offers a director to store backends (and other directors)
inside a red-black tree, allowing to retrieved them using a string.

An basic example could be::

 import stendhal;

 backend foo_be { .host = "127.0.0.1" }
 backend bar_be { .host = "127.0.0.2" }

 sub vcl_init {
   # create the director
   new sd = stendhal.director();

   # add the backends
   sd.add_backend("foo.example.com", foo_be);
   sd.add_backend("bar.example.com", bar_be);
 }

 # use it!
 sub vcl_recv {
   if (!sd.contains(req.http.host)) {
     return(synth(404));
   } else {
     set req.backend_hint = sd.backend(req.http.host);
   }
 }

INSTALLATION
============

The source tree is based on autotools to configure the building, and
does also have the necessary bits in place to do functional unit tests
using the ``varnishtest`` tool.

Building requires the Varnish header files and uses pkg-config to find
the necessary paths.

Usage::

 ./autogen.sh
 ./configure

If you have installed Varnish to a non-standard directory, call
``autogen.sh`` and ``configure`` with ``PKG_CONFIG_PATH`` pointing to
the appropriate path. For instance, when varnishd configure was called
with ``--prefix=$PREFIX``, use

 PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
 export PKG_CONFIG_PATH

Make targets:

* make - builds the vmod.
* make install - installs your vmod.
* make check - runs the unit tests in ``src/tests/*.vtc``
* make distcheck - run check and prepare a tarball of the vmod.


Installation directories
------------------------

By default, the vmod ``configure`` script installs the built vmod in
the same directory as Varnish, determined via ``pkg-config(1)``. The
vmod installation directory can be overridden by passing the
``VMOD_DIR`` variable to ``configure``.

Other files like man-pages and documentation are installed in the
locations determined by ``configure``, which inherits its default
``--prefix`` setting from Varnish.


COMMON PROBLEMS
===============

* configure: error: Need varnish.m4 -- see README.rst

  Check if ``PKG_CONFIG_PATH`` has been set correctly before calling
  ``autogen.sh`` and ``configure``
