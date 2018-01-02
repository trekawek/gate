#!/usr/bin/env ruby -w

require 'bindata'

MULTIPLIER = 16
RATE = 32_000

start = false

previous_i = true
count = 0
data = []

File.open('output.raw', 'r') do |io|
  until io.eof? do
    i = BinData::Int16le.read(io) / 32768.to_f
    start = true if i < -0.10
    next unless start
    i = i < -0.10
    
    if previous_i == i
      count = count + 1
    else
      count = (count / MULTIPLIER.to_f).round
      data << count * MULTIPLIER * 1_000_000 / RATE
      if count > 10
        puts data.inspect
        data = []
      end

      previous_i = i
      count = 1
    end
  end
end

puts "read"
