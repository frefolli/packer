BUILDDIR=./builddir

@all: ${BUILDDIR}/main.exe

${BUILDDIR}/main.exe: ./builddir
	ninja -C ${BUILDDIR}

${BUILDDIR}: meson.build
	meson setup builddir --reconfigure

clean:
	rm -rf builddir

test:
	meson test -C ${BUILDDIR}

run: ${BUILDDIR}/main.exe
	${BUILDDIR}/main.exe

install: ${BUILDDIR}/main.exe
	install -d $(DESTDIR)/usr/include
	install ./include/packer $(DESTDIR)/usr/include
	install -d $(DESTDIR)/usr/lib64
	install ${BUILDDIR}/libpacker.so $(DESTDIR)/usr/lib64
	install -d $(DESTDIR)/usr/bin
	install ${BUILDDIR}/main.exe $(DESTDIR)/usr/bin
	install -d $(DESTDIR)/usr/share
	install packer.pc $(DESTDIR)/usr/share
