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
let NERDTreeIgnore = ['Makefile.in', '^configure$', 'aclocal.m4', 'autom4te.cache', 'm4', 'config.h.in']

let g:clang_user_options = '-std=c++14'
let g:ctrlp_custom_ignore = '\v[\/](git|html)$'
