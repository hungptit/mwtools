//
//  main.cpp
//  BarrierC++11
//
//  Created by dima on 6/17/14.
//
//

#include <iostream>
#include <exception>
#include <tuple>
#include <vector>
#include <thread>
#include <future>
#include <mutex>              // std::mutex, std::unique_lock
//#include <shared_mutex>
#include <condition_variable> // std::condition_variable

#include <cxxabi.h>

struct MyException {
    MyException(const std::string &msg) : m_msg(msg) {}
    std::string m_msg;
};

typedef std::shared_ptr<std::thread>        thread_type;

std::string unknown_exception_name() throw()
{
    const std::type_info* const t = abi::__cxa_current_exception_type();
    if(t == NULL) return std::string("unknown");
    
    char *demangled_name = abi::__cxa_demangle(t->name(), NULL, NULL, NULL);
    std::string str(demangled_name);
    free(demangled_name);
    
    return str;
}


template<typename R>
class FutureInvestigator {
public:
    typedef std::shared_future<R>  future_type;
    
    FutureInvestigator(int idx = -1) : fIndex(idx) {}
    void operator()(future_type & future) {
        try {
            R r = future.get();
            if(std::get<1>(r)) {
                std::exception_ptr eptr = std::get<1>(r);
                try {
                    std::rethrow_exception(eptr);
                } catch ( ... ) {
                    std::string ex_name = unknown_exception_name();
                    std::cout << "THREAD EXCEPTION:  in future " << ex_name << std::endl;
                    if(ex_name == "MyException") {
                        try {
                            throw;
                        } catch (const MyException &mex) {
                            std::cout << "MyException EXCEPTION: from THREAD " << mex.m_msg << std::endl;
                        }
                    }
                }
            } else {
                double d = std::get<0>(r);
                std::cout << "OK " << d << std::endl;
            }
        } catch (const std::exception &ex) {
            std::cout << "EXCEPTION:  in future " << fIndex << ex.what() << std::endl;
        } catch ( ... ) {
            std::string ex_name = unknown_exception_name();
            
            std::cout << "UNKNOWN EXCEPTION:  in future " << ex_name << std::endl;
        }
    }
private:
    int fIndex;
};


class barrier
{
public:
    barrier(unsigned int count)
    : m_threshold(count), m_count(count), m_generation(0)
    {
        if (count == 0)
            throw std::invalid_argument("count cannot be zero.");
    }
    
    void wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        unsigned int gen = m_generation;
        
        if (--m_count == 0)
        {
            m_generation++;
            m_count = m_threshold;
            m_cond.notify_all();
            return;
        }
        
        while (gen == m_generation)
            m_cond.wait(lock);
        return;
    }
    
private:
    std::mutex m_mutex;
    std::condition_variable m_cond;
    unsigned int m_threshold;
    unsigned int m_count;
    unsigned int m_generation;
};


template<typename Worker>
class Runner {
public:
    Runner(barrier  &barrier, Worker w) : fBarrier(barrier), fWorker(w){
        
    }
    typename Worker::result_type operator() () {
        fBarrier.wait();
        return fWorker();
    }
    
private:
    barrier  &fBarrier;
    Worker   fWorker;
};

using namespace std::placeholders;

template<typename Worker>
void runWorkers(std::vector<Worker> workers) {
    typedef std::shared_future<typename Worker::result_type>  future_type;
    
    
    barrier bar((int)workers.size());
    std::vector< std::shared_ptr<Runner<Worker> > > runners;
    for(int i = 0; i < workers.size(); i++) {
        runners.push_back(std::shared_ptr<Runner<Worker> >(new Runner<Worker>(bar, workers[i])));
    }
    
    std::vector<future_type> futures;
    std::vector<thread_type> threads;
    for(int i = 0; i < runners.size(); i++) {
        std::packaged_task<typename Worker::result_type ()> task(*runners[i]);
        std::shared_future<typename Worker::result_type> future(task.get_future());
        futures.push_back(future);
        threads.push_back(thread_type(new std::thread(std::move(task))));
    }
    
    std::for_each(threads.begin(), threads.end(), std::bind(&std::thread::join, _1)); //std::placeholders::_1
    
    for(typename std::vector<future_type>::iterator it = futures.begin(); it != futures.end(); ++it) {
        FutureInvestigator<typename Worker::result_type> fi(1 + (int)(it - futures.begin()));
        fi(*it);
    }
}


class RunMe {
public:
    
    typedef std::tuple<double, std::exception_ptr> result_type;
    
    RunMe(int idx, double r) :
    fIndex(idx), fResult(r) { }
    
    ~RunMe() { }
    
    result_type operator()() {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::cout << "RunMe " << fIndex << std::endl;
        switch (fIndex) {
            case 1:
                throw fIndex;
            case 2:
                try {
                    throw MyException("hello world from the thread");
                } catch ( ... ) {
                    return std::make_tuple(fResult, std::current_exception());
                }
                break;
            default:
                break;
        }
        return std::make_tuple(fResult, nullptr);
    }
private:
    static std::mutex m_mutex;
    int         fIndex;
    double      fResult;
};

std::mutex RunMe::m_mutex;

#define NTHREADS 3

int main(int argc, const char * argv[])
{
    std::vector<RunMe> runners;
    for(int i = 0; i < NTHREADS; i++) {
        runners.push_back(RunMe(i, 10*i));
    }
    runWorkers(runners);
    return 0;
}

