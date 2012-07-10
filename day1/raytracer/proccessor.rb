table = {}
Dir["logs/*"].each do |file|
  p file
  file = File.basename(file)
  junk, workers, block_size = file.split("_")

  size = block_size.gsub("s","").split(".").first

  puts "meh"
  time = `tail -n 2 logs/#{file}`.split("\n").first.chomp.split(" ")[3]

  table[ [workers, size]] = time
end

p table
