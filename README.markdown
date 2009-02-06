redshell
========

[![Shell: Original (1 of 3) (c) 2007 cobalt123, some rights reserved"](http://farm2.static.flickr.com/1046/1156232979_4e4d7c01c7_m_d.jpg)](http://flickr.com/photos/cobalt/1156232979/ "Shell: Original (1 of 3) (c) 2007 cobalt123, some rights reserved")

redshell is a simple and light UNIX shell.

Suggestions and patches are very welcome.

Features
--------

Redshell has most features one expects from a POSIX shell:

* globbing
* tilde (~) and environment variable expansion
* command expansion -- i.e., ``ldd `which gcc` ``
* <del>`timeout` -- run a command for at most *n* seconds</del> I'm reworking this one
* `echo` as a shell built-in
* `pwd` as a shell built-in
* `exit` as a shell built-in
* changing directory (`cd`)
* input and output redirection via ">" and "<"
* unlimited pipes
* background job execution
* `alias`es

<del>It's a very simple shell: there's no fancy stuff such as history or
tab-completion, but that might come later.</del>
I finally got around to implementing both of them. :)

Dependencies
------------

* glib-2.0
* GNU readline or libedit >= 2.11
* ragel >= 6.3

Building
--------

The preferred method is using SCons, the Makefile is usually lagging behind.

To build it, you need SCons and Python installed. Just run `scons` and you're
done!

redshell also comes with a Makefile. To compile just run `make`.

There's still no install option in either method, sorry.

Bugs
----

There certainly are some of them, hiding in the dark corners of the code when
you open the door. If you see one, shoot me an email with the GPS coordinates of
the bug or send me the remains of the bug you squashed.

License
-------

The MIT License

Copyright (c) 2008-2009 Ricardo Martins

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
