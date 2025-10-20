#!/usr/bin/env zsh
dirs=(LiteBox LiteEngine)
setopt extended_glob
setopt globstarshort
for dir in $dirs; do
    for file in $dir/**/*.(cpp|cc|cxx|h|hpp|hh); do
    if [[ -f "$file" ]]; then
        echo "Formatting $file"
        clang-format -i -style=file "$file"
    fi
    done
done