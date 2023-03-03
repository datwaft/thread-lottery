augroup local_readonly_options
  autocmd BufRead compile_commands.json set readonly nospell norelativenumber
  autocmd BufRead src/deps/*.c set readonly nospell norelativenumber
augroup END
