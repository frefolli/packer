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
