#pragma once
#include "sysmon/core/system_reader.hpp"
#include "sysmon/core/process_view.hpp"
#include "sysmon/core/process_reader.hpp"
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <ftxui/component/screen_interactive.hpp>


class Application {
public:
    // Uygulamayı ve ana UI döngüsünü başlatacak tek fonksiyon
    void run();

private:
    // --- Çekirdek Bileşenler ---
    sysmon::SystemReader sysReader;
    sysmon::ProcessReader procReader;
    sysmon::ProcessView view;

    ftxui::ScreenInteractive* screenPtr{nullptr};
    // --- Paylaşımlı Veri ve Senkronizasyon (Senin hedefin) ---
    std::vector<sysmon::ProcessInfo> currProcesses; // Her iki thread'in okuyup/yazacağı veri
    std::vector<sysmon::ProcessInfo> viewList;
    std::mutex mt;                      // Çarpışmayı önleyen kilit

    // --- Thread Yönetimi ---
    std::jthread workerThread;

    // --- Arka Plan Fonksiyonu ---
    void workerLoop(std::stop_token stopToken);                          // Saniyede bir çalışacak işçi
};