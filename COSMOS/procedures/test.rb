puts "1"
puts "2"
puts "3"
puts "4"
puts "5"
puts "6"
puts "7"
puts "8"
puts "9"
puts "10"

play_wav_file(File.join(Cosmos::USERPATH,'config','data','tada.wav'))
value = ask('Enter the value')

cmd("INST CLEAR")

5.times do |index|
  puts index
end

if false
  puts false
else
  puts true
end

wait_expression('false', 10)
wait(10)

a = [10,
11,
12,
13,
14,
15]

{ :X1 => 1,
  :X2 => 2
}.each {|x, y| puts x}


puts "brown cow"
yep
puts "howdy"

if (yep)
  puts "uh oh"
end

puts "done"

