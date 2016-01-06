#ifndef PICOLIB_SECURITY_H_
#define PICOLIB_SECURITY_H_

namespace Pico {

    class User 
    {
        public:
            using user_id = Target::user_id;

            CONSTRUCTOR User(user_id id) : uid(id) {}
            FUNCTION User current();
            METHOD user_id id() const { return uid; }

        private:
            user_id uid;
    };

    class Group
    {
        public:
            using group_id = Target::group_id;

            CONSTRUCTOR Group(group_id id) : gid(id) {}
            FUNCTION Group current();
            METHOD group_id id() const { return gid; }

        private:
            group_id gid;
    };
}

#endif
