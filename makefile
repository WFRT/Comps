q: img/process6.png img/process3.gif
	/usr/local/Cellar/ruby/1.9.3-p125/lib/ruby/gems/1.9.1/gems/jekyll-1.0.2/bin/jekyll build

img/process6.png: img/process6_raw.png
	convert img/process6_raw.png -trim img/process6.png
	
img/process3.gif: img/process3.png
	convert img/process3.png -trim img/process3.gif
