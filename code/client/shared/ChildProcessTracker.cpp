#include <StdInc.h>
#include <mutex>

namespace childproc
{
namespace
{
        HANDLE g_childProcessJob = nullptr;
        std::once_flag g_childProcessJobInit;

        void EnsureJobObject()
        {
                g_childProcessJob = CreateJobObjectW(nullptr, nullptr);

                if (!g_childProcessJob)
                {
                        trace("childproc: failed to create tracking job object (err=%u)\n", GetLastError());
                        return;
                }

                JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfo = {};
                jobInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

                if (!SetInformationJobObject(g_childProcessJob, JobObjectExtendedLimitInformation, &jobInfo, sizeof(jobInfo)))
                {
                        trace("childproc: failed to configure tracking job (err=%u)\n", GetLastError());
                        CloseHandle(g_childProcessJob);
                        g_childProcessJob = nullptr;
                }
        }
}

        void TrackProcess(HANDLE processHandle, const char* tag)
        {
                if (!processHandle || processHandle == INVALID_HANDLE_VALUE)
                {
                        return;
                }

                std::call_once(g_childProcessJobInit, EnsureJobObject);

                if (!g_childProcessJob)
                {
                        return;
                }

                if (!AssignProcessToJobObject(g_childProcessJob, processHandle))
                {
                        trace("childproc: failed to track %s (pid=%lu, err=%u)\n",
                                (tag) ? tag : "child process",
                                GetProcessId(processHandle),
                                GetLastError());
                        return;
                }

                trace("childproc: tracking %s (pid=%lu)\n",
                        (tag) ? tag : "child process",
                        GetProcessId(processHandle));
        }
}
