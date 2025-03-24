#include <iostream>
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#pragma comment(lib, "ole32.lib")

int main() {
    HRESULT hr;

    // Initialize COM
    CoInitialize(nullptr);

    // Get the audio device enumerator
    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) {
        std::cerr << "Failed to create device enumerator.\n";
        return -1;
    }

    // Get the default audio render device (system audio output)
    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hr)) {
        std::cerr << "Failed to get default audio device.\n";
        pEnumerator->Release();
        return -1;
    }

    // Activate the IAudioClient
    IAudioClient* pAudioClient = nullptr;
    hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pAudioClient);
    if (FAILED(hr)) {
        std::cerr << "Failed to activate audio client.\n";
        pDevice->Release();
        pEnumerator->Release();
        return -1;
    }

    // Get the device format
    WAVEFORMATEX* pwfx = nullptr;
    hr = pAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr)) {
        std::cerr << "Failed to get mix format.\n";
        pAudioClient->Release();
        pDevice->Release();
        pEnumerator->Release();
        return -1;
    }

    // Initialize audio client in loopback mode
    // 12 char -> 3 float
    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 250000, 0, pwfx, nullptr);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize audio client.\n";
        CoTaskMemFree(pwfx);
        pAudioClient->Release();
        pDevice->Release();
        pEnumerator->Release();
        return -1;
    }

    // Get the audio capture client
    IAudioCaptureClient* pCaptureClient = nullptr;
    hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient);
    if (FAILED(hr)) {
        std::cerr << "Failed to get capture client.\n";
        pAudioClient->Release();
        pDevice->Release();
        pEnumerator->Release();
        return -1;
    }

    // Start capturing
    hr = pAudioClient->Start();
    if (FAILED(hr)) {
        std::cerr << "Failed to start capture.\n";
        pCaptureClient->Release();
        pAudioClient->Release();
        pDevice->Release();
        pEnumerator->Release();
        return -1;
    }

    std::cout << "Capturing system audio...\n";

    // Capture loop (simplified)
    BYTE* pData;
    float* fData;
    UINT32 numFrames;
    DWORD flags;

    for (int i = 0; i < 10000; ++i) {  // Simple loop to capture some audio
        hr = pCaptureClient->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
        if (SUCCEEDED(hr) && numFrames > 0) {
            if (numFrames > 0)
            {
                fData = new float[numFrames / 4 + 1];
                memcpy(fData, pData, numFrames);
                std::cout << "Captured " << fData[0] << " frames\n";
                delete[numFrames / 4 + 1] fData;
            }
            pCaptureClient->ReleaseBuffer(numFrames);
        }
        Sleep(100); // Simulate audio processing delay
    }

    // Cleanup
    pAudioClient->Stop();
    pCaptureClient->Release();
    pAudioClient->Release();
    pDevice->Release();
    pEnumerator->Release();
    CoUninitialize();

    std::cout << "Audio capture stopped.\n";
    return 0;
}
