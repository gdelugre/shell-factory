#ifndef PICOLIB_DEBUG_H_
#define PICOLIB_DEBUG_H_

namespace Pico {

    class CpuContext
    {
        VIRTUAL_METHOD CPU::reg_t instruction_pointer() = 0;
        VIRTUAL_METHOD CPU::reg_t stack_pointer() = 0;
    };

    class Debuggee : public Process
    {
        class Context;

        public:
            CONSTRUCTOR Debuggee(Process proc) : Debuggee(proc.id()) {}
            CONSTRUCTOR Debuggee(process_id pid) : Process(pid) {
                attach();
            }
            DESTRUCTOR ~Debuggee() {
                detach();
            }

            Context context();
            
        protected:
            METHOD int attach();
            METHOD int detach();
    };
}

#endif
