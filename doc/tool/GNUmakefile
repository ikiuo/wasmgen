# GNUmakefile

-include GNUmakefile.config

HTML  = index full \
	wasmgen statement \
	pseudo-wasmgen pseudo-assembler pseudo-replace pseudo-section \
	instruction-representation instruction-table-column \
	instruction-const-getset instruction-load-store instruction-convert \
	instruction-arithmetic instruction-logical-shift instruction-compare \
	instruction-bitop instruction-selector instruction-stack \
	instruction-ref instruction-table-memory \
	instruction-branch instruction-compare-branch instruction-block

all:	build

build:
	@for n in $(HTML);\
	do\
	  CMD="python3 builder.py --html ../sxpml/html-$$n.sxpml ../html/$$n.html";\
	  echo $$CMD; $$CMD;\
	done

-include GNUmakefile.local
