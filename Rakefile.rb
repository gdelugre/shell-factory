require 'rake'
require 'rake/clean'

require 'ipaddr'

INCLUDE_DIRS = %w{include include/sysdeps/generic include/ports}
CFLAGS = "-std=gnu11 -Os -fPIC -fno-common -fno-toplevel-reorder -fomit-frame-pointer -finline-functions -flto -nodefaultlibs -nostdlib #{INCLUDE_DIRS.map{|d| "-I#{d}"}.join(" ")}"

def build(target, *opts)
    default_opts = [ "CHANNEL", "HOST", "PORT" ]
    options = default_opts + opts
    defines = ENV.select{|e| options.include?(e)}.map{|k,v| 
        v = [ v.to_i ].pack('v').unpack('n')[0] if k == "PORT" 
        v = [ IPAddr.new(v).to_i ].pack('V').unpack('N')[0] if k == 'HOST'
        "-D#{k}=#{v}"
    }.join(' ')

    sh "gcc #{CFLAGS} shellcodes/#{target}.c -o bins/#{target}.elf #{defines}"
    sh "objcopy -O binary -j .text -j .rodata bins/#{target}.elf bins/#{target}.bin" 
end

task :readflag do
    build(:readflag, "FLAG_PATH")
end

task :execve do
    build(:execve, "SET_ARGV0")
end

task :memexec do 
    build(:memexec, "MEMORY")
end

CLEAN.include("bins/*.{elf,bin}")
