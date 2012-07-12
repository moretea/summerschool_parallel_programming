SACC=sac2c
SACC_FLAGS=-v1

# Note! Last character of the line below must be space! The clean target needs it to match.
BINS=hello_world  2_1_defining_arrays 2_2_arrays 

all: ${BINS}

clean:
	rm -rfv ${BINS} `echo '${BINS}' | ruby -e 'print STDIN.read.gsub(/ /,".c ")'`

%:	%.sac
	$(SACC) $(SACC_FLAGS) $< -o `echo $< | sed -e 's/\.sac//g'`

