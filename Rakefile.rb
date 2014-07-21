require 'rake'
require 'rake/clean'

INCLUDE_DIRS = %w{include include/sysdeps/generic include/ports}
CFLAGS = "-std=gnu11 -Os -fPIC -fomit-frame-pointer -finline-functions -flto -nodefaultlibs -nostdlib #{INCLUDE_DIRS.map{|d| "-I#{d}"}.join(" ")}"

def build(target, *opts)
    default_opts = [ "CHANNEL" ]
    options = default_opts + opts
    defines = ENV.select{|e| options.include?(e)}.map{|k,v| "-D#{k}=#{v}"}.join(' ')

    sh "gcc #{CFLAGS} shellcodes/#{target}.c -o bins/#{target}.elf #{defines}"
    sh "objcopy -O binary -j .text -j .rodata bins/#{target}.elf bins/#{target}.bin" 
end

task :readflag do
    build(:readflag, "FLAG_PATH")
end

task :execve do
    build(:execve, "SET_ARGV0", "DUP_FD")
end

task :memexec do
    build(:memexec)
end

CLEAN.include("bins/*.{elf,bin}")
