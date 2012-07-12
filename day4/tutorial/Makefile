SACC=sac2c
SACC_FLAGS=-v1

BINS=$(shell ls *.sac | sed -e 's/\.sac//g') 

all: ${BINS} regenerate_gitignore

clean:
	rm -rfv ${BINS} `echo '${BINS}' | ruby -e 'print STDIN.read.gsub(/ /,".c ")'`

%:	%.sac
	$(SACC) $(SACC_FLAGS) $< -o `echo $< | sed -e 's/\.sac//g'`

regenerate_gitignore:
	@ruby -e '\
		File.open(".gitignore", "w") do |o|; \
	  	Dir["*.sac"].collect do |f|; \
				o.puts f.gsub(".sac", ""); \
				o.puts f.gsub(".sac", ".c"); \
				o.puts; \
			end; \
		end'
