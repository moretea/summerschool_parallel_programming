SACC=sac2c
SACC_FLAGS=-v1 -target cuda64 -DCUDA
OPTIMAZATIONS= 
INPUT_IMAGE=small.ppm
#-docoal -doshr -doexpar -dolao

BINS=$(shell ls *.sac | sed -e 's/\.sac//g') 

all: ${BINS} regenerate_gitignore

clean:
	rm -rfv ${BINS} `echo '${BINS}' | ruby -e 'print STDIN.read.gsub(/ /,".c ")'`

%:	%.sac
	$(SACC) $(SACC_FLAGS) $< -o `echo $< | sed -e 's/\.sac//g'` $(OPTIMAZATIONS)

regenerate_gitignore:
	@ruby -e '\
		File.open(".gitignore", "w") do |o|; \
			o.puts "out.ppm"; \
	  	Dir["*.sac"].sort.collect do |f|; \
				o.puts f.gsub(".sac", ""); \
				o.puts f.gsub(".sac", ".c"); \
				o.puts f.gsub(".sac", ".cu"); \
				o.puts; \
			end; \
		end'

rotate90: all
	prun -v -np 1 -native '-l gpu=GTX480' ./filters -f rotate90 -i $(INPUT_IMAGE) -o out.ppm
	display out.ppm
