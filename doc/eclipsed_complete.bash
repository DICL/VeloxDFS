#!/bin/bash

function _eclipsed() 
{
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts="launch status close submit kill put get rm cat ls config cc clear" 
    dashopts="--help --version" 
  
    if ((COMP_CWORD == 1)); then
      if [[ ${cur} == -* ]]; then
        COMPREPLY=( $(compgen -W "${dashopts}" -- ${cur}) )
      else
        COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )
        return 0
      fi
    fi
}
complete -F _eclipsed -o default eclipsed 
