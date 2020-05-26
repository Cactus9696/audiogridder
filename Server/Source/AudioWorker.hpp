/*
 * Copyright (c) 2020 Andreas Pohl
 * Licensed under MIT (https://github.com/apohl79/audiogridder/blob/master/COPYING)
 *
 * Author: Andreas Pohl
 */

#ifndef AudioWorker_hpp
#define AudioWorker_hpp

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProcessorChain.hpp"
#include <thread>

namespace e47 {

struct audio_chunk_hdr_t {
    int channels;
    int samples;
    bool isDouble;
};

class ProcessorChain;

class AudioWorker : public Thread {
  public:
    AudioWorker() : Thread("AudioWorker") { m_chain = std::make_shared<ProcessorChain>(); }
    virtual ~AudioWorker() override;

    void init(std::unique_ptr<StreamingSocket> s, int channels, double rate, int samplesPerBlock, bool doublePrecission,
              std::function<void()> fn);

    void run() override;
    void shutdown();
    void clear();

    bool addPlugin(const String& id);
    void delPlugin(int idx);
    void exchangePlugins(int idxA, int idxB);
    std::shared_ptr<AudioPluginInstance> getProcessor(int idx) const { return m_chain->getProcessor(idx); }
    int getSize() const { return static_cast<int>(m_chain->getSize()); }
    int getLatencySamples() const { return m_chain->getLatencySamples(); }

    float getParameterValue(int idx, int paramIdx) { return m_chain->getParameterValue(idx, paramIdx); }

    using RecentsListType = Array<PluginDescription>;
    RecentsListType& getRecentsList(String host) const;
    void addToRecentsList(const String& id, const String& host);

  private:
    std::unique_ptr<StreamingSocket> m_socket;
    int m_channels;
    double m_rate;
    int m_samplesPerBlock;
    bool m_doublePrecission;
    std::shared_ptr<ProcessorChain> m_chain;
    static HashMap<String, RecentsListType> m_recents;
    static std::mutex m_recentsMtx;
    std::function<void()> m_onTerminate;
};

}  // namespace e47

#endif /* AudioWorker_hpp */
