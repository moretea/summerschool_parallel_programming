###
### Build settings
###

BENCHMARKS = progA progB progC
DATAFILES = $(wildcard *.d*[0-9]) $(wildcard *.inputs)
PROGRUN = bash ./progrun.sh

srcdir ?= .

all: check bench

clean:
	rm -f $(BENCHMARKS:=.tlist) $(BENCHMARKS:=.ilist) $(BENCHMARKS:=.blist)
	rm -rf benchdata

v_at = $(v_at_$(V))
v_at_ = $(v_at_0)
v_at_0 = @
v_gen = $(v_gen_$(V))
v_gen_ = $(v_gen_0)
v_gen_0 = @echo '  GEN  $@';


%.ilist: $(DATAFILES)
	$(v_at)rm -f $@ $@.tmp1 $@.tmp2 $@.tmp3 
	$(v_gen)set -e;							\
	  ifile="$*.inputs"; 						\
	  if test -r "$(srcdir)/$$ifile"; then				\
	    { while read pat; do					\
	       for f in "$(srcdir)"/$$pat ./$$pat; do			\
	         if test -r "$$f"; then echo "$$f"; fi;			\
	       done;							\
	      done; } <"$$ifile";					\
	  else								\
	    for f in "$(srcdir)/$*".d*[0-9] ./"$*".d*[0-9]; do		\
	      if test -r "$$f"; then echo "$$f"; fi;			\
	    done;							\
	  fi >"$@.tmp1" && 						\
	  for i in `cat "$@.tmp1"`; do 					\
	    suff=; if grep -q 'USE IN MAKE CHECK' "$$i" >/dev/null 2>&1; then suff=.check; fi; \
	    echo "$$i$$suff"; 						\
	  done >"$@.tmp2"
	$(v_at)sed -e 's|^$(srcdir)/||g;s|^./||g' <"$@.tmp2" | sort | uniq >"$@.tmp3"
	$(v_at)mv -f "$@.tmp3" "$@"
	$(v_at)rm -f "$@.tmp1" "$@.tmp2"

%.tlist: %.ilist
	$(v_at)rm -f "$@"
	$(v_gen)grep .check <"$<" | sed -e 's/.check//g' >"$@.tmp"
	$(v_at)mv -f "$@".tmp "$@"

%.blist: %.ilist
	$(v_at)rm -f "$@"
	$(v_gen)sed -e 's/.check//g' <"$<" >"$@.tmp"
	$(v_at)mv -f "$@.tmp" "$@"

.PRECIOUS: $(BENCHMARKS:=.tlist) $(BENCHMARKS:=.blist) $(BENCHMARKS:=.ilist)

.SECONDEXPANSION:

##
## Benchmarks
##

# Format of output files: benchdata/PROG/INPUT/PROFILE.out

DORUN_DEF = do_run() {                   \
	  set -e;                        \
	  rm -f "$$1" "$$1".err;         \
	  set +e; $(PROGRUN) "$$2" "$$3" "$$4" >"$$1".err 2>&1; ecode=$$?; set -e; \
	  if test $$ecode != 0; then     \
	    if test "$$4" = check; then  \
	      { echo "Exit status: $$ecode"; echo; echo "Error log:"; echo; sed -e 's/^/  /g' <"$$1".err; } >&2; \
	      exit $$ecode;              \
	    else                         \
	      echo "  FAIL $@" >&2;      \
	    fi;                          \
	  else                           \
	    mv -f "$$1".err "$$1";       \
	  fi;                            \
	}

%.out: 	$$(word 2,$$(subst /, ,$$@)) $$(word 3,$$(subst /, ,$$@))
	$(v_at)mkdir -p "$(dir $@)"
	$(v_gen)$(DORUN_DEF); do_run "$@" "$(word 2,$(subst /, ,$@))" "$(word 3,$(subst /, ,$@))" "$(basename $(notdir $@))"

##
## Unit testing
##

%.check:
	$(v_at)$(MAKE) "$*.tlist"
	$(v_at)$(MAKE) $$(for I in $$(cat "$*.tlist"); do echo "benchdata/$*/$$I/check.out"; done)

check:
	$(v_at)$(MAKE) $(BENCHMARKS:=.check)

##
## Automated benchmarking
##

BENCH_PROFILES ?= hello

%.bench:
	$(v_at)$(MAKE) "$*.blist"
	$(v_at)$(MAKE) $(foreach P,$(BENCH_PROFILES), \
	          $$(for I in $$(cat "$*.blist"); do echo "benchdata/$*/$$I/$(P).out"; done))

bench:
	$(v_at)$(MAKE) $(BENCHMARKS:=.bench)
