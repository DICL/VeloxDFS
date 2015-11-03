all:
	-latex poster
	-bibtex poster
	-latex poster
	-dvips -j0 -Ppdf -Pdownload35 -G0 -t letter -D 1200 -Z -mode ljfzzz poster.dvi -o poster.ps
	-ps2pdf -sPAPERSIZE=a4 poster.ps

clean:
	rm poster.bbl
	rm poster.blg
	rm poster.aux
	rm poster.dvi
	rm poster.log
