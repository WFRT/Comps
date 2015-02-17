if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syntax spell toplevel
syntax case ignore
syntax sync linebreaks=1

syntax match nlAttr        / \S*=/he=e-1                    " attribute name. Highlight does not include = sign
syntax match nlAttr        / [a-zA-Z0-9]\+ /he=e-1          " boolean attribute. Attribute without value
syntax match nlAttr        / [a-zA-Z0-9]\+$/                " boolean attribute. Attribute without value at the end
syntax match nlTag         /^\S*/                           " tag

" Errors
syntax match nlError       /^\S*=\S*.*/                     " Starts with an atrribute
syntax match nlError       /^\S*\s\+.*[a-zA-Z0-9,]\+=$/     " 'atr=' at the end
syntax match nlError       /^\S*\s\+.*[a-zA-Z0-9,]\+= .*/   " 'atr= ' in the middle
syntax match nlError       /.* \([a-zA-Z0-9]*\)=.* \1=.*/   " Same attribute used twice

" Comments
syntax match nlComment     /^#.*/
syntax match nlComment       /#.*/                          " Inlined comment (i.e. '... #...')

syntax match nlOther      /=[a-zA-Z0-9]*\./hs=s+1,he=e-1
syntax match nlOther      /,[a-zA-Z0-9]*\./hs=s+1,he=e-1

hi link nlAttr Type
hi link nlTag  String
hi link nlComment  Comment
hi link nlError  Error
hi link nlOther  Keyword
