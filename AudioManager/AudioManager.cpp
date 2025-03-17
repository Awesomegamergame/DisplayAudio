#include "AudioManager.h"
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys.h>
#include <iostream>
#include <sstream>

void GetSpeakerConfiguration(char* buffer, int bufferSize) {
    HRESULT m_hr;

    m_hr = CoInitialize(NULL);

    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pDevice = NULL;
    IAudioClient* pAudioClient = NULL;
    IPropertyStore* pProps = NULL;
    WAVEFORMATEX* pwfx = NULL;
    LPWSTR deviceId = NULL;
    PROPVARIANT deviceNameProp;
    PropVariantInit(&deviceNameProp);

    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) return;

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    if (FAILED(hr)) {
        pEnumerator->Release();
        return;
    }

    // Get the device name
    hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
    if (SUCCEEDED(hr)) {
        hr = pProps->GetValue(PKEY_Device_FriendlyName, &deviceNameProp);
    }

    hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
    if (FAILED(hr)) {
        pProps->Release();
        pDevice->Release();
        pEnumerator->Release();
        return;
    }

    hr = pAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr)) {
        pAudioClient->Release();
        pProps->Release();
        pDevice->Release();
        pEnumerator->Release();
        return;
    }

    std::ostringstream oss;

    // Device name
    oss << "Device Name: ";
    if (SUCCEEDED(hr) && deviceNameProp.vt == VT_LPWSTR) {
        std::wstring deviceName(deviceNameProp.pwszVal);
        oss << std::string(deviceName.begin(), deviceName.end());
    }
    else {
        oss << "Unknown Device";
    }
    oss << "\n";

    // Number of Channels
    oss << "Number of Channels: " << pwfx->nChannels << "\n";

    // Speaker Configuration
    oss << "Speaker Configuration: ";
    if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        WAVEFORMATEXTENSIBLE* pwfex = (WAVEFORMATEXTENSIBLE*)pwfx;
        DWORD channelMask = pwfex->dwChannelMask;

        if (channelMask & SPEAKER_FRONT_LEFT) oss << "Front Left ";
        if (channelMask & SPEAKER_FRONT_RIGHT) oss << "Front Right ";
        if (channelMask & SPEAKER_FRONT_CENTER) oss << "Front Center ";
        if (channelMask & SPEAKER_LOW_FREQUENCY) oss << "Subwoofer ";
        if (channelMask & SPEAKER_BACK_LEFT) oss << "Rear Left ";
        if (channelMask & SPEAKER_BACK_RIGHT) oss << "Rear Right ";
        if (channelMask & SPEAKER_SIDE_LEFT) oss << "Side Left ";
        if (channelMask & SPEAKER_SIDE_RIGHT) oss << "Side Right ";
    }
    oss << "\n";

    // Detect common speaker configurations
    if (pwfx->nChannels == 1) {
        oss << "Mono\n";
    }
    else if (pwfx->nChannels == 2) {
        oss << "Stereo\n";
    }
    else if (pwfx->nChannels == 6) {
        oss << "5.1 Surround\n";
    }
    else if (pwfx->nChannels == 8) {
        oss << "7.1 Surround\n";
    }
    else {
        oss << "Unknown Configuration\n";
    }

    strncpy_s(buffer, bufferSize, oss.str().c_str(), bufferSize - 1);

    PropVariantClear(&deviceNameProp);
    CoTaskMemFree(pwfx);
    pProps->Release();
    pAudioClient->Release();
    pDevice->Release();
    pEnumerator->Release();
    CoUninitialize();
}
