#ifndef CONTENT_PUBLIC_GODEYE_GODEYE_THREAD_H_
#define CONTENT_PUBLIC_GODEYE_GODEYE_THREAD_H_

#include <string>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/single_thread_task_runner.h"
#include "base/task_runner_util.h"
#include "base/time/time.h"
#include "base/sequenced_task_runner.h"

namespace base {
class MessageLoop;
class SequencedWorkerPool;
class Thread;
}


namespace CppProcess {

    class CustomThreadDelegate;
    class CustomThreadImpl;

///////////////////////////////////////////////////////////////////////////////
// GodeyeThread
//
// Utility functions for threads that are known by a program-wide
// name.  For example, there is one IO thread for the entire
// process, and various pieces of code find it useful to retrieve a
// pointer to the IO thread's message loop.
//
// Invoke a task by thread ID:
//
//   GodeyeThread::PostTask(GodeyeThread::IO, FROM_HERE, task);
//
// The return value is false if the task couldn't be posted because the target
// thread doesn't exist.  If this could lead to data loss, you need to check the
// result and restructure the code to ensure it doesn't occur.
//
// This class automatically handles the lifetime of different threads.
// It's always safe to call PostTask on any thread.  If it's not yet created,
// the task is deleted.  There are no race conditions.  If the thread that the
// task is posted to is guaranteed to outlive the current thread, then no locks
// are used.  You should never need to cache pointers to MessageLoops, since
// they're not thread safe.
    class CustomThread {
    public:
        // An enumeration of the well-known threads.
        // NOTE: threads must be listed in the order of their life-time, with each
        // thread outliving every other thread below it.
        enum ID {
            // This is the thread that interacts with the database.
                Upload,

            // This is the thread that interacts with the file system.
                Process,

            // This is the thread to handle slow HTTP cache operations.
                Timer,

            //kafka
                Kafka,

            //Sync线程
                Sync,

            //WARNING 需要添加单个线程时，需要放在Sync之前

            // 线程组1
                CustomProcess1,

            // 线程组2
                CustomProcess2,

            //ws 初始化连接线程组
                WSInitProcess,

            //ws 关闭链接线程组
                WSRemove,

            // This identifier does not represent a thread.  Instead it counts the
            // number of well-known threads.  Insert new well-known threads before this
            // identifier.
            // NOTE: do not add new threads here that are only used by a small number of
            // files. Instead you should just use a Thread class and pass its
            // task runner around. Named threads there are only for threads that
            // are used in many places.
                ID_COUNT
        };

        // These are the same methods in message_loop.h, but are guaranteed to either
        // get posted to the MessageLoop if it's still alive, or be deleted otherwise.
        // They return true iff the thread existed and the task was posted.  Note that
        // even if the task is posted, there's no guarantee that it will run, since
        // the target thread may already have a Quit message in its queue.
        static bool PostTask(ID identifier,
                             const tracked_objects::Location &from_here,
                             const base::Closure &task);

        static bool PostDelayedTask(ID identifier,
                                    const tracked_objects::Location &from_here,
                                    const base::Closure &task,
                                    base::TimeDelta delay);

        static bool PostNonNestableTask(ID identifier,
                                        const tracked_objects::Location &from_here,
                                        const base::Closure &task);

        static bool PostNonNestableDelayedTask(
            ID identifier,
            const tracked_objects::Location &from_here,
            const base::Closure &task,
            base::TimeDelta delay);

        //获取线程组当前活跃的线程数
        static int GetThreadPoolWorkingNum(ID thread_id);

        //设置当前工作时可使用的最大线程,根据情况增加，但不减少线程数
        static void SetThreadPoolWorkingMaxNum(ID threadID, size_t threadNum);

        // Callable on any thread.  Returns whether the given well-known thread is
        // initialized.
        static bool IsThreadInitialized(ID identifier) WARN_UNUSED_RESULT;

        // Callable on any thread.  Returns whether you're currently on a particular
        // thread.  To DCHECK this, use the DCHECK_CURRENTLY_ON() macro above.
        static bool CurrentlyOn(ID identifier) WARN_UNUSED_RESULT;

        // If the current message loop is one of the known threads, returns true and
        // sets identifier to its ID.  Otherwise returns false.
        static bool GetCurrentThreadIdentifier(ID *identifier) WARN_UNUSED_RESULT;

        //initialize the threads of Godeye
        static void InitGlobal();

        //get the name of this thread
        static std::string GetCurrentThreadName();


        // Sets the delegate for the specified GodeyeThread.
        //
        // Only one delegate may be registered at a time.  Delegates may be
        // unregistered by providing a nullptr pointer.
        //
        // If the caller unregisters a delegate before CleanUp has been
        // called, it must perform its own locking to ensure the delegate is
        // not deleted while unregistering.
        static void SetDelegate(ID identifier, CustomThreadDelegate *delegate);

    private:
        friend class CustomThreadImpl;

    public:
        CustomThread() {}

    private:
        DISALLOW_COPY_AND_ASSIGN(CustomThread);
    };

}//end namespace CppProccess

#endif  // CONTENT_PUBLIC_GODEYE_GODEYE_THREAD_H_
