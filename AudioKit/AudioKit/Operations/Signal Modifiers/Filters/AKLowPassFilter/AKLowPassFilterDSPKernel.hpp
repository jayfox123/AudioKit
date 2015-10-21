//
//  AKLowPassFilterDSPKernel.hpp
//  AudioKit
//
//  Autogenerated by scripts by Aurelius Prochazka. Do not edit directly.
//  Copyright (c) 2015 Aurelius Prochazka. All rights reserved.
//

#ifndef AKLowPassFilterDSPKernel_hpp
#define AKLowPassFilterDSPKernel_hpp

#import "AKDSPKernel.hpp"
#import "AKParameterRamper.hpp"

extern "C" {
#include "base.h"
#include "tone.h"
}

enum {
    halfPowerPointAddress = 0
};

class AKLowPassFilterDSPKernel : public AKDSPKernel {
public:
    // MARK: Member Functions

    AKLowPassFilterDSPKernel() {}

    void init(int channelCount, double inSampleRate) {
        channels = channelCount;

        sampleRate = float(inSampleRate);

        sp_create(&sp);
        sp_tone_create(&tone);
        sp_tone_init(sp, tone);
        tone->hp = 1000;
    }

    void reset() {
    }

    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case halfPowerPointAddress:
                halfPowerPointRamper.set(clamp(value, (float)12.0, (float)20000.0));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case halfPowerPointAddress:
                return halfPowerPointRamper.goal();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
            case halfPowerPointAddress:
                halfPowerPointRamper.startRamp(clamp(value, (float)12.0, (float)20000.0), duration);
                break;

        }
    }

    void setBuffers(AudioBufferList* inBufferList, AudioBufferList* outBufferList) {
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        // For each sample.
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            double halfPowerPoint = double(halfPowerPointRamper.getStep());

            int frameOffset = int(frameIndex + bufferOffset);

            tone->hp = (float)halfPowerPoint;

            for (int channel = 0; channel < channels; ++channel) {
                float* in  = (float*)inBufferListPtr->mBuffers[channel].mData  + frameOffset;
                float* out = (float*)outBufferListPtr->mBuffers[channel].mData + frameOffset;

                sp_tone_compute(sp, tone, in, out);
            }
        }
    }

    // MARK: Member Variables

private:

    int channels = 2;
    float sampleRate = 44100.0;

    AudioBufferList* inBufferListPtr = nullptr;
    AudioBufferList* outBufferListPtr = nullptr;

    sp_data *sp;
    sp_tone *tone;

public:
    AKParameterRamper halfPowerPointRamper = 1000;
};

#endif /* AKLowPassFilterDSPKernel_hpp */
