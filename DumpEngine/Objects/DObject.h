
#ifndef DUMPENGINE_DOBJECT_H
#define DUMPENGINE_DOBJECT_H
#include <atomic>
#include <cstdint>


class DObject {
public:
    DObject() = default;
protected:
    std::uint32_t ID = -1;

private:
    static std::atomic<unsigned int> s_NextID;

public:
    virtual void Start();

};


#endif //DUMPENGINE_DOBJECT_H