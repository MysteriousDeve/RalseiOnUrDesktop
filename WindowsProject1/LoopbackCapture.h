#pragma once
#include <iostream>
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#pragma comment(lib, "ole32.lib")

class LoopbackCapture
{
    HRESULT hr;
    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioClient* pAudioClient = nullptr;
    WAVEFORMATEX* pwfx = nullptr;
    IAudioCaptureClient* pCaptureClient = nullptr;

    bool success = false;
public:
    LoopbackCapture() {

        // Initialize COM
        CoInitialize(nullptr);

        // Get the audio device enumerator
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
        if (FAILED(hr)) {
            //std::cerr << "Failed to create device enumerator.\n";
            return;
        }

        // Get the default audio render device (system audio output)
        hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
        if (FAILED(hr)) {
            //std::cerr << "Failed to get default audio device.\n";
            pEnumerator->Release();
            return;
        }

        // Activate the IAudioClient
        hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pAudioClient);
        if (FAILED(hr)) {
            //std::cerr << "Failed to activate audio client.\n";
            pDevice->Release();
            pEnumerator->Release();
            return;
        }

        // Get the device format
        hr = pAudioClient->GetMixFormat(&pwfx);
        if (FAILED(hr)) {
            //std::cerr << "Failed to get mix format.\n";
            pAudioClient->Release();
            pDevice->Release();
            pEnumerator->Release();
            return;
        }

        // Initialize audio client in loopback mode
        // 24 char -> 6 float
        hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 500000, 0, pwfx, nullptr);
        if (FAILED(hr)) {
            //std::cerr << "Failed to initialize audio client.\n";
            CoTaskMemFree(pwfx);
            pAudioClient->Release();
            pDevice->Release();
            pEnumerator->Release();
            return;
        }

        // Get the audio capture client
        hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient);
        if (FAILED(hr)) {
            //std::cerr << "Failed to get capture client.\n";
            pAudioClient->Release();
            pDevice->Release();
            pEnumerator->Release();
            return;
        }

        // Start capturing
        hr = pAudioClient->Start();
        if (FAILED(hr)) {
            //std::cerr << "Failed to start capture.\n";
            pCaptureClient->Release();
            pAudioClient->Release();
            pDevice->Release();
            pEnumerator->Release();
            return;
        }

        // Mark init success
        success = true;
    }

    BYTE* pData;
    float* fData;
    UINT32 numFrames;
    DWORD flags;
    float GetAudioVolumeAvg()
    {
        // Capture
        hr = pCaptureClient->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
        if (SUCCEEDED(hr) && numFrames > 0) {
            fData = new float[numFrames / 4 + 1];
            memcpy(fData, pData, numFrames);

            int sampleCount = min(10, numFrames);
            float sum = 0;
            for (int i = 0; i < sampleCount; i++)
            {
                sum += abs(fData[i]);
            }

            delete[numFrames / 4 + 1] fData;
            pCaptureClient->ReleaseBuffer(numFrames);

            return sum / sampleCount;
        }
        return 0;
    }

    ~LoopbackCapture()
    {
        // Cleanup
        pAudioClient->Stop();
        pCaptureClient->Release();
        pAudioClient->Release();
        pDevice->Release();
        pEnumerator->Release();
        CoUninitialize();
    }
};