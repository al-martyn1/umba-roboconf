if exist %~dp0\tracy goto PULL_TRACY
git clone https://github.com/wolfpld/tracy.git  %~dp0\tracy
goto SWITCH_TO_MASTER

:PULL_TRACY
git -C %~dp0\tracy checkout master
git -C %~dp0\tracy pull

:SWITCH_TO_MASTER
git -C %~dp0\tracy switch master
@rem 41fc2930434bb2296cc8c255f82bcb731f8c0f06
git -C %~dp0\tracy checkout 3601576b3fcdef4c8f9d6c242a0d567369dc9c32

@rem https://stackoverflow.com/questions/10228760/how-do-i-fix-a-git-detached-head
@rem git checkout master will get you back on the master branch. If you wanted to clear out any working copy changes, you probably wanted to do git reset --hard.
@rem https://stackoverflow.com/questions/3965676/why-did-my-git-repo-enter-a-detached-head-state
@rem 


if not exist %~dp0\tracy goto NO_TRACY
echo start code . > %~dp0\tracy\_start_code.bat

:NO_TRACY


@rem tracy\public\common\TracyProtocol.hpp

@rem Protocol version 64
@rem Tracy ersions - https://crates.io/crates/tracy-client
@rem                 https://github.com/nagisa/rust_tracy_client

@rem Revision: 41fc2930434bb2296cc8c255f82bcb731f8c0f06
@rem Author: Ivan Molodetskikh <yalterz@gmail.com>
@rem Date: 22.10.2023 7:03:58
@rem Message:
@rem Add GpuTimeSync event
@rem 
@rem Allows to resynchronise GPU and CPU timestamps during profiling.
@rem ----
@rem Modified: public/client/TracyProfiler.cpp
@rem Modified: public/common/TracyProtocol.hpp
@rem Modified: public/common/TracyQueue.hpp
@rem Modified: public/tracy/TracyC.h
@rem Modified: server/TracyEventDebug.cpp
@rem Modified: server/TracyWorker.cpp
@rem Modified: server/TracyWorker.hpp
@rem 
