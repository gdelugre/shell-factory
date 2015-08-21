#ifndef PICOLIB_IO_H_
#define PICOLIB_IO_H_

using namespace Pico;

namespace Pico {

    namespace Filesystem {

        class File : public BasicStream
        {
            public:
                constexpr static int READ       = (1 << 0);
                constexpr static int WRITE      = (1 << 1);
                constexpr static int APPEND     = (1 << 2);
                constexpr static int TRUNCATE   = (1 << 3);

                FUNCTION File   open(const char *path, int flags);
                FUNCTION File   create(const char *path, int flags, mode_t mode);
                FUNCTION int    remove(const char *path);

                CONSTRUCTOR     File(const char *path, int flags = READ|WRITE, bool create = false, mode_t mode = 0700);
        };

        class Directory
        {
            public:
                FUNCTION        Directory&& open(const char *path);
                FUNCTION int    create(const char *path, mode_t mode);
                FUNCTION int    remove(const char *path);

                template <typename Func>
                FUNCTION int    each(const char *path, Func);
                FUNCTION int    set_current(const char *path);
                FUNCTION int    get_current(char *path, size_t size);
                FUNCTION int    change_root(const char *path);

                CONSTRUCTOR Directory(const char *path);

                template <typename Func>
                METHOD int      list(Func);
                METHOD int      set_current();
                METHOD int      close();
                METHOD int      file_desc() const { return fd; }

            private:
                int fd = -1;
        };
    }
}

#endif
