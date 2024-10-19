BUILDDIR=./builddir

@all: ${BUILDDIR}/packer.exe

${BUILDDIR}/packer.exe: ./builddir
	ninja -C ${BUILDDIR}

${BUILDDIR}: meson.build
	meson setup builddir --reconfigure

clean:
	rm -rf builddir

test:
	meson test -C ${BUILDDIR}

install:
	install -d ${DESTDIR}/usr/lib
	install ./builddir/libpacker.so ${DESTDIR}/usr/lib
	install -d ${DESTDIR}/usr/bin
	install ./builddir/packer.exe ${DESTDIR}/usr/bin/packer
	install -d ${DESTDIR}/usr/include/packer
	install ./include/packer/*.hh ${DESTDIR}/usr/include/packer
