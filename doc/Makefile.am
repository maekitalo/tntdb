EXTRA_DIST = @PACKAGE@.doxygen.in \
	main.page \
	tntdb.markdown

dist_doc_DATA = \
	connect.md \
	customtypes.md \
	datetime.md \
	index.md \
	logging.md \
	modify.md \
	select.md \
	transaction.md

if MAKE_DOXYGEN

html_DATA = html/index.html

html/index.html: @PACKAGE@.doxygen
	@DOXYGEN@ @PACKAGE@.doxygen

install-data-hook:
	cp -pR html/* $(DESTDIR)$(htmldir)

endif
