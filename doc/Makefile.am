EXTRA_DIST = @PACKAGE@.doxygen.in \
	main.page \
	tntdb.markdown

if MAKE_DOXYGEN

html_DATA = html/index.html

html/index.html: @PACKAGE@.doxygen
	@DOXYGEN@ @PACKAGE@.doxygen

install-data-hook:
	cp -pR html/* $(DESTDIR)$(htmldir)

endif
