require 'rake'
require 'rake/clean'

require 'ipaddr'

class String
    COLORS = {
        red: 31, green: 32, brown: 33, blue: 34,  magenta: 35, cyan: 36, gray: 37,
    }

    def color(c)
        colors = {
        }
        "\e[#{COLORS[c]}m#{self}\e[0m"
    end

    def bold
        "\e[1m#{self}\e[21m"
    end
end

class IPAddr
    def to_define
        if self.ipv4?
            "#{self.to_string.gsub(".",",")}"
        else
            "#{
            self.to_string.gsub(":","")
                .split(/(..)/)
                .delete_if{|x| x.empty?}
                .map{|d| "0x" + d}
                .join(',')
            }"
        end
    end
end

CC = "g++"
OUTPUT_DIR = "bins"
SHELLCODE_DIR = "shellcodes"
INCLUDE_DIRS = %w{include}
CFLAGS = %w{-std=c++1y
            -Wall
            -Wextra
            -Wfatal-errors
            -fno-common
            -fomit-frame-pointer
            -nostdlib
            -Wl,-e_start
            -Wl,--gc-sections
         }

COMPILER_CFLAGS =
{
    /^g\+\+$/ =>
        %w{-fno-toplevel-reorder
           -finline-functions
           -nodefaultlibs
           -Os
           },

    /^clang\+\+$/ =>
        %w{-Oz
           -Wno-invalid-noreturn
          }
}

# Architecture-dependent flags.
ARCH_CFLAGS =
{
    /mips/ => %w{-mshared -mno-abicalls -mno-plt -mno-gpopt -mno-long-calls -G 0},
    /.*/ => %w{-fPIC}
}

def show_info(str)
    puts "[".bold + "*".bold.color(:green) + "] ".bold + str
end

def show_error(str)
    puts "[".bold + "*".bold.color(:red) + "] ".bold + 'Error: '.color(:cyan) + str
    abort
end

def cc_invoke(cc, triple)
    return cc if triple.empty?

    case cc
    when 'g++'
        "#{triple}-#{cc}"

    when 'clang++'
        "#{cc} -target #{triple} --sysroot /usr/#{triple}/"
    end
end

# Returns [ source_path, output_basename ]
def target_to_source(target)
    dirs = target.to_s.split('/').delete_if{|dir| dir.empty?}

    if dirs.length == 1
        source_dir = SHELLCODE_DIR
        source_basename = dirs[0]
    else
        source_dir = dirs[0..-2].join('/')
        source_basename = dirs[-1]
    end

    [ source_dir, source_basename ]
end

def compile(target, triple, output_dir, *opts)
    common_opts = %w{CHANNEL HOST PORT NO_BUILTIN FORK_ON_ACCEPT REUSE_ADDR RELAX_INLINE}
    options = common_opts + opts
    defines = ENV.select{|e| options.include?(e)}
    options = common_opts + opts
    cc = ENV['CC'] || CC
    cflags = CFLAGS.dup
    source_dir, target_name = target_to_source(target)
    source_file = File.join(source_dir, "#{target_name}.cc")

    unless File.exists?(source_file)
        show_error("Cannot find source for target '#{target.to_s.color(:red)}'.")
    end

    host_os = RbConfig::CONFIG['target_os']
    host_arch = RbConfig::CONFIG['target_cpu']
    host_vendor = RbConfig::CONFIG['target_vendor']
    host_triple = [ host_arch, host_vendor, host_os ].join('-')

    show_info("#{'Generating target'.color(:cyan)} '#{target.to_s.color(:red)}'")
    puts "    #{'├'.bold} #{'Compiler:'.color(:green)} #{cc}"
    puts "    #{'├'.bold} #{'Host architecture:'.color(:green)} #{host_triple}"
    puts "    #{'├'.bold} #{'Target architecture:'.color(:green)} #{triple.empty? ? host_triple : triple}"
    puts "    #{'└'.bold} #{'Options:'.color(:green)} #{defines}"
    puts

    ARCH_CFLAGS.each_pair { |arch, flags|
        if triple =~ arch
            cflags += flags
            break
        end
    }

    COMPILER_CFLAGS.each_pair { |comp, flags|
        if cc =~ comp
            cflags += flags
            break
        end
    }

    if ENV['CFLAGS']
        cflags += [ ENV['CFLAGS'] ]
    end

    unless ENV['WITH_WARNINGS'].to_i == 1
        cflags << '-w'
    end
    
    if defines['NO_BUILTIN'].to_i == 1
        cflags << "-fno-builtin"
    end

    if ENV['OUTPUT_LIB'].to_i == 1
        cflags << '-shared'
    end

    cflags += INCLUDE_DIRS.map{|d| "-I#{d}"}
    defines = defines.map{|k,v|
        v = IPAddr.new(v).to_define if k == 'HOST'
        "-D#{k}=#{v}"
    }

    if ENV['OUTPUT_DEBUG'].to_i == 1
        sh "#{cc_invoke(cc,triple)} -S #{cflags.join(" ")} #{source_file} -o #{output_dir}/#{target_name}.S #{defines.join(' ')}" do |ok, _|
            (puts; show_error("Compilation failed.")) unless ok
        end
    end

    sh "#{cc_invoke(cc,triple)} #{cflags.join(' ')} #{source_file} -o #{output_dir}/#{target_name}.elf #{defines.join(' ')}" do |ok, _|
        (puts; show_error("Compilation failed.")) unless ok
    end
end

def generate_shellcode(target, triple, output_dir)
    _, target_name = target_to_source(target)
    triple += '-' unless triple.empty?
    sh "#{triple}objcopy -O binary -j .text -j .funcs -j .rodata #{output_dir}/#{target_name}.elf #{output_dir}/#{target_name}.bin" do |ok, res|
        (puts; show_error("Cannot extract shellcode from #{output_dir}/#{target_name}.elf")) unless ok
    end

    puts
    data = File.binread("#{output_dir}/#{target_name}.bin")
    show_info "#{'Generated target:'.color(:cyan)} #{data.size} bytes."
    puts "    #{'└'.bold} #{'Contents:'.color(:green)} \"#{data.unpack("C*").map{|b| "\\x%02x" % b}.join.color(:brown)}\"" if ENV['OUTPUT_HEX'].to_i == 1
end

def build(target, *opts)
    output_dir = OUTPUT_DIR
    triple = ''
    triple = ENV['TRIPLE'] if ENV['TRIPLE']

    make_directory(output_dir)
    compile(target, triple, output_dir, *opts)
    generate_shellcode(target, triple, output_dir)
end

def make_directory(path)
    Dir.mkdir(path) unless Dir.exists?(path)
end

task :shellexec do
    build(:shellexec, "COMMAND", "SET_ARGV0")
end

task :memexec do
    build(:memexec, "PAYLOAD_SIZE")
end

task :help do
    STDERR.puts <<-USAGE

 Shellcode generation:

    rake <shellcode> [OPTION1=VALUE1] [OPTION2=VALUE2] ...

 Compilation options:

    CC:             Let you choose the compiler. Only supported are g++ and clang++.  
    TRIPLE:         Cross compilation target. For example: "aarch64-linux-gnu".
    CFLAGS:         Add custom flags to the compiler. For example "-m32".
    NO_BUILTIN:     Does not use the compiler builtins for common memory operations. 
    OUTPUT_LIB:     Compiles to a shared library instead of a standard executable.
    OUTPUT_DEBUG:   Instructs the compiler to emit an assembly file.
    OUTPUT_HEX:     Prints the resulting shellcode as an hexadecimal string.
    WITH_WARNINGS:  Set to 1 to enable compiler warnings.
    RELAX_INLINE:   Set to 1 to let the compiler uninline some functions.

 Shellcode customization options:

    CHANNEL:        Shellcode communication channel.
                    Supported options: NO_CHANNEL, TCP_CONNECT, TCP_LISTEN, TCP6_CONNECT, TCP6_LISTEN, SCTP_CONNECT, SCTP_LISTEN, SCTP6_CONNECT, SCTP6_LISTEN, USE_STDOUT, USE_STDERR
    HOST:           Remote host or local address for socket bind.
    PORT:           Remote port or local port for socket bind.
    FORK_ON_ACCEPT: Keeps listening when accepting connections.
    REUSE_ADDR:     Bind sockets with SO_REUSEADDR.

    USAGE
end

task :default => :help

rule '' do |task|
    if task.name != 'default'
        build(task.name)
    end
end

CLEAN.include("bins/*.{elf,bin}")
