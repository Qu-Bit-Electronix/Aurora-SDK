/** Aurora Hardware Support File */
#pragma once
#ifndef AURORA_HW_H
#define AURORA_HW_H

#include "daisy_seed.h"
#include "fatfs.h"

#define DTCMRAM __attribute__((section(".dtcmram_bss")))
#define D2RAM __attribute__((section(".d2_bss")))
#define D2RAM2 __attribute__((section(".d2r2_bss")))
#define ITCMRAM __attribute__((section(".itcmram_bss")))

namespace aurora
{

/** @brief const used internally within Aurora Firmware to manage buffer memory 
 *  This can be safely exceeded in custom firmware.
 */
static constexpr int kMaxBlockSize = 96;

/** @brief Global buffers for the LED driver
 *  Non-cached for DMA usage.
 */
static daisy::LedDriverPca9685<2, true>::DmaBuffer DMA_BUFFER_MEM_SECTION
    led_dma_buffer_a,
    led_dma_buffer_b;

/** @brief global USB host handle accessor 
 *  This is left global so that it is guaranteed to be within the AXI SRAM
 *  with the aurora_sram.lds linker file.
 */
daisy::USBHostHandle usb;

/** @brief global accessor to the FatFS interface. 
 *  This is left global so that it is guaranteed to be within the AXI SRAM
 *  with the aurora_sram.lds linker file.
 */
daisy::FatFSInterface fatfs_interface;

/** @brief indexed accessors for knob controls 
 *  Example usage:
 *  float val = hw.GetKnobValue(KNOB_TIME);
 */
enum ControlKnobs
{
    KNOB_TIME,
    KNOB_REFLECT,
    KNOB_MIX,
    KNOB_ATMOSPHERE,
    KNOB_BLUR,
    KNOB_WARP,
    KNOB_LAST,
};

/** @brief indexed accessors for CV controls 
 *  Example usage:
 *  float val = hw.GetCvValue(CV_ATMOSPHERE);
 */
enum ControlCVs
{
    CV_ATMOSPHERE,
    CV_TIME,
    CV_MIX,
    CV_REFLECT,
    CV_BLUR,
    CV_WARP,
    CV_LAST,
};

/** @brief indexed accessors for momentary switches 
 *  Example usage:
 *  bool state = hw.GetSwitch(SW_FREEZE).Pressed();
 */
enum Switches
{
    SW_FREEZE,
    SW_REVERSE,
    SW_SHIFT,
    SW_LAST,
};

/** @brief indexed accessors for momentary switches 
 *  Example usage:
 *  bool trig = hw.GetGateTrig(GATE_FREEZE);
 */
enum Gates
{
    GATE_FREEZE,
    GATE_REVERSE,
    GATE_LAST,
};


/** @brief indexed accessors for RGB LEDs 
 *  Example usage:
 *  hw.SetLed(LED_FREEZE, 0.f, 0.f, 1.f);
 */
enum Leds
{
    LED_REVERSE,
    LED_FREEZE,
    LED_1,
    LED_2,
    LED_3,
    LED_4,
    LED_5,
    LED_6,
    LED_BOT_1,
    LED_BOT_2,
    LED_BOT_3,
    LED_LAST,
};

/** @brief Calibration data container for Aurora 
 *  This data is calibrated from the Qu-Bit default Aurora firmware.
 *  It is advised not to save over this data unless you are prepared to recalibrate.
*/
struct CalibrationData
{
    CalibrationData() : warp_scale(60.f), warp_offset(0.f), cv_offset{0.f} {}
    float warp_scale, warp_offset;
    float cv_offset[CV_LAST];

    /** @brief checks sameness */
    bool operator==(const CalibrationData &rhs)
    {
        if(warp_scale != rhs.warp_scale)
        {
            return false;
        }
        else if(warp_offset != rhs.warp_offset)
        {
            return false;
        }
        else
        {
            for(int i = 0; i < CV_LAST; i++)
            {
                if(cv_offset[i] != rhs.cv_offset[i])
                    return false;
            }
        }
        return true;
    }

    /** @brief Not equal operator */
    bool operator!=(const CalibrationData &rhs) { return !operator==(rhs); }
};

/** @brief Hardware support class for the Qu-Bit Aurora 
 *  This should be created, and intitialized at the beginning of any program
 *  before running anything else.
 * 
 *  If you've used the Daisy Seed, etc. before this object takes the place
 *  of the core "DaisySeed" or other board support objects.
*/
class Hardware
{
  public:
    /** @brief Empty Constructor 
     *  Call `Init` from main to initialize
    */
    Hardware() {}

    /** @brief Empty Destructor
     *  This object should span the life of the program
     */
    ~Hardware() {}

    /** @brief Initialize the hardware. 
     *  Call this function at the start of main()
     * 
     *  @param boost true sets the processor to run at the maximum 480MHz, 
     *               false sets the processor to run at 400MHz.
     *               Defaults to true (480MHz).
     */
    void Init(bool boost = true)
    {
        seed.Init(boost);
        hw_version = GetBoardRevision();
        ConfigureAudio();
        ConfigureControls();
        ConfigureLeds();
        seed.adc.Start();
        SetAudioSampleRate(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);
        SetAudioBlockSize(96);
        UpdateHidRates();

        LoadCalibrationData();
        cal_save_flag_ = false;
        for(int i = 0; i < CV_LAST; i++)
        {
            cv_offsets_[i] = 0.f;
        }
    }

    /** @brief delay function; same as System::Delay(), and should not be called 
     *         from any interrupt callbacks (LowPriorityCallback/AudioCallback).
     * 
     *  @param del number of milliseconds to delay
    */
    void DelayMs(size_t del) { seed.DelayMs(del); }

    /** This starts up a callback that is on the lowest priority interrupt level
     *  This provides an area for non-background tasks that should interrupt low
     *  level activity like diskio.
     *
     *  @param cb callback to take place at target frequency
     *  @param target_freq freq in hz that the callback should take place.
     *  @param data any data to send through callback; this defaults to nullptr
     */
    void StartLowPriorityCallback(daisy::TimerHandle::PeriodElapsedCallback cb,
                                  uint32_t target_freq,
                                  void    *data = nullptr)
    {
        daisy::TimerHandle::Config timcfg;
        timcfg.periph        = daisy::TimerHandle::Config::Peripheral::TIM_5;
        timcfg.dir           = daisy::TimerHandle::Config::CounterDir::UP;
        auto tim_base_freq   = daisy::System::GetPClk2Freq();
        auto tim_target_freq = target_freq;
        auto tim_period      = tim_base_freq / tim_target_freq;
        timcfg.period        = tim_period;
        timcfg.enable_irq    = true;
        tim5_handle.Init(timcfg);
        tim5_handle.SetCallback(cb, data);
        /** Start Audio */
        tim5_handle.Start();
    }

    /** @brief Starts a specified Interleaving audio callback */
    void StartAudio(daisy::AudioHandle::InterleavingAudioCallback cb)
    {
        seed.StartAudio(cb);
    }

    /** @brief Starts a specified audio callback 
     *         Data is non-interleaved 
     *         (i.e. {{L0, L1, ... , LN},{R0, R1, ... , RN}})
     */
    void StartAudio(daisy::AudioHandle::AudioCallback cb)
    {
        current_cb_ = cb;
        seed.StartAudio(cb);
    }

    /** @brief Changes current callback to a new interleaved callback */
    void ChangeAudioCallback(daisy::AudioHandle::InterleavingAudioCallback cb)
    {
        seed.ChangeAudioCallback(cb);
    }


    /** @brief Changes current callback to a new non-interleaved callback */
    void ChangeAudioCallback(daisy::AudioHandle::AudioCallback cb)
    {
        current_cb_ = cb;
        seed.ChangeAudioCallback(cb);
    }

    /** @brief Updates the samplerate to one of the allowed target samplerates.
     *         This function stops the audio completely, and will cause clicks.
     * 
     *  @param sr target samplerate in Hz. Allowed values are 16000, 32000, 48000, 96000
     *            any other value will fallback to 48kHz
     */
    void ChangeSampleRate(int sr)
    {
        daisy::SaiHandle::Config::SampleRate srval;
        size_t                               new_blocksize = 96;
        switch(sr)
        {
            case 16000:
                srval = daisy::SaiHandle::Config::SampleRate::SAI_16KHZ;
                break;
            case 32000:
                srval = daisy::SaiHandle::Config::SampleRate::SAI_32KHZ;
                break;
            case 48000:
                srval = daisy::SaiHandle::Config::SampleRate::SAI_48KHZ;
                break;
            case 96000:
                srval         = daisy::SaiHandle::Config::SampleRate::SAI_96KHZ;
                new_blocksize = 48;
                break;
            default:
                srval = daisy::SaiHandle::Config::SampleRate::SAI_48KHZ;
                break;
        }
        seed.StopAudio();
        seed.SetAudioSampleRate(srval);
        seed.SetAudioBlockSize(new_blocksize);
        seed.StartAudio(current_cb_);
    }

    /** @brief Stops Audio */
    void StopAudio() { seed.StopAudio(); }

    /** @brief sets the audio sample rate. Audio must be stopped for this to work properly 
     *  @param samplerate target samplerate in in daisy::SaiHandle::Config::SampleRate 
     */
    void SetAudioSampleRate(daisy::SaiHandle::Config::SampleRate samplerate)
    {
        seed.SetAudioSampleRate(samplerate);
    }

    /** @brief sets the audio sample rate. Audio must be stopped for this to work properly 
     *  @param samplerate target samplerate in Hz. Allowed values are 16000, 32000, 48000, 96000
     *            any other value will fallback to 48kHz
     */
    void SetAudioSampleRate(int samplerate)
    {
        daisy::SaiHandle::Config::SampleRate srval;
        switch(samplerate)
        {
            case 16000:
                srval = daisy::SaiHandle::Config::SampleRate::SAI_16KHZ;
                break;
            case 32000:
                srval = daisy::SaiHandle::Config::SampleRate::SAI_32KHZ;
                break;
            case 48000:
                srval = daisy::SaiHandle::Config::SampleRate::SAI_48KHZ;
                break;
            case 96000:
                srval = daisy::SaiHandle::Config::SampleRate::SAI_96KHZ;
                break;
            default:
                srval = daisy::SaiHandle::Config::SampleRate::SAI_48KHZ;
                break;
        }
        seed.SetAudioSampleRate(srval);
    }

    /** @brief returns the sample rate in Hz of the audio engine */
    float AudioSampleRate() { return seed.audio_handle.GetSampleRate(); }

    /** @brief sets the number of samples to process in each audio callback */
    void SetAudioBlockSize(size_t blocksize)
    {
        seed.SetAudioBlockSize(blocksize);
    }

    /** @brief returns the number of samples to process in each audio callback */
    size_t AudioBlockSize() { return seed.AudioBlockSize(); }

    /** @brief returns the rate in Hz that the audio callback gets called */
    float AudioCallbackRate() const { return seed.AudioCallbackRate(); }

    /** @brief sets the state of the LED on the daisy itself */
    void SetTestLed(bool state) { seed.SetLed(state); }

    /** @brief sets all RGB LEDs to off state 
     *  This can be called from the top of wherever LEDs are periodically set
     *  or within the UI framework's Canvas descriptor via the 
     *  clearFunction_ function.
     */
    void ClearLeds()
    {
        for(int i = 0; i < LED_LAST; i++)
        {
            SetLed((Leds)i, 0.0f, 0.0f, 0.0f);
        }
    }

    /** @brief Writes the state of all LEDs to the hardware 
     *  This can be called from a fixed interval in the main loop,
     *  or within the UI framework's Canvas descriptor via the 
     *  flushFunction_ function.
     */
    void WriteLeds() { led_driver_.SwapBuffersAndTransmit(); }

    /** @brief Sets the RGB value of a given LED 
     *  @param idx LED index (one of Leds enum above)
     *  @param r 0- 1 red value 
     *  @param g 0- 1 green value 
     *  @param b 0- 1 blue value 
    */
    void SetLed(Leds idx, float r, float g, float b)
    {
        LedIdx led = LedMap[idx];
        if(led.r != -1)
            led_driver_.SetLed(led.r, r);
        led_driver_.SetLed(led.g, g);
        led_driver_.SetLed(led.b, b);
    }

    /** @brief filters and debounces all control 
     *  This should be run once per audio callback.
     */
    void ProcessAllControls()
    {
        ProcessAnalogControls();
        ProcessDigitalControls();
    }

    /** @brief filters and debounces digital controls (switches)
     *  This is called from ProcessAllControls, and should be run once per audio callback
     */
    void ProcessDigitalControls()
    {
        for(int i = 0; i < SW_LAST; i++)
        {
            switches[i].Debounce();
        }
    }

    /** @brief filters all analog controls (knobs and CVs)
     *  This is called from ProcessAllControls, and should be run once per audio callback
     */
    void ProcessAnalogControls()
    {
        for(int i = 0; i < KNOB_LAST; i++)
        {
            controls[i].Process();
        }

        for(int i = 0; i < CV_LAST; i++)
        {
            cv[i].Process();
        }
    }
    /** @brief returns a 0-1 value for the given knob control
     *  @param ctrl knob index to read from. Should be one of ControlKnob (i.e. KNOB_TIME)
     */
    inline float GetKnobValue(int ctrl) const { return controls[ctrl].Value(); }

    /** @brief returns a reference to a given momentary switch 
     *         Example Usage:
     *         bool state = hw.GetButton(SW_FREEZE).Pressed();
     *  @param idx one of the Switches enum values
     */
    inline const daisy::Switch &GetButton(int idx) const
    {
        return switches[idx];
    }

    /** @brief returns true if the gate input just went high 
     *  This is expected to be checked only once per audio callback
     * 
     *  @param idx one of the Gates enum values
     */
    inline bool GetGateTrig(int idx) { return gates[idx].Trig(); }

    /** @brief returns true if the gate input is currently high 
     *  @param idx one of the Gates enum values
     */
    inline bool GetGateState(int idx) { return gates[idx].State(); }

    /** @brief Update HidRates for new Callback rate when samplerate/blocksize change */
    void UpdateHidRates()
    {
        for(int i = 0; i < CV_LAST; i++)
        {
            cv[i].SetSampleRate(AudioCallbackRate());
        }
        for(int i = 0; i < KNOB_LAST; i++)
        {
            controls[i].SetSampleRate(AudioCallbackRate());
        }
        for(int i = 0; i < SW_LAST; i++)
        {
            switches[i].SetUpdateRate(AudioCallbackRate());
        }
    }

    /** @brief starts the mounting process for USB Drive use if it is present */
    void PrepareMedia(
        daisy::USBHostHandle::ConnectCallback     connect_cb      = nullptr,
        daisy::USBHostHandle::DisconnectCallback  disconnect_cb   = nullptr,
        daisy::USBHostHandle::ClassActiveCallback class_active_cb = nullptr,
        daisy::USBHostHandle::ErrorCallback       error_cb        = nullptr,
        void                                     *userdata        = nullptr)
    {
        /** Initialize hardware and set user callbacks */
        daisy::USBHostHandle::Config usbcfg;
        usbcfg.connect_callback      = connect_cb;
        usbcfg.disconnect_callback   = disconnect_cb;
        usbcfg.class_active_callback = class_active_cb;
        usbcfg.error_callback        = error_cb;
        usbcfg.userdata              = userdata;
        usb.Init(usbcfg);

        /** Prepare FatFS -- fmount will defer until first attempt to read/write */
        daisy::FatFSInterface::Config fsi_cfg;
        fsi_cfg.media = daisy::FatFSInterface::Config::MEDIA_USB;
        fatfs_interface.Init(fsi_cfg);
        f_mount(&fatfs_interface.GetUSBFileSystem(),
                fatfs_interface.GetUSBPath(),
                0);
    }

    /** @brief Return a MIDI note number value from -60 to 60 corresponding to 
     *      the -5V to 5V input range of the Warp CV input.
     */
    inline float GetWarpVoct()
    {
        return voct_cal.ProcessInput(cv[CV_WARP].Value());
    }

    /** @brief  gets calibrated offset-adjusted CV Value from the hardware.
     *  @param  cv_idx index of the CV to read from.
     *  @return offset adjusted output (except for warp which is v/oct calibrated)
     *  @note   when returning warp CV from this function it will be with no calibrated
     *    offset, and is identical to reading from the AnalogControl itself.
     */
    inline float GetCvValue(int cv_idx)
    {
        if(cv_idx != CV_WARP)
            return cv[cv_idx].Value() - cv_offsets_[cv_idx];
        else
            return cv[cv_idx].Value();
    }

    /** @brief called during a customized calibration UI to record the 1V value */
    inline void CalibrateV1(float v1) { warp_v1_ = v1; }

    /** @brief called during a customized calibration UI to record the 3V value 
     *         and set that calibraiton has completed and can be saved. 
     */
    inline void CalibrateV3(float v3)
    {
        warp_v3_ = v3;
        voct_cal.Record(warp_v1_, warp_v3_);
        cal_save_flag_ = true;
    }

    /** @brief Sets the calibration data for 1V/Octave over Warp CV 
     *  typically set after reading stored data from external memory.
     */
    inline void SetWarpCalData(float scale, float offset)
    {
        voct_cal.SetData(scale, offset);
    }

    /** @brief Gets the current calibration data for 1V/Octave over Warp CV 
     *  typically used to prepare data for storing after successful calibration
     */
    inline void GetWarpCalData(float &scale, float &offset)
    {
        voct_cal.GetData(scale, offset);
    }

    /** @brief Sets the cv offset from an externally array of data */
    inline void SetCvOffsetData(float *data)
    {
        for(int i = 0; i < CV_LAST; i++)
        {
            cv_offsets_[i] = data[i];
        }
    }

    /** Fills an array with the offset data currently being used */
    inline void GetCvOffsetData(float *data)
    {
        for(int i = 0; i < CV_LAST; i++)
        {
            data[i] = cv_offsets_[i];
        }
    }

    /** @brief Checks to see if calibration has been completed and needs to be saved */
    inline bool ReadyToSaveCal() const { return cal_save_flag_; }

    /** @brief signal the cal-save flag to clear once calibration data has been written to ext. flash memory */
    inline void ClearSaveCalFlag() { cal_save_flag_ = false; }

    /** Array of CV inputs */
    daisy::AnalogControl cv[CV_LAST];

    /** Array of momentary switches */
    daisy::Switch switches[SW_LAST];

    /** Array of gate inputs */
    daisy::GateIn gates[GATE_LAST];

    /** Array of knob controls */
    daisy::AnalogControl controls[KNOB_LAST];

    /** Daisy Seed base object */
    daisy::DaisySeed seed;

  private:
    /** @brief Address offset where calibration data is stored in the Aurora 
     *  default firmware.
     *  This data is calibrated from the Qu-Bit default Aurora firmware.
     *  It is advised not to save over this data unless you are prepared to recalibrate.
    */
    static constexpr uint32_t kCalibrationDataOffset = 4096;

    /** @brief Internal struct for managing LED indices */
    struct LedIdx
    {
        int8_t r;
        int8_t g;
        int8_t b;
    };

    /** @brief internal mapping for LED routing on hardware */
    const LedIdx LedMap[LED_LAST] = {{0, 1, 2},
                                     {3, 4, 5},
                                     {6, 7, 8},
                                     {9, 10, 11},
                                     {16, 17, 18},
                                     {12, 13, 14},
                                     {22, 23, 24},
                                     {28, 29, 30},
                                     {-1, 20, 21},
                                     {-1, 26, 27},
                                     {-1, 19, 25}};

    /** @brief Specifiers for hardware revisions */
    enum class HardwareVersion
    {
        REV3, /**< Last version using normal Daisy Seed + PCM3060 */
        REV4, /**< s2dfm */
    };


    daisy::LedDriverPca9685<2, true> led_driver_;

    daisy::AudioHandle::AudioCallback current_cb_;

    daisy::TimerHandle tim5_handle;
    daisy::I2CHandle   i2c;
    daisy::Pcm3060     codec;

    HardwareVersion hw_version;

    /** Cal data */
    float                  warp_v1_, warp_v3_;
    daisy::VoctCalibration voct_cal;
    float                  cv_offsets_[CV_LAST];

    bool cal_save_flag_;


    void ConfigureAudio()
    {
        /** Only need to set up any specific audio when dealing with the rev3 hardware */
        if(hw_version == HardwareVersion::REV3)
        {
            daisy::I2CHandle::Config codec_i2c_config;
            codec_i2c_config.periph
                = daisy::I2CHandle::Config::Peripheral::I2C_1;
            codec_i2c_config.pin_config = {seed.GetPin(11), seed.GetPin(12)};
            codec_i2c_config.speed
                = daisy::I2CHandle::Config::Speed::I2C_400KHZ;
            codec_i2c_config.mode = daisy::I2CHandle::Config::Mode::I2C_MASTER;

            i2c.Init(codec_i2c_config);
            codec.Init(i2c);

            daisy::SaiHandle::Config sai_cfg;
            sai_cfg.periph    = daisy::SaiHandle::Config::Peripheral::SAI_2;
            sai_cfg.sr        = daisy::SaiHandle::Config::SampleRate::SAI_48KHZ;
            sai_cfg.bit_depth = daisy::SaiHandle::Config::BitDepth::SAI_24BIT;
            sai_cfg.a_sync    = daisy::SaiHandle::Config::Sync::SLAVE;
            sai_cfg.b_sync    = daisy::SaiHandle::Config::Sync::MASTER;
            sai_cfg.a_dir     = daisy::SaiHandle::Config::Direction::RECEIVE;
            sai_cfg.b_dir     = daisy::SaiHandle::Config::Direction::TRANSMIT;
            sai_cfg.pin_config.fs   = seed.GetPin(27);
            sai_cfg.pin_config.mclk = seed.GetPin(24);
            sai_cfg.pin_config.sck  = seed.GetPin(28);
            sai_cfg.pin_config.sa   = seed.GetPin(26);
            sai_cfg.pin_config.sb   = seed.GetPin(25);
            // Then Initialize
            daisy::SaiHandle sai_2_handle;
            sai_2_handle.Init(sai_cfg);
            daisy::AudioHandle::Config audio_cfg;
            audio_cfg.blocksize = 48;
            audio_cfg.samplerate
                = daisy::SaiHandle::Config::SampleRate::SAI_48KHZ;
            audio_cfg.postgain = 1.0f;
            seed.audio_handle.Init(audio_cfg, sai_2_handle);
        }
        else
        {
            /** Set 44.1kHz Deemp Filter pin on Daisy Seed2 DFM to off */
            daisy::GPIO deemp;
            daisy::Pin  deemp_pin(daisy::PORTB, 11);
            deemp.Init(deemp_pin, daisy::GPIO::Mode::OUTPUT);
            deemp.Write(0);
        }
    }

    // pots,switches, gates, CV
    void ConfigureControls()
    {
        if(hw_version == HardwareVersion::REV3)
        {
            // ===== CV and Pots (adc) =====
            daisy::AdcChannelConfig cfg[CV_LAST + 1];

            for(int i = 0; i < CV_LAST; i++)
            {
                cfg[i].InitSingle(seed.GetPin(17 + i));
            }
            cfg[CV_LAST].InitMux(seed.GetPin(16),
                                 6,
                                 seed.GetPin(7),
                                 seed.GetPin(8),
                                 seed.GetPin(9));
            seed.adc.Init(cfg, CV_LAST + 1);

            // init cv as bipolar analog controls
            for(size_t i = 0; i < CV_LAST; i++)
            {
                cv[i].InitBipolarCv(seed.adc.GetPtr(i), AudioCallbackRate());
            }

            // init pots as analog controls
            for(size_t i = 0; i < KNOB_LAST; i++)
            {
                controls[i].Init(seed.adc.GetMuxPtr(CV_LAST, i),
                                 AudioCallbackRate());
            }

            // ===== gates =====
            dsy_gpio_pin pin;
            pin = seed.GetPin(23);
            gates[GATE_FREEZE].Init(&pin);
        }
        else
        {
            /** For the new version we have a different ADC layout */
            daisy::AdcChannelConfig cfg[CV_LAST + KNOB_LAST];
            /** Read CVs then Pots for best consistency with last version */
            cfg[CV_LAST + KNOB_TIME].InitSingle(daisy::seed::A0);
            cfg[CV_LAST + KNOB_REFLECT].InitSingle(daisy::seed::A1);
            cfg[CV_ATMOSPHERE].InitSingle(daisy::seed::A2);
            cfg[CV_TIME].InitSingle(daisy::seed::A3);
            cfg[CV_MIX].InitSingle(daisy::seed::A4);
            cfg[CV_REFLECT].InitSingle(daisy::seed::A5);
            cfg[CV_BLUR].InitSingle(daisy::seed::A6);
            cfg[CV_WARP].InitSingle(daisy::seed::A7);
            cfg[CV_LAST + KNOB_MIX].InitSingle(daisy::seed::A8);
            cfg[CV_LAST + KNOB_WARP].InitSingle(daisy::seed::A9);
            cfg[CV_LAST + KNOB_ATMOSPHERE].InitSingle(daisy::seed::A10);
            cfg[CV_LAST + KNOB_BLUR].InitSingle(daisy::seed::A11);
            seed.adc.Init(cfg, CV_LAST + KNOB_LAST);

            for(size_t i = 0; i < CV_LAST; i++)
            {
                cv[i].InitBipolarCv(seed.adc.GetPtr(i), AudioCallbackRate());
            }
            // init pots as analog controls with offset to last CV for ptr
            for(size_t i = 0; i < KNOB_LAST; i++)
            {
                controls[i].Init(seed.adc.GetPtr(CV_LAST + i),
                                 AudioCallbackRate());
            }
            dsy_gpio_pin freeze_pin = daisy::seed::D26;
            gates[GATE_FREEZE].Init(&freeze_pin);
        }
        // And anything generic to both versions
        // ===== switches =====
        switches[SW_FREEZE].Init(seed.GetPin(1), AudioCallbackRate());
        switches[SW_REVERSE].Init(seed.GetPin(10), AudioCallbackRate());
        switches[SW_SHIFT].Init(seed.GetPin(13), AudioCallbackRate());

        dsy_gpio_pin revpin = seed.GetPin(14);
        gates[GATE_REVERSE].Init(&revpin);
    }

    void ConfigureLeds()
    {
        // reinit i2c at 1MHz
        daisy::I2CHandle::Config codec_i2c_config;
        codec_i2c_config.periph = daisy::I2CHandle::Config::Peripheral::I2C_1;
        codec_i2c_config.pin_config = {seed.GetPin(11), seed.GetPin(12)};
        codec_i2c_config.speed      = daisy::I2CHandle::Config::Speed::I2C_1MHZ;
        codec_i2c_config.mode = daisy::I2CHandle::Config::Mode::I2C_MASTER;
        i2c.Init(codec_i2c_config);

        // init driver
        led_driver_.Init(i2c, {0x00, 0x01}, led_dma_buffer_a, led_dma_buffer_b);

        ClearLeds();
        WriteLeds();
    }

    HardwareVersion GetBoardRevision()
    {
        daisy::GPIO version_gpio;
        version_gpio.Init(daisy::seed::D2,
                          daisy::GPIO::Mode::INPUT,
                          daisy::GPIO::Pull::PULLUP);
        /** Read High means rev3 otherwise rev4 */
        auto pinstate = version_gpio.Read();
        version_gpio.DeInit();
        if(pinstate)
            return HardwareVersion::REV3;
        else
            return HardwareVersion::REV4;
    }

    /** @brief Loads and sets calibration data */
    void LoadCalibrationData()
    {
        daisy::PersistentStorage<CalibrationData> cal_storage(seed.qspi);
        CalibrationData                           default_cal;
        cal_storage.Init(default_cal, kCalibrationDataOffset);
        auto &cal_data = cal_storage.GetSettings();
        SetWarpCalData(cal_data.warp_scale, cal_data.warp_offset);
        SetCvOffsetData(cal_data.cv_offset);
    }
};

} // namespace aurora

#endif
