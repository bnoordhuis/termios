termios
=======

A utility for dumping the contents of `struct termios` for a file descriptor or
character device.

build
=====

Easy, just run `make`. :-)

usage
=====

Basic usage, defaults to probing stdin:

    $ termios
    c_iflag         BRKINT|ICRNL|IMAXBEL|IXANY|IXON
    c_oflag         ONLCR|OPOST
    c_cflag         CREAD|CSIZE|HUPCL
    c_lflag         ECHO|ECHOCTL|ECHOE|ECHOK|ECHOKE|ICANON|IEXTEN|ISIG|PENDIN
    c_cc            4,255,255,127,23,21,18,255,3,28,26,25,17,19,22,15,1,0,20,255
    c_ispeed        9600
    c_ospeed        9600

To probe a file descriptor, in this case stderr:

    $ termios 2
    # <output elided for brevity>

To probe a character device:

    $ termios /dev/ttyS0
    # <output elided for brevity>
