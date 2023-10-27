#ifndef CONTENT_PUBLIC_BROWSER_BROWSER_THREAD_DELEGATE_H_

#define CONTENT_PUBLIC_BROWSER_BROWSER_THREAD_DELEGATE_H_


// A Delegate for content embedders to perform extra initialization/cleanup on

namespace CppProcess {

    class CustomThreadDelegate {

    public:

        virtual ~CustomThreadDelegate() = default;

        // Called prior to completing initialization of GodeyeThread::IO.

        virtual void Init() = 0;

        // Called during teardown of GodeyeThread::IO.

        virtual void CleanUp() = 0;

    };

}// end namespace CppProcess
#endif  // CONTENT_PUBLIC_BROWSER_BROWSER_THREAD_DELEGATE_H_