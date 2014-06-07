
if HAVE_DOXYGEN

directory = $(srcdir)/../doc/html

#dist_pkgdata_DATA = $(directory)
#dist_pkgdata_DATA = $(shell find $(directory) -type f)
$(directory): doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) doxygen.cfg
	echo Timestamp > doxyfile.stamp


install-data-local:
	$(MKDIR_P) $(DESTDIR)/$(docdir)/html/search
	$(INSTALL_DATA) html/*.html $(DESTDIR)/$(docdir)/html
	$(INSTALL_DATA) html/*.png $(DESTDIR)/$(docdir)/html
if HAVE_DOT
	$(INSTALL_DATA) html/*.md5 $(DESTDIR)/$(docdir)/html
endif
	$(INSTALL_DATA) html/*.css $(DESTDIR)/$(docdir)/html
	$(INSTALL_DATA) html/*.js $(DESTDIR)/$(docdir)/html


CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
	rm -rf $(top_srcdir)/doc/html

endif
