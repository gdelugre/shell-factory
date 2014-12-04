require 'rake'
require 'rake/clean'

require 'ipaddr'

class IPAddr
    def to_define
        if self.ipv4?
            "\\{#{self.to_string.gsub(".",",")}\\}"
        else
            "\\{#{
            self.to_string.gsub(":","")
                .split(/(..)/)
                .delete_if{|x| x.empty?}
                .map{|d| "0x" + d}
                .join(',')
            }\\}"
        end
    end
end

INCLUDE_DIRS = %w{include include/sysdeps/generic include/ports}
CFLAGS = %w{-std=gnu11
            -Wall
            -Os
            -fPIC
            -fno-common
            -fno-toplevel-reorder
            -fomit-frame-pointer
            -finline-functions
            -nodefaultlibs -nostdlib
         }

def build(target, *opts)
    common_opts = [ "CHANNEL", "HOST", "PORT", "NO_BUILTIN" ]
    options = common_opts + opts
    defines = ENV.select{|e| options.include?(e)}
    cflags = CFLAGS.dup

    if defines['NO_BUILTIN'] and defines['NO_BUILTIN'].to_i == 1
        cflags << "-fno-builtin"
    end

    if ENV['OUTPUT_DEBUG'] and ENV['OUTPUT_DEBUG'].to_i == 1
        sh "gcc -S #{cflags.join(" ")} shellcodes/#{target}.c -o bins/#{target}.S #{defines}"
    end

    cflags += INCLUDE_DIRS.map{|d| "-I#{d}"}
    defines = defines.map{|k,v|
        v = IPAddr.new(v).to_define if k == 'HOST'
        "-D#{k}=#{v}"
    }

    sh "gcc #{cflags.join(' ')} shellcodes/#{target}.c -o bins/#{target}.elf #{defines.join(' ')}"
    sh "objcopy -O binary -j .text -j .funcs -j .rodata bins/#{target}.elf bins/#{target}.bin" 
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

rule '' do |task|
    build(task.name)
end

CLEAN.include("bins/*.{elf,bin}")
