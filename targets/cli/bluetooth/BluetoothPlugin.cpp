#include "BluetoothPlugin.h"
#include "portaudio.h"
#include <dbus/dbus.h>

#define SAMPLE_RATE       (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_CHANNELS      (2)
#define PA_SAMPLE_TYPE    paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE    (0)

// @TODO Refactor all of this to proper C++. Global shared_ptr is not ideal.
std::shared_ptr<BluetoothPlugin> mainBluetoothPlugin;

static bool dbus_message_iter_get_basic_boolean(DBusMessageIter *iter) {
	dbus_bool_t tmp = FALSE;
	return dbus_message_iter_get_basic(iter, &tmp), tmp;
}

static unsigned int dbus_message_iter_get_basic_integer(DBusMessageIter *iter) {
	dbus_uint32_t tmp = 0;
	return dbus_message_iter_get_basic(iter, &tmp), tmp;
}

static const char *dbus_message_iter_get_basic_string(DBusMessageIter *iter) {
	const char *tmp = "";
	return dbus_message_iter_get_basic(iter, &tmp), tmp;
}

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

    PaStreamParameters inputParameters;
    PaStream* stream;

    DBusError err;
    DBusConnection* conn = NULL;
    int ret;

    while (true) {
        if (this->btEventQueue.wtpop(event, 100)) {
            if (event == BTEvent::Initialize) {
                // initialise the errors
                dbus_error_init( &err );
                // connect to the bus
                conn = dbus_bus_get( DBUS_BUS_SYSTEM, &err );
                if ( dbus_error_is_set( &err ) ) {
                    BELL_LOG(error, "bluetooth", "DBUS connection error");
                    dbus_error_free( &err );
                }
                if ( NULL == conn ) {
                    continue;
                }

                // add a rule for which messages we want to see
                dbus_bus_add_match( conn,
                      "type='signal',interface='org.freedesktop.DBus.Properties'",
                      &err ); // see signals from the given interface
                dbus_connection_flush( conn );
                if ( dbus_error_is_set( &err ) ) {
                    BELL_LOG(error, "bluetooth", "DBUS Match error");
                }
            }

            if (event == BTEvent::Deinitialize) {
//                bt_sink_deinit();
            }

            if (event == BTEvent::Disconnect) {
                BELL_LOG(info, "bluetooth", "Disconnecting...");
                closeStream(stream);
                Pa_Terminate();Pa_Initialize();
                BELL_SLEEP_MS(1500);
                mainAudioBuffer->unlockAccess();
                status = ModuleStatus::SHUTDOWN;
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
                    btEventQueue.push(BTEvent::LockAccess);
                    Pa_Terminate();Pa_Initialize();
                    BELL_SLEEP_MS(2000);
                    continue;
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

        }
        if(conn != NULL) {
            // non blocking read of the next available message
            dbus_connection_read_write( conn, 0 );
            DBusMessage *msg = dbus_connection_pop_message( conn );
            // loop again if we haven't read a message
            if(msg != NULL ) {
                DBusMessageIter iter;
                dbus_message_iter_init(msg, &iter);
                if(dbus_message_iter_has_next(&iter)){
                    dbus_message_iter_next(&iter);
                    if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_ARRAY){
                        DBusMessageIter iter_array;
                        dbus_message_iter_recurse(&iter, &iter_array);
                        if(dbus_message_iter_get_arg_type(&iter_array) == DBUS_TYPE_DICT_ENTRY){
                            DBusMessageIter iter_entry;
                            DBusMessageIter iter_entry_val;
                            const char *key;
                            dbus_message_iter_recurse(&iter_array, &iter_entry);
                            dbus_message_iter_get_basic(&iter_entry, &key);
                            dbus_message_iter_next(&iter_entry);
                            dbus_message_iter_recurse(&iter_entry, &iter_entry_val);
                            if(dbus_message_iter_get_arg_type(&iter_entry_val) == DBUS_TYPE_STRING) {
                                if(strcmp(key, "Status") == 0){
                                    if(strcmp(dbus_message_iter_get_basic_string(&iter_entry_val), "playing") == 0){
                                        auto event = std::make_unique<PauseChangedEvent>(false);
                                        mainEventBus->postEvent(std::move(event));
                                    }else if(strcmp(dbus_message_iter_get_basic_string(&iter_entry_val), "paused") == 0){
                                        auto event = std::make_unique<PauseChangedEvent>(true);
                                        mainEventBus->postEvent(std::move(event));
                                    }
                                }
                            } else if(dbus_message_iter_get_arg_type(&iter_entry_val) == DBUS_TYPE_UINT16){
                                if(strcmp(key, "Volume") == 0){
                                    uint16_t val;
                                    dbus_message_iter_get_basic(&iter_entry_val, &val);
                                    printf("Vol: %d\n", val);
                                }
                            } else if(dbus_message_iter_get_arg_type(&iter_entry_val) == DBUS_TYPE_BOOLEAN){
                                if(strcmp(key, "Connected") == 0){
                                    bool val;
                                    dbus_message_iter_get_basic(&iter_entry_val, &val);
                                    if(val == true){
                                        mainBluetoothPlugin->setStatus(ModuleStatus::RUNNING);
                                    } else {
                                        mainBluetoothPlugin->setStatus(ModuleStatus::SHUTDOWN);
                                        btEventQueue.push(BTEvent::Disconnect);
                                    }
                                    printf("Connected: %d\n", val);
                                }
                            }
                        }
                    }
                }
                // free the message
                dbus_message_unref( msg );
            }
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
