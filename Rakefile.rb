require 'rake'
require 'rake/clean'

require 'pathname'
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
        "\e[1m#{self}\e[0m"
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

class Triple
    attr_accessor :arch, :vendor, :os, :abi

    def initialize(arch, vendor, os, abi)
        @arch, @vendor, @os, @abi = arch, vendor, os, abi
    end

    def to_s
        [ @arch, @vendor, @os, @abi ].delete_if{|f| f.nil?}.join('-')
    end

    def self.parse(str)
        fields = str.split('-')
        case fields.size
        when 2 then Triple.new(fields[0], nil, fields[1], nil)
        when 3 then Triple.new(fields[0], nil, fields[1], fields[2])
        when 4 then Triple.new(fields[0], fields[1], fields[2], fields[3])
        else
            fail "Cannot parse triple: #{str}"
        end
    end

    def self.current
        Triple.parse RbConfig::CONFIG['target']
    end
end

CC = "g++"
OUTPUT_DIR = "bins"
SHELLCODE_DIR = "shellcodes"
INCLUDE_DIRS = %w{include}
LD_SCRIPT = File.join(File.dirname(__FILE__), "factory.lds")
OUTPUT_SECTIONS = %w{.text .rodata .data}
CFLAGS = %W{-std=c++1y
            -Wall
            -Wextra
            -Wfatal-errors
            -fno-common
            -fno-rtti
            -fno-exceptions
            -fno-non-call-exceptions
            -fomit-frame-pointer
            -ffunction-sections
            -fdata-sections
            -nostdlib
            -Wl,-e_start
            -Wl,--gc-sections
            -Wl,-N
            -Wl,-T#{LD_SCRIPT}
         }

COMPILER_CFLAGS =
{
    /^g\+\+/ =>
        %w{-fno-toplevel-reorder
           -finline-functions
           -nodefaultlibs
           -Os
           },

    /^clang\+\+/ =>
        %w{-Oz
           -Wno-invalid-noreturn
          }
}

OS_CFLAGS =
{
    /linux/ => %w{-Wl,--build-id=none},
    /none/ => %w{-U__STDC_HOSTED__}
}

# Architecture-dependent flags.
ARCH_CFLAGS =
{
    /mips/ => %w{-mshared -mno-abicalls -mno-plt -mno-gpopt -mno-long-calls -G 0},
    /.*/ => %w{-fPIC}
}

def show_info(str, list = {})
    STDERR.puts "[".bold + "*".bold.color(:green) + "] ".bold + str

    list.each_with_index do |item, i|
        name, value = item
        branch = (i == list.size - 1) ? '└' : '├'
        STDERR.puts "    #{branch.bold} #{(name + ?:).color(:green)} #{value}"
    end
end

def show_error(str)
    STDERR.puts "[".bold + "*".bold.color(:red) + "] ".bold + 'Error: '.color(:cyan) + str
    abort
end

def cc_invoke(cc, triple, sysroot = nil)
    if triple.empty?
        return cc if sysroot.nil?
        return "#{cc} --sysroot=#{sysroot}"
    end

    triple_cc =
    case cc
    when 'g++'
        "#{triple}-#{cc}"

    when 'clang++'
        sysroot ||= "/usr/#{triple}"
        "#{cc} -target #{triple} --sysroot=#{sysroot}"
    end

    triple_cc << " --sysroot=#{sysroot}" unless sysroot.nil?
    triple_cc
end

# Returns [ source_path, output_basename ]
def target_to_source(target)
    path = Pathname.new(target.to_s).cleanpath
    if path.relative? and path.each_filename.to_a.size == 1
        path = Pathname.new(SHELLCODE_DIR).join(path)
    end

    path.split
end

def compile(target, triple, output_dir, *opts)
    common_opts = %w{CHANNEL HOST PORT NO_BUILTIN FORK_ON_ACCEPT REUSE_ADDR RELAX_INLINE NO_ASSERTS HEAP_BASE HEAP_SIZE}
    options = common_opts + opts
    defines = ENV.select{|e| options.include?(e)}
    options = common_opts + opts
    cc = ENV['CC'] || CC
    cflags = CFLAGS.dup
    source_dir, target_name = target_to_source(target)
    source_file = source_dir.join("#{target_name}.cc")
    sysroot = ENV['SYSROOT']

    unless File.exists?(source_file)
        show_error("Cannot find source for target '#{target.to_s.color(:red)}'.")
    end

    host_triple = Triple.current
    target_triple = triple.empty? ? Triple.current : Triple.parse(triple)

    show_info("#{'Generating target'.color(:cyan)} '#{target.to_s.color(:red)}'",
        'Compiler' => cc,
        'Host architecture' => host_triple,
        'Target architecture' => target_triple,
        'Options' => defines
    )
    STDERR.puts

    ARCH_CFLAGS.each_pair do |arch, flags|
        if target_triple.arch =~ arch
            cflags += flags
            break
        end
    end

    OS_CFLAGS.each_pair do |os, flags|
        if target_triple.os =~ os
            cflags += flags
            break
        end
    end

    COMPILER_CFLAGS.each_pair do |comp, flags|
        if cc =~ comp
            cflags += flags
            break
        end
    end

    if ENV['STACK_REALIGN'].to_i == 1
        cflags << "-mstackrealign"
    end

    if ENV['CFLAGS']
        cflags += [ ENV['CFLAGS'] ]
    end

    if ENV['ARCH']
        cflags << "-march=#{ENV['ARCH']}"
    end

    if ENV['CPU']
        cflags << "-mcpu=#{ENV['CPU']}"
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

    if ENV['OUTPUT_STRIP'].to_i == 1
        cflags << "-s"
    end

    if ENV['OUTPUT_DEBUG'].to_i == 1
        output_file = output_dir.join("#{target_name}.S")
        cflags << '-g'
        sh "#{cc_invoke(cc,triple,sysroot)} -S #{cflags.join(" ")} #{source_file} -o #{output_file} #{defines.join(' ')}" do |ok, _|
            (STDERR.puts; show_error("Compilation failed.")) unless ok
        end
    end

    output_file = output_dir.join("#{target_name}.elf")
    sh "#{cc_invoke(cc,triple,sysroot)} #{cflags.join(' ')} #{source_file} -o #{output_file} #{defines.join(' ')}" do |ok, _|
        (STDERR.puts; show_error("Compilation failed.")) unless ok
    end
end

def generate_shellcode(target, triple, output_dir)
    _, target_name = target_to_source(target)
    triple_info = triple.empty? ? Triple.current : Triple.parse(triple)

    input_file = output_dir.join("#{target_name}.elf")
    output_file = output_dir.join("#{target_name}.#{triple_info.arch}-#{triple_info.os}.bin")

    # Extract shellcode.
    triple += '-' unless triple.empty?
    sections = OUTPUT_SECTIONS.map{|s| "-j #{s}"}.join(' ')
    sh "#{triple}objcopy -O binary #{sections} #{input_file} #{output_file}" do |ok, res|
        STDERR.puts
        show_error("Cannot extract shellcode from #{input_file}") unless ok
    end

    # Read shellcode.
    data = File.binread(output_file)

    output = {}
    output['Contents'] = "\"#{data.unpack("C*").map{|b| "\\x%02x" % b}.join.color(:brown)}\"" if ENV['OUTPUT_HEX'].to_i == 1
    show_info("#{'Generated target:'.color(:cyan)} #{data.size} bytes.", output)

    print data unless STDOUT.tty?
end

def build(target, *opts)
    output_dir = Pathname.new(OUTPUT_DIR)
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

desc 'Show help'
task :help do
    STDERR.puts <<-USAGE

 #{'Shellcode generation:'.color(:cyan)}

    #{'rake <shellcode> [OPTION1=VALUE1] [OPTION2=VALUE2] ...'.bold}

 #{'Compilation options:'.color(:cyan)}

    #{'CC:'.color(:green)}             Let you choose the compiler. Only supported are g++ and clang++.
    #{'TRIPLE:'.color(:green)}         Cross compilation target. For example: "aarch64-linux-gnu".
    #{'ARCH'.color(:green)}            Specify a specific architecture to compile to (e.g. armv7-r).
    #{'CPU'.color(:green)}             Specify a specific CPU to compile to (e.g. cortex-a15).
    #{'CFLAGS:'.color(:green)}         Add custom flags to the compiler. For example "-m32".
    #{'SYSROOT:'.color(:green)}        Use the specified directory as the filesystem root for finding headers.
    #{'NO_BUILTIN:'.color(:green)}     Does not use the compiler builtins for common memory operations.
    #{'OUTPUT_LIB:'.color(:green)}     Compiles to a shared library instead of a standard executable.
    #{'OUTPUT_DEBUG:'.color(:green)}   Instructs the compiler to emit an assembly file.
    #{'OUTPUT_STRIP:'.color(:green)}   Strip symbols from output file.
    #{'OUTPUT_HEX:'.color(:green)}     Prints the resulting shellcode as an hexadecimal string.
    #{'WITH_WARNINGS:'.color(:green)}  Set to 1 to enable compiler warnings.
    #{'NO_ASSERTS:'.color(:green)}     Set to 1 to disable runtime asserts.
    #{'RELAX_INLINE:'.color(:green)}   Set to 1, 2 or 3 to let the compiler uninline some functions.
    #{'STACK_REALIGN:'.color(:green)}  Set to 1 to ensure stack alignment to a 16 bytes boundary (Intel only).

 #{'Shellcode customization options:'.color(:cyan)}

    #{'CHANNEL:'.color(:green)}        Shellcode communication channel.
                    Supported options: NO_CHANNEL, TCP_CONNECT, TCP_LISTEN, TCP6_CONNECT, TCP6_LISTEN, SCTP_CONNECT, SCTP_LISTEN, SCTP6_CONNECT, SCTP6_LISTEN, USE_STDOUT, USE_STDERR
    #{'HOST:'.color(:green)}           Remote host or local address for socket bind.
    #{'PORT:'.color(:green)}           Remote port or local port for socket bind.
    #{'FORK_ON_ACCEPT:'.color(:green)} Keeps listening when accepting connections.
    #{'REUSE_ADDR:'.color(:green)}     Bind sockets with SO_REUSEADDR.
    #{'HEAP_BASE:'.color(:green)}      Base address for heap allocations.
    #{'HEAP_SIZE:'.color(:green)}      Size of heap, defaults to 64k.

    USAGE

    exit
end

task :default => :help

rule '' do |task|
    if task.name != 'default'
        build(task.name)
    end
end

CLEAN.include("bins/*.{elf,bin}")
