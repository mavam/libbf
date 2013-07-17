REPO := git@github.com:mavam/libbf.git

doc: check
	doxygen Doxyfile

check: FORCE
	@test -d gh-pages/api \
		|| (git clone $(REPO) gh-pages && git checkout gh-pages)

commit: doc
	@cd gh-pages \
		&& git add api \
		&& git commit -a -e -m 'Update Doxygen API documentation.'

deploy: FORCE
	@cd gh-pages && git push origin HEAD

update:
	doxygen -u Doxyfile

FORCE:

.PHONY: doc check deploy push update FORCE
