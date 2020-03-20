#!/bin/sh

# autogen.sh: Automatically generate the files necessary to build using
# autoconf.  This must be run on a fresh checkout from CVS and also if any
# autoconf files (configure.in, etc.) are updated.

# aclocal doesn't like it if you -I the system aclocal directory
SYS_ACLOCAL_DIR=`aclocal --print-ac-dir`
for i in build-aux /usr/local/share/aclocal /sw/share/aclocal; do
    if [ ! $i = $SYS_ACLOCAL_DIR -a -d $i ]; then
        ACLOCAL_OPTS="$ACLOCAL_OPTS -I $i"
    fi
done
aclocal $ACLOCAL_OPTS || exit 1

autoheader || exit 1
autoconf || exit 1

for i in libtoolize glibtoolize; do
    if which $i > /dev/null 2>&1; then
        LIBTOOLIZE=$i
    fi
done
$LIBTOOLIZE --automake || exit 1

automake --add-missing --include-deps --foreign || exit 1

autoreconf -f -i
