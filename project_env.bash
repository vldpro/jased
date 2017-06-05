#!/bin/bash

PROJECT_HOME="$HOME/yadisk/univer/projects/jased"

home() { cd -- "$PROJECT_HOME"; }

build() { home && make; }

run() { home && ./bin/jased "$*"; }

include() { cd -- "$PROJECT_HOME"/include; }

src() { cd -- "$PROJECT_HOME"/src; }

lines_count() { find "$PROJECT_HOME" -name "*.[ch]" | xargs wc -l | sort -nr; }
