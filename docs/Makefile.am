dist_man_MANS = dvdstyler.1

LINGUAS=ca de el en es eu fi fr hu it ja nb pl pt pt_BR ro ru si uk zh_CN zh_TW
docdir=@datadir@/@PACKAGE@/docs
doc_DATA = $(LINGUAS:%=help_%.zip)

help_%.zip: DVDStylerGuide_%.xml
	t=`mktemp -d tempXXXXXXX`; \
	(cd "$$t" && $(XMLTO) --skip-validation htmlhelp ../DVDStylerGuide_$*.xml && rm -f *.hh* && $(ZIP) ../help_$*.zip *.html); \
	rm -f "$$t"/*.html; \
	rm -f "$$t"/*.proc; \
	if [ -d "images/$*" ]; then \
		$(ZIP) help_$*.zip htmlhelp.hhp toc.hhc images/$*/*.png images/$*/*.jpg; \
	else \
		$(ZIP) help_$*.zip htmlhelp.hhp toc.hhc images/en/*.png images/en/*.jpg; \
	fi; \
	rmdir "$$t"

html: $(LINGUAS:%=html_%)

html_%: DVDStylerGuide_%.xml
	rm -rf html_$*; \
	mkdir html_$*; \
	(cd html_$* && $(XMLTO) --skip-validation htmlhelp ../DVDStylerGuide_$*.xml && rm -f *.hh*); \
	mkdir html_$*/images; \
	if [ -d "images/$*" ]; then \
		mkdir html_$*/images/$*; \
		cp images/$*/*.png images/$*/*.jpg html_$*/images/$*/; \
	else \
		mkdir html_$*/images/en; \
		cp images/en/*.png images/en/*.jpg html_$*/images/en/; \
	fi;
