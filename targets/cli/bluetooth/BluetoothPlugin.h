#ifndef EUPHONIUM_LINUX_BLUETOOTH_H
#define EUPHONIUM_LINUX_BLUETOOTH_H

#include "Core.h"
#include "CoreEvents.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "Module.h"
#include "Queue.h"
#include "ScriptLoader.h"
#include "Task.h"
#include <memory>

//extern "C" {
//#include "bt_app_sink.h"
//}

class BluetoothPlugin : public bell::Task, public Module {

  private:
    enum class BTEvent {
        Initialize,
        Deinitialize,
        Disconnect,
        LockAccess,
    };
    bell::Queue<BTEvent> btEventQueue;

  public:
    BluetoothPlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void shutdown();
    void configurationUpdated(){};
    void startAudioThread();
    void runTask();
    void setStatus(ModuleStatus status);
};

extern std::shared_ptr<BluetoothPlugin> mainBluetoothPlugin;

#endif
