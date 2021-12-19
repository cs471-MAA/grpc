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
wincmd _ | wincmd |
vsplit
2wincmd h
wincmd w
wincmd _ | wincmd |
split
wincmd _ | wincmd |
split
2wincmd k
wincmd w
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
exe 'vert 1resize ' . ((&columns * 28 + 113) / 226)
exe '2resize ' . ((&lines * 16 + 26) / 52)
exe 'vert 2resize ' . ((&columns * 98 + 113) / 226)
exe '3resize ' . ((&lines * 16 + 26) / 52)
exe 'vert 3resize ' . ((&columns * 98 + 113) / 226)
exe '4resize ' . ((&lines * 15 + 26) / 52)
exe 'vert 4resize ' . ((&columns * 98 + 113) / 226)
exe 'vert 5resize ' . ((&columns * 98 + 113) / 226)
argglobal
enew
file NERD_tree_1
balt benchmark.dockerfile
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
let s:l = 5 - ((4 * winheight(0) + 8) / 16)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 5
normal! 0
wincmd w
argglobal
if bufexists("benchmark.dockerfile") | buffer benchmark.dockerfile | else | edit benchmark.dockerfile | endif
if &buftype ==# 'terminal'
  silent file benchmark.dockerfile
endif
balt grpc.dockerfile
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
let s:l = 1 - ((0 * winheight(0) + 8) / 16)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 023|
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
let s:l = 266 - ((14 * winheight(0) + 7) / 15)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 266
normal! 029|
wincmd w
argglobal
if bufexists("README.md") | buffer README.md | else | edit README.md | endif
if &buftype ==# 'terminal'
  silent file README.md
endif
balt benchmark.dockerfile
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
let s:l = 10 - ((9 * winheight(0) + 24) / 49)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 10
normal! 02|
wincmd w
4wincmd w
exe 'vert 1resize ' . ((&columns * 28 + 113) / 226)
exe '2resize ' . ((&lines * 16 + 26) / 52)
exe 'vert 2resize ' . ((&columns * 98 + 113) / 226)
exe '3resize ' . ((&lines * 16 + 26) / 52)
exe 'vert 3resize ' . ((&columns * 98 + 113) / 226)
exe '4resize ' . ((&lines * 15 + 26) / 52)
exe 'vert 4resize ' . ((&columns * 98 + 113) / 226)
exe 'vert 5resize ' . ((&columns * 98 + 113) / 226)
tabnext 1
badd +10 Makefile
badd +3 benchmark.dockerfile
badd +18 grpc.dockerfile
badd +1 term://~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc//8825:/usr/bin/zsh
badd +0 README.md
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
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
