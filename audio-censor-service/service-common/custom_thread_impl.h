#ifndef CONTENT_GODEYE_GODEYE_THREAD_IMPL_H_
#define CONTENT_GODEYE_GODEYE_THREAD_IMPL_H_

#include "base/threading/thread.h"
#include "custom_thread.h"
#include "base/sequenced_task_runner.h"
#include <typeinfo>

namespace CppProcess {

    class CustomThreadImpl : public CustomThread,
                                            public base::Thread {
    public:
        // Construct a CustomThreadImpl with the supplied identifier.  It is an error
        // to construct a CustomThreadImpl that already exists.
        explicit CustomThreadImpl(CustomThread::ID identifier);

        // Special constructor for the main thread and unittests. If a
        // |message_loop| is provied, we use a dummy thread here since the main
        // thread already exists.
        CustomThreadImpl(CustomThread::ID identifier,
                         base::MessageLoop *message_loop);

        ~CustomThreadImpl() override;

        bool StartWithOptions(const Options &options);


    protected:
        void Init() override;

        void Run(base::MessageLoop *message_loop) override;

        void CleanUp() override;

    private:
        // We implement all the functionality of the public GodeyeThread
        // functions, but state is stored in the CustomThreadImpl to keep
        // the API cleaner. Therefore make GodeyeThread a friend class.
        friend class CustomThread;

        // The following are unique function names that makes it possible to tell
        // the thread id from the callstack alone in crash dumps.
        void CustomProcess2ThreadRun(base::MessageLoop *message_loop);

        void UploadThreadRun(base::MessageLoop *message_loop);

        void ProcessThreadRun(base::MessageLoop *message_loop);

        void TimerThreadRun(base::MessageLoop *message_loop);

        void KafkaThreadRun(base::MessageLoop *message_loop);

        void SyncThreadRun(base::MessageLoop *message_loop);

        void CustomProcess1ThreadRun(base::MessageLoop *message_loop);

        void WSInitProcessThreadRun(base::MessageLoop *message_loop);

        void WSRemoveThreadRun(base::MessageLoop *message_loop);


        static bool PostTaskHelper(
            CustomThread::ID identifier,
            const tracked_objects::Location &from_here,
            const base::Closure &task,
            base::TimeDelta delay,
            bool nestable);

        // Common initialization code for the constructors.
        void Initialize();

        // The identifier of this thread.  Only one thread can exist with a given
        // identifier at a given time.
        ID identifier_;
    };

}//end namespace CppPrcess

#endif  // CONTENT_GODEYE_GODEYE_THREAD_IMPL_H_