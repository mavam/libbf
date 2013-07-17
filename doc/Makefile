REPO := git@github.com:mavam/libbf.git

doc: check
	doxygen Doxyfile

check: FORCE
	test -d gh-pages/api || (git clone $(REPO) gh-pages && git checkout gh-pages)

deploy: doc
	@cd gh-pages && git commit -a -e -m 'Update Doxygen API documentation.'

update:
	doxygen -u Doxyfile

FORCE:

.PHONY: doc check deploy update FORCE
