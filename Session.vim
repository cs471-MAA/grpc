let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
argglobal
%argdel
edit Makefile
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd _ | wincmd |
split
wincmd _ | wincmd |
split
2wincmd k
wincmd w
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 21 + 119) / 239)
exe '2resize ' . ((&lines * 8 + 29) / 59)
exe 'vert 2resize ' . ((&columns * 217 + 119) / 239)
exe '3resize ' . ((&lines * 18 + 29) / 59)
exe 'vert 3resize ' . ((&columns * 217 + 119) / 239)
exe '4resize ' . ((&lines * 28 + 29) / 59)
exe 'vert 4resize ' . ((&columns * 217 + 119) / 239)
argglobal
enew
file NERD_tree_1
balt Makefile
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal nofen
wincmd w
argglobal
balt Makefile
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal nofen
silent! normal! zE
let &fdl = &fdl
let s:l = 6 - ((5 * winheight(0) + 4) / 8)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 6
normal! 065|
wincmd w
argglobal
if bufexists("grpc.dockerfile") | buffer grpc.dockerfile | else | edit grpc.dockerfile | endif
if &buftype ==# 'terminal'
  silent file grpc.dockerfile
endif
balt Makefile
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal nofen
silent! normal! zE
let &fdl = &fdl
let s:l = 20 - ((12 * winheight(0) + 9) / 18)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 20
normal! 013|
wincmd w
argglobal
if bufexists("term://~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc//8825:/usr/bin/zsh") | buffer term://~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc//8825:/usr/bin/zsh | else | edit term://~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc//8825:/usr/bin/zsh | endif
if &buftype ==# 'terminal'
  silent file term://~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc//8825:/usr/bin/zsh
endif
balt Makefile
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal nofen
let s:l = 10000 - ((8 * winheight(0) + 14) / 28)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 10000
normal! 072|
wincmd w
4wincmd w
exe 'vert 1resize ' . ((&columns * 21 + 119) / 239)
exe '2resize ' . ((&lines * 8 + 29) / 59)
exe 'vert 2resize ' . ((&columns * 217 + 119) / 239)
exe '3resize ' . ((&lines * 18 + 29) / 59)
exe 'vert 3resize ' . ((&columns * 217 + 119) / 239)
exe '4resize ' . ((&lines * 28 + 29) / 59)
exe 'vert 4resize ' . ((&columns * 217 + 119) / 239)
tabnext 1
badd +6 Makefile
badd +0 term://~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc//8825:/usr/bin/zsh
badd +12 grpc.dockerfile
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 shortmess=filnxtToOFc
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
