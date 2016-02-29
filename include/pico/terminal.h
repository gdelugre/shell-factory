#ifndef PICOLIB_TERMINAL_H_
#define PICOLIB_TERMINAL_H_

namespace Pico {

    class TTY : public BasicStream
    {
        public:
            using Settings = Target::terminal_settings;
            using BasicStream::BasicStream;

            struct Size {
                unsigned short rows;
                unsigned short cols;
            };

            METHOD Size size();
            METHOD int set_size(Size);
            METHOD int flush();

            METHOD int get_settings(Settings& settings);
            METHOD int set_settings(Settings settings);
            METHOD int set_raw(Settings& old);
    };
}

#endif
