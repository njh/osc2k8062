osc2k8062
=========

OSC to DMX bridge server for the Velleman [K8062] kit or [VM116] pre-built module.
Based on [k8062forlinux] by Denis Moreaux.


Dependencies
------------

To build osc2k8062, you will need both [libusb] and [liblo].
Both are available is most operating systems.



Compiling from Git
------------------

To build and install osc2k8062 from Git sources, then run the following commands:

    ./autogen.sh
    ./configure
    make
    make install


Mac OS X
--------

Because the K8062 is an HID class device, the Mac OS X kernel will by default claim
the device and libusb will be unable to take control of it. The solution to this problem
is to install a dummy kernel extension, that leaves it available to userland processes.

A dummy kernel extension is available in the ```macosx``` directory. The kernel extension
can be build and installed using the ```install.sh``` shell script.

The dependencies, [liblo] and [libusb] are both available in [Homebrew].


License
-------

This software is licensed under the terms of the GPLv2 license.
This license was inherited from k8062forlinux.
See the file LICENSE for details.


Contact
-------

* Author:    Nicholas J Humfrey
* Email:     njh@aelius.com
* Twitter:   [@njh]
* Home Page: http://www.aelius.com/njh/


[k8062forlinux]:    http://k8062forlinux.sourceforge.net/
[K8062]:            http://www.velleman.eu/products/view/?country=gb&lang=en&id=353412
[VM116]:            http://www.velleman.eu/products/view/?country=gb&lang=en&id=354968
[liblo]:            http://liblo.sourceforge.net/
[libusb]:           http://libusb.info/
[Homebrew]:         http://brew.sh/
[@njh]:             http://twitter.com/njh
