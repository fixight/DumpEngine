//
// Created by egors on 28.11.2025.
//

#ifndef DUMPENGINE_DENGINE_H
#define DUMPENGINE_DENGINE_H
#include <memory>


class TickManager;

class DEngine {

public:

    static DEngine& Get();

    void InitEngine();

    std::shared_ptr<TickManager> GetTickManager(){return tickManager;};


private:
    std::shared_ptr<TickManager> tickManager;

private:
    DEngine() = default;
};


#endif //DUMPENGINE_DENGINE_H