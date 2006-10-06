EXTRA_DIST = @PACKAGE@.doxygen.in \
	tntdb.odt \
	tntdb.pdf

if MAKE_DOXYGEN

htmldir = $(datadir)/@PACKAGE@/html

html_DATA = html/index.html

html/index.html: @PACKAGE@.doxygen
	@DOXYGEN@ @PACKAGE@.doxygen

install-data-hook:
	cp -dpR html/* $(DESTDIR)$(htmldir)

endif
