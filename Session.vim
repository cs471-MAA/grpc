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
set stal=2
tabnew
tabrewind
edit .gitignore
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd _ | wincmd |
split
1wincmd k
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
exe 'vert 1resize ' . ((&columns * 28 + 119) / 238)
exe '2resize ' . ((&lines * 26 + 29) / 58)
exe 'vert 2resize ' . ((&columns * 209 + 119) / 238)
exe '3resize ' . ((&lines * 27 + 29) / 58)
exe 'vert 3resize ' . ((&columns * 209 + 119) / 238)
argglobal
enew
file NERD_tree_1
balt term://~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc//8825:/usr/bin/zsh
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
let s:l = 10026 - ((25 * winheight(0) + 13) / 26)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 10026
normal! 035|
wincmd w
argglobal
balt term://~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc//8825:/usr/bin/zsh
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
let s:l = 2 - ((1 * winheight(0) + 13) / 27)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 2
normal! 010|
wincmd w
2wincmd w
exe 'vert 1resize ' . ((&columns * 28 + 119) / 238)
exe '2resize ' . ((&lines * 26 + 29) / 58)
exe 'vert 2resize ' . ((&columns * 209 + 119) / 238)
exe '3resize ' . ((&lines * 27 + 29) / 58)
exe 'vert 3resize ' . ((&columns * 209 + 119) / 238)
tabnext
edit docker-compose.yml
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
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
exe 'vert 1resize ' . ((&columns * 31 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 206 + 119) / 238)
argglobal
enew
file NERD_tree_2
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
balt ~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/async-rpc-fbthrift/docker-compose.yml
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
let s:l = 3 - ((2 * winheight(0) + 27) / 54)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 3
normal! 018|
wincmd w
exe 'vert 1resize ' . ((&columns * 31 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 206 + 119) / 238)
tabnext 1
set stal=1
badd +5 Makefile
badd +10 benchmark.dockerfile
badd +0 grpc.dockerfile
badd +10054 term://~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc//8825:/usr/bin/zsh
badd +13 README.md
badd +85 common.cmake
badd +66 CMakeLists.txt
badd +3 .dockerignore
badd +0 .gitignore
badd +0 ~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/async-rpc-fbthrift/docker-compose.yml
badd +0 docker-compose.yml
badd +16 mockDatabase/mockDatabaseAsync.h
badd +1 mockDatabase/mockDatabaseAsync.cc
badd +58 mockDatabase/asyncFindLastMessageHandler.h
badd +19 shared/consts.h
badd +10 sanitizationService/sanitizationServiceAsync.cc
badd +1 ~/Desktop/EPFL/3_-_Advanced_multiprocessor_architecture/proj/grpc
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
