BASE := .
DOXYFILE := $(BASE)/Doxyfile
DOC := $(BASE)/html/
WEB := $(HOME)/code/www/output/libbf/api

doc: FORCE
	doxygen $(DOXYFILE)

www: doc
	rsync -az --delete $(DOC) $(WEB)

update:
	doxygen -u $(DOXYFILE)

FORCE:
