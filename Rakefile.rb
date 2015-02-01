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

OUTPUT_DIR = "bins"
INCLUDE_DIRS = %w{include include/sysdeps/generic include/ports}
CFLAGS = %w{-std=gnu11
            -Wall
            -Os
            -fno-common
            -fno-toplevel-reorder
            -fomit-frame-pointer
            -finline-functions
            -nodefaultlibs -nostdlib
            -Wl,-e_start
         }

# Architecture-dependent flags.
ARCH_CFLAGS =
{
    /mips/ => %w{-mshared -mno-abicalls},
    /.*/ => %w{-fPIC}
}

def compile(target, toolchain, output_dir, *opts)
    common_opts = %w{CHANNEL HOST PORT NO_BUILTIN FORK_ON_ACCEPT REUSE_ADDR}
    options = common_opts + opts
    defines = ENV.select{|e| options.include?(e)}
    options = common_opts + opts
    cflags = CFLAGS.dup

    ARCH_CFLAGS.each_pair { |arch, flags|
        if toolchain =~ arch
            cflags += flags
            break
        end
    }

    if ENV['CFLAGS']
        cflags += ENV['CFLAGS'].split
    end
    
    if defines['NO_BUILTIN'] and defines['NO_BUILTIN'].to_i == 1
        cflags << "-fno-builtin"
    end

    if ENV['OUTPUT_DEBUG'] and ENV['OUTPUT_DEBUG'].to_i == 1
        sh "#{toolchain}gcc -S #{cflags.join(" ")} shellcodes/#{target}.c -o #{output_dir}/#{target}.S #{defines}"
    end

    if ENV['OUTPUT_LIB'] and ENV['OUTPUT_LIB'].to_i == 1
        cflags << '-shared'
    end

    cflags += INCLUDE_DIRS.map{|d| "-I#{d}"}
    defines = defines.map{|k,v|
        v = IPAddr.new(v).to_define if k == 'HOST'
        "-D#{k}=#{v}"
    }

    sh "#{toolchain}gcc #{cflags.join(' ')} shellcodes/#{target}.c -o #{output_dir}/#{target}.elf #{defines.join(' ')}"
end

def generate_shellcode(target, toolchain, output_dir)
    sh "#{toolchain}objcopy -O binary -j .text -j .funcs -j .rodata bins/#{target}.elf #{output_dir}/#{target}.bin" 
end

def build(target, *opts)
    output_dir = OUTPUT_DIR
    toolchain = ''

    if ENV['CROSS_COMPILE']
        toolchain = ENV['CROSS_COMPILE'] + '-'
    end

    compile(target, toolchain, output_dir, *opts)
    generate_shellcode(target, toolchain, output_dir)
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
