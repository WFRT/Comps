q: img/process6.png img/process3.gif img/iconSmall.gif img/examples/0.jpg img/examples/36.jpg img/examples/225.jpg img/examples/nelev10x.jpg img/sochiSmall.jpg img/bchydroSmall.jpg
	/usr/local/Cellar/ruby/1.9.3-p125/lib/ruby/gems/1.9.1/gems/jekyll-1.0.2/bin/jekyll build

img/process6.png: img/process6_raw.png
	convert img/process6_raw.png -antialias -trim -resize 1050x274 img/process6.png
	
img/process3.gif: img/process3.png
	convert img/process3.png -trim img/process3.gif

img/iconSmall.gif: img/icon.png
	convert img/icon.png -resize 64x64 img/iconSmall.gif

img/sochiSmall.jpg: img/sochi2.jpg
	convert $< -resize 316x144 $@

img/bchydroSmall.jpg: img/bchydro2.jpg
	convert $< -resize 316x144 $@

img/examples/%.jpg: img/examples/%.png
	convert $< -resize 254x499 $@
