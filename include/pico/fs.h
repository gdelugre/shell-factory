#ifndef PICOLIB_IO_H_
#define PICOLIB_IO_H_

namespace Pico {

    namespace Filesystem {

        using namespace Target::Filesystem;

        struct Rights
        {
            CONSTRUCTOR Rights(fs_rights r) : value(r) {}

            fs_rights value;
        };

        struct Owner
        {
            CONSTRUCTOR Owner(User u, Group g) : user(u), group(g) {}

            User user;
            Group group;
        };

        class File : public BasicStream
        {
            public:
                constexpr static fs_rights default_rights = default_file_rights;
                constexpr static int READ       = (1 << 0);
                constexpr static int WRITE      = (1 << 1);
                constexpr static int APPEND     = (1 << 2);
                constexpr static int TRUNCATE   = (1 << 3);

                FUNCTION File           open(const char *path, int flags);
                FUNCTION File           create(const char *path, int flags, Rights rights = default_rights);
                FUNCTION int            remove(const char *path);
                FUNCTION size_t         size(const char *path);
                FUNCTION bool           exists(const char *path);
                FUNCTION bool           is_readable(const char *path);
                FUNCTION bool           is_writable(const char *path);
                FUNCTION bool           is_executable(const char *path);
                FUNCTION Owner          owner(const char *path);
                FUNCTION Rights         rights(const char *path);
                FUNCTION int            change_owner(const char *path, Owner owner);
                FUNCTION int            change_rights(const char *path, Rights rights);

                NO_RETURN METHOD void   execute(char *const argv[] = nullptr, char *const envp[] = nullptr);
                METHOD size_t           size();
                METHOD Owner            owner();
                METHOD Rights           rights();
                METHOD int              change_owner(Owner owner);
                METHOD int              change_rights(Rights rights);

                CONSTRUCTOR             File(const char *path, int flags = READ|WRITE,
                                             bool create = false, Rights rights = default_rights);
        };

        class Directory : public SingleIO
        {
            public:
                using SingleIO::SingleIO;

                constexpr static fs_rights default_rights = default_dir_rights;

                FUNCTION        Directory open(const char *path);
                FUNCTION int    create(const char *path, Rights rights = default_rights);
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
        };
    }
}

#endif
