// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 Samuel Villarreal
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION: Sound system
//
//-----------------------------------------------------------------------------

#include "SDL.h"
#include "common.h"
#include "sound.h"
#include "client.h"
#include "filesystem.h"
#include "world.h"
#include "gameManager.h"

kexCvar cvarPitchShift("s_pitchshift", CVF_BOOL|CVF_CONFIG, "1", "TODO");
kexCvar cvarSoundVolume("s_sndvolume", CVF_FLOAT|CVF_CONFIG, "0.5", 0, 1, "TODO");

#define SND_METRICS 0.0035f
#define SND_VECTOR2METRICS(vec) \
    vec[0] * SND_METRICS,       \
    vec[1] * SND_METRICS,       \
    vec[2] * SND_METRICS

#define SND_INT2TIME(t) ((float)t * ((1.0f / 60.0f) * 1000.0f))

kexSoundSystem soundSystem;
kexHeapBlock kexSoundSystem::hb_sound("sound", false, NULL, NULL);

//
// printsoundinfo
//

COMMAND(printsoundinfo) {
    common.CPrintf(COLOR_CYAN, "------------- Sound Info -------------\n");
    common.CPrintf(COLOR_GREEN, "Device: %s\n", soundSystem.GetDeviceName());
    common.CPrintf(COLOR_GREEN, "Available Sources: %i\n", soundSystem.GetNumActiveSources());
}

//
// playsoundshader
//

COMMAND(playsoundshader) {
    if(command.GetArgc() < 2) {
        return;
    }

    soundSystem.StartSound(command.GetArgv(1), NULL);
}

//
// stopsounds
//

COMMAND(stopsounds) {
    if(command.GetArgc() < 1) {
        return;
    }

    soundSystem.StopAll();
}

//
// kexWavFile::kexWavFile
//

kexWavFile::kexWavFile(void) {
}

//
// kexWavFile::~kexWavFile
//

kexWavFile::~kexWavFile(void) {
}

//
// kexWavFile::CompareTag
//

bool kexWavFile::CompareTag(const char *tag, int offset) {
    byte *buf = waveFile + offset;

    return
        (buf[0] == tag[0] &&
         buf[1] == tag[1] &&
         buf[2] == tag[2] &&
         buf[3] == tag[3]);
}

//
// kexWavFile::GetFormat
//

int kexWavFile::GetFormat(void) {
    switch(channels) {
    case 1:
        switch(bits) {
        case 8:
            return AL_FORMAT_MONO8;
        case 16:
            return AL_FORMAT_MONO16;
        }
        break;
    case 2:
        switch(bits) {
        case 8:
            return AL_FORMAT_STEREO8;
        case 16:
            return AL_FORMAT_STEREO16;
        }
        break;
    default:
        common.Error("Snd_GetWaveFormat: Unsupported number of channels - %i", channels);
        return -1;
    }

    common.Error("Snd_GetWaveFormat: Unknown bits format - %i", bits);
    return -1;
}

//
// kexWavFile::Allocate
//

void kexWavFile::Allocate(const char *name, byte *data) {
    strcpy(filePath, name);
    waveFile = data;

    if(!CompareTag("RIFF", 0)) {
        common.Error("kexWavFile::Allocate: RIFF header not found in %s", name);
    }
    if(!CompareTag("WAVE", 8)) {
        common.Error("kexWavFile::Allocate: WAVE header not found in %s", name);
    }
    if(!CompareTag("fmt ", 12)) {
        common.Error("kexWavFile::Allocate: fmt header not found in %s", name);
    }
    if(!CompareTag("data", 36)) {
        common.Error("kexWavFile::Allocate: data header not found in %s", name);
    }

    if(*(data + 16) != 16) {
        common.Error("kexWavFile::Allocate: WAV chunk size must be 16 (%s)", name);
    }

    formatCode  = *(short*)(data + 20);
    channels    = *(short*)(data + 22);
    samples     = *(int*)(data + 24);
    bytes       = *(int*)(data + 28);
    blockAlign  = *(short*)(data + 32);
    bits        = *(short*)(data + 34);
    waveSize    = *(int*)(data + 40);
    data        = data + 44;

    alGetError();
    alGenBuffers(1, &buffer);

    if(alGetError() != AL_NO_ERROR) {
        common.Error("kexWavFile::Allocate: failed to create buffer for %s", name);
    }

    alBufferData(buffer, GetFormat(), data, waveSize, samples);
}

//
// kexWavFile::Delete
//

void kexWavFile::Delete(void) {
    alDeleteBuffers(1, &buffer);
    buffer = 0;
}

//
// kexSoundSource::kexSoundSource
//

kexSoundSource::kexSoundSource(void) {
}

//
// kexSoundSource::~kexSoundSource
//

kexSoundSource::~kexSoundSource(void) {
}

//
// kexSoundSource::Generate
//

bool kexSoundSource::Generate(void) {
    alGetError();
    alGenSources(1, &handle);

    if(alGetError() != AL_NO_ERROR) {
        return false;
    }

    startTime   = 0;
    bInUse      = false;
    bLooping    = false;
    bPlaying    = false;
    sfx         = NULL;
    obj         = NULL;
    volume      = 1.0f;

    alSourcei(handle, AL_LOOPING, AL_FALSE);
    alSourcei(handle, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcef(handle, AL_GAIN, 1.0f);
    alSourcef(handle, AL_PITCH, 1.0f);
    return true;
}

//
// kexSoundSource::Set
//

void kexSoundSource::Set(sfx_t *sfxRef, kexGameObject *refObj) {
    if(sfxRef == NULL) {
        return;
    }

    sfx = sfxRef;

    if(refObj) {
        refObj->AddRef();
    }

    if(sfx->bLerpVol) {
        volume = sfx->gainLerpStart;
    }

    if(sfx->bLerpFreq) {
        pitch = sfx->freqLerpStart;
    }

    obj = refObj;
}

//
// kexSoundSource::Stop
//

void kexSoundSource::Stop(void) {
    alSourceStop(handle);

    if(sfx && sfx->wavFile) {
        alSourceUnqueueBuffers(handle, 1, sfx->wavFile->GetBuffer());
    }
}

//
// kexSoundSource::Free
//

void kexSoundSource::Free(void) {
    bInUse      = false;
    bPlaying    = false;
    sfx         = NULL;
    volume      = 1.0f;
    pitch       = 1.0f;
    startTime   = 0;

    if(obj) {
        obj->RemoveRef();
        obj = NULL;
    }

    alSource3f(handle, AL_POSITION, 0, 0, 0);
}

//
// kexSoundSource::Delete
//

void kexSoundSource::Delete(void) {
    if(bPlaying) {
        Free();
    }

    Stop();

    alSourcei(handle, AL_BUFFER, 0);
    alDeleteSources(1, &handle);

    handle = 0;
}

//
// kexSoundSource::Reset
//

void kexSoundSource::Reset(void) {
    startTime   = kexSoundSystem::time;
    bInUse      = true;
    bPlaying    = false;
    volume      = 1.0f;
    pitch       = 1.0f;
}

//
// kexSoundSource::Play
//

void kexSoundSource::Play(void) {
    float time;
    kexWavFile *wave = sfx->wavFile;

    time = (float)startTime + SND_INT2TIME(sfx->delay);

    if(time > kexSoundSystem::time) {
        return;
    }

    alSourceQueueBuffers(handle, 1, wave->GetBuffer());

    if(cvarPitchShift.GetBool()) {
        alSourcef(handle, AL_PITCH, sfx->dbFreq);
    }

    if(obj && obj != gameManager.localPlayer.Puppet()) {
        kexVec3 org = obj->GetOrigin();
        alSourcef(handle, AL_ROLLOFF_FACTOR, sfx->rolloffFactor);
        alSourcei(handle, AL_SOURCE_RELATIVE, AL_FALSE);
        alSource3f(handle, AL_POSITION, SND_VECTOR2METRICS(org));
    }
    else {
        alSourcei(handle, AL_SOURCE_RELATIVE, AL_TRUE);
    }

    alSourcef(handle, AL_GAIN, sfx->gain * volume * cvarSoundVolume.GetFloat());
    alSourcePlay(handle);

    bPlaying = true;
    startTime = kexSoundSystem::time;
}

//
// kexSoundSource::Update
//

void kexSoundSource::Update(void) {
    if(!bInUse) {
        return;
    }

    if(sfx != NULL) {
        if(!bPlaying) {
            Play();
        }
        else {
            ALint state;

            alGetSourcei(handle, AL_SOURCE_STATE, &state);
            if(state != AL_PLAYING) {
                Stop();
                Free();
            }
            else {
                if(sfx->bLerpVol && bPlaying) {
                    float time = (float)startTime + SND_INT2TIME(sfx->gainLerpDelay);

                    if(time <= kexSoundSystem::time) {
                        float volLerp = (float)sfx->gainLerpTime / 60.0f;
                        volume = (sfx->gainLerpEnd - volume) * volLerp + volume;

                        if(volume > 1) volume = 1;
                        if(volume < 0.01f) {
                            Stop();
                            Free();
                            return;
                        }

                        alSourcef(handle, AL_GAIN, sfx->gain * volume * cvarSoundVolume.GetFloat());
                    }
                }

                if(sfx->bLerpFreq && bPlaying && cvarPitchShift.GetBool()) {
                    float time = (float)startTime + SND_INT2TIME(sfx->freqLerpDelay);

                    if(time <= kexSoundSystem::time) {
                        float freqLerp = (float)sfx->freqLerpTime / 60.0f;
                        pitch = (sfx->freqLerpEnd - pitch) * freqLerp + pitch;

                        alSourcef(handle, AL_PITCH, pitch);
                    }
                }
            }
        }
    }
}

int kexSoundSystem::time = 0;

//
// kexSoundSystem::kexSoundSystem
//

kexSoundSystem::kexSoundSystem(void) {
}

//
// kexSoundSystem::~kexSoundSystem
//

kexSoundSystem::~kexSoundSystem(void) {
}

//
// kexSoundSystem::Init
//

void kexSoundSystem::Init(void) {
    int i;

    alDevice = alcOpenDevice(NULL);
    if(!alDevice) {
        common.Error("kexSoundSystem::Init: Failed to create OpenAL device");
    }

    alContext = alcCreateContext(alDevice, NULL);
    if(!alContext) {
        common.Error("kexSoundSystem::Init: Failed to create OpenAL context");
    }

    if(!alcMakeContextCurrent(alContext)) {
        common.Error("kexSoundSystem::Init: Failed to set current context");
    }

    for(i = 0; i < SND_MAX_SOURCES; i++) {
        kexSoundSource *sndSrc = &sources[activeSources];

        if(sndSrc->Generate() == false) {
            break;
        }

        activeSources++;
    }

    alListener3f(AL_POSITION, 0, 0, 0);

    common.Printf("Sound System Initialized (%s)\n", GetDeviceName());
}

//
// kexSoundSystem::Shutdown
//

void kexSoundSystem::Shutdown(void) {
    int i;
    kexWavFile *wavFile;

    common.Printf("Shutting down audio\n");

    for(i = 0; i < activeSources; i++) {
        sources[i].Delete();
    }

    for(i = 0; i < MAX_HASH; i++) {
        for(wavFile = wavList.GetData(i); wavFile; wavFile = wavList.Next()) {
            wavFile->Delete();
        }
    }

    alcMakeContextCurrent(NULL);
    alcDestroyContext(alContext);
    alcCloseDevice(alDevice);

    Mem_Purge(kexSoundSystem::hb_sound);
}

//
// kexSoundSystem::GetDeviceName
//

char *kexSoundSystem::GetDeviceName(void) {
    return (char*)alcGetString(alDevice, ALC_DEVICE_SPECIFIER);
}

//
// kexSoundSystem::StopAll
//

void kexSoundSystem::StopAll(void) {
    for(int i = 0; i < activeSources; i++) {
        sources[i].Stop();
        sources[i].Free();
    }
}

//
// kexSoundSystem::UpdateListener
//

void kexSoundSystem::UpdateListener(void) {
    ALfloat orientation[6];
    kexVec3 org;

    if(localWorld.IsLoaded() == false) {
        return;
    }

    kexCamera *camera = localWorld.Camera();
    kexMatrix matrix = camera->ModelView();
    
    orientation[0] = -matrix.vectors[0].z;
    orientation[1] = -matrix.vectors[1].z;
    orientation[2] = -matrix.vectors[2].z;
    orientation[3] =  matrix.vectors[0].y;
    orientation[4] =  matrix.vectors[1].y;
    orientation[5] =  matrix.vectors[2].y;

    org = camera->GetOrigin();

    alListenerfv(AL_ORIENTATION, orientation);
    alListener3f(AL_POSITION, SND_VECTOR2METRICS(org));

    for(int i = 0; i < soundSystem.GetNumActiveSources(); i++) {
        soundSystem.GetSources()[i].Update();
    }

    kexSoundSystem::time = client.GetTime();
}

//
// kexSoundSystem::GetAvailableSource
//

kexSoundSource *kexSoundSystem::GetAvailableSource(void) {
    int i;
    kexSoundSource *src = NULL;

    for(i = 0; i < activeSources; i++) {
        kexSoundSource *sndSrc = &sources[i];

        if(sndSrc->InUse()) {
            continue;
        }

        src = sndSrc;
        src->Reset();
        break;
    }

    return src;
}

//
// kexSoundSystem::HasSource
//

bool kexSoundSystem::HasSource(const kexGameObject *obj) {
    for(int i = 0; i < activeSources; i++) {
        kexSoundSource *sndSrc = &sources[i];

        if(sndSrc->obj == obj) {
            return true;
        }
    }

    return false;
}

//
// kexSoundSystem::CacheWavFile
//

kexWavFile *kexSoundSystem::CacheWavFile(const char *name) {
    kexWavFile *wavFile = NULL;

    if(!(wavFile = wavList.Find(name))) {
        byte *data;

        if(fileSystem.OpenFile(name, &data, kexSoundSystem::hb_sound) == 0) {
            return NULL;
        }

        wavFile = wavList.Add(name, kexSoundSystem::hb_sound);
        wavFile->Allocate(name, data);
    }

    return wavFile;
}

//
// kexSoundSystem::CacheShaderFile
//

kexSoundShader *kexSoundSystem::CacheShaderFile(const char *name) {
    kexSoundShader *snd;

    if(name == NULL || name[0] == 0) {
        return NULL;
    }

    if(!(snd = shaderList.Find(name))) {
        kexLexer *lexer;

        if(!(lexer = parser.Open(name))) {
            return NULL;
        }

        snd = shaderList.Add(name, kexSoundSystem::hb_sound);
        snd->Load(lexer);
        parser.Close();
    }

    return snd;
}

//
// kexSoundSystem::StartSound
//

void kexSoundSystem::StartSound(const char *name, kexGameObject *obj) {
    kexSoundShader *sndShader;

    if(!(sndShader = CacheShaderFile(name))) {
        return;
    }

    sndShader->Play(obj);
}
