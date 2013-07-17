REPO := git@github.com:mavam/libbf.git

deploy: doc
	@cd gh-pages && git commit -a -e -m 'Update Doxygen API documentation.'

doc: check
	doxygen Doxyfile

check: FORCE
	test -d gh-pages/api || (git clone $(REPO) gh-pages && git checkout gh-pages)

update:
	doxygen -u Doxyfile

FORCE:

.PHONY: doc check update FORCE
