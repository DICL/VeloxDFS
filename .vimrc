"Indentation
set shiftwidth=2
set expandtab
set tabstop=2
set backspace=2
set foldmethod=marker
set cino=N-s
match Error /{{{\|}}}/
set smartindent

set makeprg=cd\ ~/build;\ make\ -j8\ check
