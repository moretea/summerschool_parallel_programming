#!/usr/bin/env ruby

# Clear
`rm -rf cuda/*`
`rm -rf seq/*`

Dir.chdir File.expand_path("../../cuda-filters", __FILE__) do
  system("make run")
end

Dir.chdir File.expand_path("../../seq-filters", __FILE__) do
  system("make run")
end


# Compare files

cuda = Dir["cuda/*"].sort
seq  = Dir["seq/*"].sort


base_cuda = cuda.collect { |x| File.basename x }
base_seq  = seq.collect { |x| File.basename x }

if  (base_cuda | base_seq) != (base_cuda && base_seq)
 puts "Not the same files!"
 exit
end


failed = []

# Now check for differences
Dir.chdir File.expand_path("../diff", __FILE__) do
  base_cuda.each do |f|
    cuda = "../cuda/#{f}"
    seq  = "../seq/#{f}"
    system("pwd && ../../cuda-filters/compare_images #{cuda} #{seq} 2>&1")

    unless $?.success?
      failed << f
    end
  end
end

puts
puts

if failed.any?
  puts "DIFFS!" + failed.inspect
else
  puts "Good to go, hurray!"
end

