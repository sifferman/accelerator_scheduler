
#include "system.h"

#include <set>
#include <iostream>


#define __compare__shared_ptr_computation_t bool(*)(std::shared_ptr<computation_t>,std::shared_ptr<computation_t>)
bool __compare__shared_ptr_computation(shared_ptr<computation_t> l, shared_ptr<computation_t> r) {
    return l->priority < r->priority;
}


shared_ptr<accelerator_t> system_t::choose_one_accelerator(const bool parallel) const {

    unordered_set< shared_ptr<accelerator_t> > accels;
    for (auto c : computations)
        for (auto a : c->accelerators)
            accels.insert(a);

    shared_ptr<accelerator_t> out = 0;
    double max_speedup = 1.0;

    cout << accels.size() << endl;

    for (auto a : accels) { // for every accelerator
        uint32_t homogenious_time = 0;
        uint32_t system_time = 0;
        uint32_t accelerator_time = fpga->p;
        for (auto c : computations) {
            if (c->accelerators.find(a) != c->accelerators.end()) { // if computation can use accelerator
                accelerator_time += c->accelerator_setup_time(a);
                system_time += c->accelerator_setup_time(a);
                accelerator_time += c->accelerator_computation_time(a);
            } else {
                system_time += c->system_time();
            }
            homogenious_time += c->system_time();
        }
        uint32_t heterogeneous_time = parallel ? max(accelerator_time, system_time) : (accelerator_time+system_time);
        double speedup = (double)homogenious_time / heterogeneous_time;
        cout << a->name << ": "<< speedup << endl;
        if (speedup > max_speedup) {
            out = a;
            max_speedup = speedup;
        }
    }

    return out;
}