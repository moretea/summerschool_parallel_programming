puts `rm -vrf *.ppm`
puts `killall -9 python`

WORKERS = (1..8).to_a
SIZES   = [ [5,5], [32,10], [32,100], [320,200] ]

WORKERS.each do |worker|
  SIZES.each do |size|
    height, width = size
    block_name = "#{height}x#{width}"

    log_file = "logs/sink_#{worker}_s#{block_name}.log"

    puts "Logging to #{log_file}"

    if File.exists?(log_file)
      puts "SKIPPING WORKERS: #{worker}, BLOCK SIZE: #{size}"
      next
    else
      puts "RUNNING  WORKERS: #{worker}, BLOCK SIZE: #{size}"
    end

    p_vent = fork do
      STDOUT.reopen(File.open("/dev/null","w"))
      STDERR.reopen(File.open("/dev/null","w"))
      exec("python ventilator.py 320 200 #{height} #{width}")
    end

    p_sink = fork do
      STDOUT.reopen(File.open(log_file,"w"))
      exec("python sink.py localhost");
    end

    p_workers = fork do
      STDOUT.reopen(File.open("/dev/null","w"))
      STDERR.reopen(File.open("/dev/null","w"))
      exec("python worker.py localhost localhost")
    end

    puts "Done. Waiting for death"
    Process.wait
    Process.wait
    Process.wait
  end
end
