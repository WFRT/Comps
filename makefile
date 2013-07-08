page: schemes/index.html
	jekyll build

schemes/index.html: compileSchemes.py
	python compileSchemes.py
