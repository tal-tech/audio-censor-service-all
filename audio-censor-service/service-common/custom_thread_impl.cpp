#include <string>
#include <map>
#include <set>
#include "base/atomicops.h"
#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/lazy_instance.h"
#include "base/single_thread_task_runner.h"
#include "base/threading/sequenced_worker_pool.h"
#include "base/threading/thread_restrictions.h"
#include "custom_thread_impl.h"
#include "custom_thread_delegate.h"

namespace CppProcess {

    namespace {

#define InitThreadNum 1

// Friendly names for the well-known threads.
        static const char *g_thread_names[CustomThread::ID_COUNT] = {
            "Custom_UploadThread",
            "Custom_ProcessThread",
            "Custom_TimerThread",
            "Custom_KafkaThread",
            "Custom_SyncThread",
            "Custom_CustomProcess1Thread",
            "Custom_CustomProcess2Thread",
            "Custom_WSInitProcess",
            "Custom_WSRemove"
        };

        struct CustomThreadGlobals {
            CustomThreadGlobals() : initialized(false){
                memset(threads, 0, (CustomThread::Sync + 1) * sizeof(threads[0]));
                memset(thread_delegates, 0, (CustomThread::Sync + 1) * sizeof(thread_delegates[0]));
            }

            struct ThreadPoolInfo{
                // This lock protects |threads|. Do not read or modify that array
                // without holding this lock. Do not block while holding this lock.
                base::Lock lock;

                //上一次添加了多个任务的线程index
                size_t lastThreadAddedTask = 0;

                //active threads number
                size_t activeThreadNum = InitThreadNum;

                std::vector<CustomThreadImpl*>  threads;
            };

            //initialized
            volatile bool initialized;

            // This array is protected by |lock|. The threads are not owned by this
            // array. Typically, the threads are owned on the UI thread by
            // BrowserMainLoop. CustomThreadImpl objects remove themselves from this
            // array upon destruction.
            CustomThreadImpl *threads[CustomThread::Sync + 1];

            std::map <CustomThread::ID, ThreadPoolInfo>  threadPool;

            base::Lock threadPoolLock;

            // Only atomic operations are used on this array. The delegates are not owned
            // by this array, rather by whoever calls CustomThread::SetDelegate.
            CustomThreadDelegate *thread_delegates[CustomThread::Sync + 1];
        };

        base::LazyInstance<CustomThreadGlobals>::Leaky
            g_globals = LAZY_INSTANCE_INITIALIZER;
    }  // namespace

    CustomThreadImpl::CustomThreadImpl(ID identifier)
        : Thread(g_thread_names[identifier]),
          identifier_(identifier) {
        Initialize();
    }

    CustomThreadImpl::CustomThreadImpl(ID identifier,
                                       base::MessageLoop *message_loop)
        : Thread(message_loop->thread_name()), identifier_(identifier) {
        set_message_loop(message_loop);
        Initialize();
    }

    void CustomThreadImpl::Init() {
        CustomThreadGlobals &globals = g_globals.Get();
        if(identifier_ >= 0 && identifier_ <= Sync) {
            using base::subtle::AtomicWord;
            AtomicWord *storage =
                reinterpret_cast<AtomicWord *>(&globals.thread_delegates[identifier_]);
            AtomicWord stored_pointer = base::subtle::NoBarrier_Load(storage);
            CustomThreadDelegate *delegate =
                reinterpret_cast<CustomThreadDelegate *>(stored_pointer);
            if(delegate)
                delegate->Init();
        }
    }

    NOINLINE void CustomThreadImpl::CustomProcess2ThreadRun(base::MessageLoop *message_loop) {
        volatile int line_number = __LINE__;
        Thread::Run(message_loop);
        CHECK_GT(line_number, 0);
    }

    NOINLINE void CustomThreadImpl::UploadThreadRun(base::MessageLoop *message_loop) {
        volatile int line_number = __LINE__;
        Thread::Run(message_loop);
        CHECK_GT(line_number, 0);
    }

    NOINLINE void CustomThreadImpl::ProcessThreadRun(base::MessageLoop *message_loop) {
        volatile int line_number = __LINE__;
        Thread::Run(message_loop);
        CHECK_GT(line_number, 0);
    }

    NOINLINE void CustomThreadImpl::TimerThreadRun(base::MessageLoop *message_loop) {
        volatile int line_number = __LINE__;
        Thread::Run(message_loop);
        CHECK_GT(line_number, 0);
    }

    NOINLINE void CustomThreadImpl::KafkaThreadRun(base::MessageLoop *message_loop) {
        volatile int line_number = __LINE__;
        Thread::Run(message_loop);
        CHECK_GT(line_number, 0);
    }

    NOINLINE void CustomThreadImpl::SyncThreadRun(base::MessageLoop *message_loop) {
        volatile int line_number = __LINE__;
        Thread::Run(message_loop);
        CHECK_GT(line_number, 0);
    }

    NOINLINE void CustomThreadImpl::CustomProcess1ThreadRun(base::MessageLoop *message_loop) {
        volatile int line_number = __LINE__;
        Thread::Run(message_loop);
        CHECK_GT(line_number, 0);
    }

    NOINLINE void CustomThreadImpl::WSInitProcessThreadRun(base::MessageLoop *message_loop) {
        volatile int line_number = __LINE__;
        Thread::Run(message_loop);
        CHECK_GT(line_number, 0);
    }

    NOINLINE void CustomThreadImpl::WSRemoveThreadRun(base::MessageLoop *message_loop) {
        volatile int line_number = __LINE__;
        Thread::Run(message_loop);
        CHECK_GT(line_number, 0);
    }


    void CustomThreadImpl::Run(base::MessageLoop *message_loop) {
        CustomThread::ID thread_id = ID_COUNT;
        if(!GetCurrentThreadIdentifier(&thread_id))
            return Thread::Run(message_loop);

        switch(thread_id) {
            case CustomThread::CustomProcess2:
                return CustomProcess2ThreadRun(message_loop);
            case CustomThread::Upload:
                return UploadThreadRun(message_loop);
            case CustomThread::Process:
                return ProcessThreadRun(message_loop);
            case CustomThread::Timer:
                return TimerThreadRun(message_loop);
            case CustomThread::Kafka:
                return KafkaThreadRun(message_loop);
            case CustomThread::Sync:
                return SyncThreadRun(message_loop);
            case CustomThread::CustomProcess1:
                return CustomProcess1ThreadRun(message_loop);
            case CustomThread::WSInitProcess:
                return WSInitProcessThreadRun(message_loop);
            case CustomThread::WSRemove:
                return WSRemoveThreadRun(message_loop);
            case CustomThread::ID_COUNT:
                CHECK(false);  // This shouldn't actually be reached!
                break;
        }
        Thread::Run(message_loop);
    }

    void CustomThreadImpl::CleanUp() {
        CustomThreadGlobals &globals = g_globals.Get();
        if(identifier_ >= 0 && identifier_ <= Sync) {
            using base::subtle::AtomicWord;
            AtomicWord *storage =
                reinterpret_cast<AtomicWord *>(&globals.thread_delegates[identifier_]);
            AtomicWord stored_pointer = base::subtle::NoBarrier_Load(storage);
            CustomThreadDelegate *delegate =
                reinterpret_cast<CustomThreadDelegate *>(stored_pointer);

            if(delegate)
                delegate->CleanUp();
        }
    }

    void CustomThreadImpl::Initialize() {
        if(identifier_ >= 0 && identifier_ <= Sync) {
            CustomThreadGlobals &globals = g_globals.Get();
            DCHECK(globals.threads[identifier_] == NULL);
            globals.threads[identifier_] = this;
        }
    }

    CustomThreadImpl::~CustomThreadImpl() {
        // All Thread subclasses must call Stop() in the destructor. This is
        // doubly important here as various bits of code check they are on
        // the right CustomThread.
        Stop();
        if(identifier_ >= 0 && identifier_ <= Sync) {
            CustomThreadGlobals &globals = g_globals.Get();
            globals.threads[identifier_] = NULL;
        }
    }

    bool CustomThreadImpl::StartWithOptions(const Options &options) {
        // The global thread table needs to be locked while a new thread is
        // starting, as the new thread can asynchronously start touching the
        // table (and other thread's message_loop).
        CustomThreadGlobals &globals = g_globals.Get();
        base::AutoLock lock(globals.threadPoolLock);
        return Thread::StartWithOptions(options);
    }

// static
    bool CustomThreadImpl::PostTaskHelper(
        CustomThread::ID identifier,
        const tracked_objects::Location &from_here,
        const base::Closure &task,
        base::TimeDelta delay,
        bool nestable) {

        if(g_globals == nullptr){
            GLOG(FATAL) << "uninit CustomThread";
            return false;
        }

        base::MessageLoop *message_loop = nullptr;
        CustomThreadGlobals &globals = g_globals.Get();

        if(!globals.initialized)
            return false;

        auto iThreadPool = globals.threadPool.find(identifier);
        if(identifier >= 0 && identifier <= Sync) {
            message_loop =
                globals.threads[identifier] ? globals.threads[identifier]->message_loop()
                                            : nullptr;
        } else if(iThreadPool != globals.threadPool.end()) {

            base::AutoLock lock(iThreadPool->second.lock);
            //重新找一个线程添加(记录上一次添加的位置)，
            message_loop = iThreadPool->second.threads.
                at((iThreadPool->second.lastThreadAddedTask++) %
                       iThreadPool->second.activeThreadNum)->message_loop();
        }else{
            GLOG(ERROR) << "unable to find thread id to post:" << identifier;
        }

        if(message_loop) {
            if(nestable) {
                message_loop->PostDelayedTask(from_here, task, delay);
            } else {
                message_loop->PostNonNestableDelayedTask(from_here, task, delay);
            }
        }else{
            GLOG(ERROR) << "unable to get valid message loop to post:" << identifier;
        }
        return message_loop != nullptr;
    }

// static
    bool CustomThread::IsThreadInitialized(ID identifier) {
        if(g_globals == nullptr){
            GLOG(FATAL) << "uninit CustomThread";
            return false;
        }

        CustomThreadGlobals &globals = g_globals.Get();
        auto iThreadPool = globals.threadPool.find(identifier);
        if(identifier >= 0 && identifier <= Sync)
            return globals.threads[identifier] != nullptr;
        else if(iThreadPool != globals.threadPool.end()) {
            base::AutoLock lock(iThreadPool->second.lock);
            return !iThreadPool->second.threads.empty();
        } else
            return false;
    }

// static
    bool CustomThread::CurrentlyOn(ID identifier) {
        if(g_globals == nullptr){
            GLOG(FATAL) << "uninit CustomThread";
            return false;
        }
        // We shouldn't use MessageLoop::current() since it uses LazyInstance which
        // may be deleted by ~AtExitManager when a WorkerPool thread calls this
        // function.
        // http://crbug.com/63678
        base::ThreadRestrictions::ScopedAllowSingleton allow_singleton;
        CustomThreadGlobals &globals = g_globals.Get();

        if(!globals.initialized){
            GLOG(FATAL) << "thread pool not init";
            return false;
        }
        auto iThreadPool = globals.threadPool.find(identifier);
        if(identifier >= 0 && identifier <= Sync) {
            return globals.threads[identifier] &&
                   globals.threads[identifier]->message_loop() ==
                   base::MessageLoop::current();
        } else if(iThreadPool != globals.threadPool.end()) {
            base::AutoLock lock(iThreadPool->second.lock);
            base::MessageLoop *curmessageloop = base::MessageLoop::current();
            for(auto & thread : iThreadPool->second.threads) {
                if(curmessageloop == thread->message_loop())
                    return true;
            }
        }
        return false;
    }

// static
    int CustomThread::GetThreadPoolWorkingNum(ID thread_id){
        if(g_globals == nullptr){
            GLOG(FATAL) << "uninit CustomThread";
            return 0;
        }

        CustomThreadGlobals &globals = g_globals.Get();
        auto iThreadPool = globals.threadPool.find(thread_id);

        int working_num = 0;
        if(iThreadPool != globals.threadPool.end()){
            working_num = iThreadPool->second.activeThreadNum;
        }
        return working_num;
    }

// static
    void CustomThread::SetThreadPoolWorkingMaxNum(ID threadID, size_t threadNum) {
        //不允许设置线程数为0
        if(g_globals == nullptr){
            GLOG(FATAL) << "uninit CustomThread";
            return;
        }
        if(threadNum == 0){
            GLOG(WARNING) << "cannot set thread num with 0";
            return;
        }

        CustomThreadGlobals &globals = g_globals.Get();
        auto iThreadPool = globals.threadPool.find(threadID);
        if(iThreadPool != globals.threadPool.end()){
            base::AutoLock lock(iThreadPool->second.lock);

            //thread max num
            if(threadNum > 100)
                iThreadPool->second.activeThreadNum = 100;
            else
                iThreadPool->second.activeThreadNum = threadNum;

            //补齐线程数量, 保证线程数量总是大于等于activeThreadNum
            if(iThreadPool->second.activeThreadNum > iThreadPool->second.threads.size()) {
                size_t addThreadNum = iThreadPool->second.activeThreadNum - iThreadPool->second.threads.size();
                for(unsigned int i = 0;
                    i < addThreadNum; ++i) {
                    auto *thread = new CustomThreadImpl(threadID);
                    thread->Start();
                    iThreadPool->second.threads.push_back(thread);
                }
            }
        }
    }


//static
    void CustomThread::InitGlobal() {
        if(!(g_globals == nullptr)){
            GLOG(WARNING) << "already init CustomThread";
            return;
        }

        CustomThreadGlobals &globals = g_globals.Get();

        for(int i = 0; i <= CustomThread::Sync; i++) {
            globals.threads[i] = new CustomThreadImpl(static_cast<CustomThread::ID>(i));
            globals.threads[i]->Start();
        }

        for(int i = CustomThread::Sync + 1; i < CustomThread::ID_COUNT; i++) {
            for(int j = 0; j < InitThreadNum; ++j) {
                auto *lthread = new CustomThreadImpl((CustomThread::ID)i);
                lthread->Start();
                globals.threadPool[(CustomThread::ID)i].threads.push_back(lthread);
            }
        }

        globals.initialized = true;
    }

// static
    bool CustomThread::PostTask(ID identifier,
                                const tracked_objects::Location &from_here,
                                const base::Closure &task) {
        return CustomThreadImpl::PostTaskHelper(
            identifier, from_here, task, base::TimeDelta(), true);
    }

// static
    bool CustomThread::PostDelayedTask(ID identifier,
                                       const tracked_objects::Location &from_here,
                                       const base::Closure &task,
                                       base::TimeDelta delay) {
        return CustomThreadImpl::PostTaskHelper(
            identifier, from_here, task, delay, true);
    }

// static
    bool CustomThread::PostNonNestableTask(
        ID identifier,
        const tracked_objects::Location &from_here,
        const base::Closure &task) {
        return CustomThreadImpl::PostTaskHelper(
            identifier, from_here, task, base::TimeDelta(), false);
    }

// static
    bool CustomThread::PostNonNestableDelayedTask(
        ID identifier,
        const tracked_objects::Location &from_here,
        const base::Closure &task,
        base::TimeDelta delay) {
        return CustomThreadImpl::PostTaskHelper(
            identifier, from_here, task, delay, false);
    }

// static
    bool CustomThread::GetCurrentThreadIdentifier(ID *identifier) {
        if(g_globals == nullptr){
            return false;
        }

        base::ThreadRestrictions::ScopedAllowSingleton allow_singleton;
        base::MessageLoop *cur_message_loop = base::MessageLoop::current();
        CustomThreadGlobals &globals = g_globals.Get();

        if(!globals.initialized){
            return false;
        }

        for(int i = 0; i <= Sync; ++i) {
            if(globals.threads[i] &&
               globals.threads[i]->message_loop() == cur_message_loop) {
                *identifier = globals.threads[i]->identifier_;
                return true;
            }
        }

        for(auto & iThreadPool : globals.threadPool){
            base::AutoLock lock(iThreadPool.second.lock);
            for(unsigned int i = 0; i < iThreadPool.second.threads.size(); ++i) {
                if(cur_message_loop == iThreadPool.second.threads.at(i)->message_loop()) {
                    *identifier = iThreadPool.first;
                    return true;
                }
            }
        }
        return false;
    }

//static
    std::string CustomThread::GetCurrentThreadName() {

        CustomThread::ID thread_id = ID_COUNT;
        if(!GetCurrentThreadIdentifier(&thread_id))
            return "Unknow thread";

        return std::string(g_thread_names[thread_id]);

    }

// static
    void CustomThread::SetDelegate(ID identifier,
                                   CustomThreadDelegate *delegate) {
        if(g_globals == nullptr){
            GLOG(FATAL) << "need to init CustomThread";
            return;
        }


        using base::subtle::AtomicWord;
        CustomThreadGlobals &globals = g_globals.Get();

        if(!globals.initialized){
            GLOG(FATAL) << "uninit thread pool";
            return;
        }

        if(identifier >= 0 && identifier <= Sync) {
            auto *storage = reinterpret_cast<AtomicWord *>(
                &globals.thread_delegates[identifier]);
            AtomicWord old_pointer = base::subtle::NoBarrier_AtomicExchange(
                storage, reinterpret_cast<AtomicWord>(delegate));

            // This catches registration when previously registered.
            DCHECK(!delegate || !old_pointer);
        }
    }

}// end namespace CppProcess