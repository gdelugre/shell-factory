#ifndef PICOLIB_DEBUG_H_
#define PICOLIB_DEBUG_H_

namespace Pico {

    class Debuggee : public Process
    {
        public:
            CONSTRUCTOR Debuggee(Process proc) : Debuggee(proc.process_id()) {}
            CONSTRUCTOR Debuggee(pid_t pid) : Process(pid) {
                attach();
            }
            DESTRUCTOR ~Debuggee() {
                detach();
            }
            
        protected:
            METHOD int attach();
            METHOD int detach();
    };
}

#endif
