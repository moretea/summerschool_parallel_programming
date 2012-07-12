SACC=sac2c
SACC_FLAGS=-v1

BINS=hello_world 

all: ${BINS}

clean:
	rm -rfv ${BINS} `echo '${BINS}' | ruby -e 'print STDIN.read.gsub(/ /,".c ")'`

%:	%.sac
	$(SACC) $(SACC_FLAGS) $< -o `echo $< | sed -e 's/\.sac//g'`

