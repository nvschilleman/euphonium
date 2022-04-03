#include "BluetoothPlugin.h"
#include "portaudio.h"

#define SAMPLE_RATE       (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_CHANNELS      (2)
/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG       (0) /**/

#define PA_SAMPLE_TYPE    paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE    (0)

// @TODO Refactor all of this to proper C++. Global shared_ptr is not ideal.
std::shared_ptr<BluetoothPlugin> mainBluetoothPlugin;

int inputCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData ) {

    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;

    uint8_t *sample = (uint8_t *)rptr;

    mainAudioBuffer->write(sample, 4*framesPerBuffer);
    return paContinue;
}

/*
static void sink_data_handler(const uint8_t *data, uint32_t len) {
    if (mainBluetoothPlugin->status == ModuleStatus::RUNNING) {
        size_t bytesWritten = 0;
        while (bytesWritten < len) {
            bytesWritten +=
                mainAudioBuffer->write(data + bytesWritten, len - bytesWritten);
        }
    }
}

#define BT_SINK_AUDIO_STARTED 1
#define BT_SINK_AUDIO_STOPPED 2
#define BT_SINK_PLAY 3
#define BT_SINK_STOP 4
#define BT_SINK_PAUSE 5
#define BT_SINK_RATE 6
#define BT_SINK_METADATA 7
#define BT_SINK_VOLUME 8

static bool bt_sink_cmd_handler(bt_sink_cmd_t cmd, va_list args) {
    switch (cmd) {
    case BT_SINK_AUDIO_STARTED: {

        //mainAudioBuffer->lockAccess();
        BELL_LOG(info, "bluetooth", "Audio sink started");
        mainBluetoothPlugin->setStatus(ModuleStatus::RUNNING);
        break;
    }
    case BT_SINK_AUDIO_STOPPED: {
        BELL_LOG(info, "bluetooth", "Audio sink stopped");

        break;
    }
    case BT_SINK_PLAY: {
        auto event = std::make_unique<PauseChangedEvent>(false);
        mainEventBus->postEvent(std::move(event));
        // LOG_INFO("BT playing");
        break;
    }
    case BT_SINK_STOP:

        BELL_LOG(info, "bluetooth", "BT stopped");
        break;
    case BT_SINK_PAUSE: {
        auto event = std::make_unique<PauseChangedEvent>(true);
        mainEventBus->postEvent(std::move(event));
        // LOG_INFO("BT paused, just silence");
        break;
    }
    case BT_SINK_RATE: {
        uint32_t sampleRate = va_arg(args, u32_t);
        mainBluetoothPlugin->setStatus(ModuleStatus::RUNNING);
        BELL_LOG(info, "bluetooth", "Sample rate changed to %d", sampleRate);
        mainAudioBuffer->configureOutput(AudioOutput::SampleFormat::INT16,
                                         sampleRate);
        break;
    }
    case BT_SINK_METADATA: {
        char *artist = va_arg(args, char *);
        char *album = va_arg(args, char *);
        char *title = va_arg(args, char *);
        auto event = std::make_unique<SongChangedEvent>(
            std::string(title), std::string(album), std::string(artist),
            "bluetooth", "https://i.imgur.com/Fuu73lv.png");
        EUPH_LOG(info, "bluetooth", "Song name changed");
        mainEventBus->postEvent(std::move(event));

        break;
    }
    case BT_SINK_VOLUME: {
        EUPH_LOG(info, "bluetooth", "Volume changed");
        u32_t volume = va_arg(args, u32_t);
        volume = 100 * powf(volume / 128.0f, 3);
        auto event = std::make_unique<VolumeChangedEvent>(volume);
        EUPH_LOG(info, "bluetooth", "Volume changed");
        mainEventBus->postEvent(std::move(event));
        break;
    }
    default:
        break;
    }

    return true;
}
*/
BluetoothPlugin::BluetoothPlugin() : bell::Task("bt_euph", 4 * 1024, 3, 0, false) {
    name = "bluetooth";
//    auto event = std::make_unique<PauseChangedEvent>(false);
//    mainEventBus->postEvent(std::move(event));
//    mainBluetoothPlugin->setStatus(ModuleStatus::RUNNING);
    // bt_sink_init(bt_sink_cmd_handler, sink_data_handler);
}

void closeStream(PaStream* stream){
    if(Pa_IsStreamActive(stream)) {
        Pa_CloseStream(stream);
    }
}

void BluetoothPlugin::shutdown() {
    EUPH_LOG(info, "bluetooth", "Shutting down...");
    //mainAudioBuffer->unlockAccess();
    setStatus(ModuleStatus::SHUTDOWN);
}

void BluetoothPlugin::runTask() {
    BTEvent event;

    PaStreamParameters  inputParameters;
    PaStream*           stream;

    while (true) {
        if(1){
            event = BTEvent::LockAccess;
//        if (this->btEventQueue.wpop(event)) {
            if (event == BTEvent::Initialize) {
                std::string deviceName = std::any_cast<std::string>(config["name"]);
//                snprintf(bt_sink_name, sizeof bt_sink_name, "%s", deviceName.c_str());
//                bt_sink_init(bt_sink_cmd_handler, sink_data_handler);
            }

            if (event == BTEvent::Deinitialize) {
//                bt_sink_deinit();
            }

            if (event == BTEvent::Disconnect) {
                BELL_LOG(info, "bluetooth", "Disconnecting...");
                closeStream(stream);
//                bt_disconnect();
                BELL_SLEEP_MS(1500);
                //bt_sink_deinit();
                mainAudioBuffer->unlockAccess();
                status = ModuleStatus::SHUTDOWN;
                //bt_sink_init(bt_sink_cmd_handler, sink_data_handler);
            }

            if (event == BTEvent::LockAccess) {
                // get bluetooth input device
                bool device_found = false;
                for (int i = 0; i < Pa_GetDeviceCount(); i++) {
                    const PaDeviceInfo *deviceInfo;
                    deviceInfo = Pa_GetDeviceInfo(i);
                    if (deviceInfo->maxInputChannels > 0) {
                        if(strcmp(deviceInfo->name, "bluealsa") == 0){
                            inputParameters.device = i;
                            device_found = true;
                        }
                    }
                }

                if(!device_found){
                    BELL_LOG(error, "bluetooth", "Bluetooth input sink not found");
                    return;
                }
                inputParameters.channelCount = 2;                    /* stereo input */
                inputParameters.sampleFormat = PA_SAMPLE_TYPE;
                inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
                inputParameters.hostApiSpecificStreamInfo = NULL;

                Pa_OpenStream(
                      &stream,
                      &inputParameters,
                      NULL,                  /* &outputParameters, */
                      SAMPLE_RATE,
                      FRAMES_PER_BUFFER,
                      paClipOff,      /* we won't output out of range samples so don't bother clipping them */
                      inputCallback,
                      NULL);

                Pa_StartStream( stream );

                BELL_LOG(info, "bluetooth", "Locking access...");
                mainAudioBuffer->shutdownExcept(name);
                mainAudioBuffer->configureOutput(AudioOutput::SampleFormat::INT16,
                                                 44100);
                mainAudioBuffer->lockAccess();
                status = ModuleStatus::RUNNING;
            }

        } else {
            BELL_SLEEP_MS(1000);
        }
    }
}

void BluetoothPlugin::startAudioThread() {
    startTask();
    btEventQueue.push(BTEvent::Initialize);
}

void BluetoothPlugin::setStatus(ModuleStatus status) {
    if (this->status == status) return;
    this->status = status;
    if (status == ModuleStatus::SHUTDOWN) {
        btEventQueue.push(BTEvent::Disconnect);
    } else {
        btEventQueue.push(BTEvent::LockAccess);
    }
}

void BluetoothPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
    scriptLoader->loadScript("esp32/bluetooth", berry);
}

void BluetoothPlugin::setupBindings() {}
