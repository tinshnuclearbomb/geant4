/// \file optical/wls/src/WLSRunAction.cc
/// \brief Implementation of the WLSRunAction class
//
//

#include "WLSRunAction.hh"

#include "WLSDetectorConstruction.hh"
#include "WLSRun.hh"
#include "WLSSteppingAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo: thiết lập các histogram và cấu hình cho phân tích dữ liệu
WLSRunAction::WLSRunAction()
{
  auto analysisManager = G4AnalysisManager::Instance();

  // Đặt kiểu file mặc định là ROOT
  analysisManager->SetDefaultFileType("root");
  // Đặt mức verbose cho phân tích (1: in thông tin cơ bản)
  analysisManager->SetVerboseLevel(1);
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Tạo các histogram 1 chiều (H1) cho năng lượng, thời gian và số photon
  analysisManager->CreateH1("Energy", "Energy of optical photon", 100, 2. * CLHEP::eV,
                            3.2 * CLHEP::eV);
  analysisManager->CreateH1("Time", "Arrival time", 100, 0., 100. * CLHEP::ns);
  analysisManager->CreateH1("Number of photons", "Number of photons", 100, 0., 100.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm GenerateRun: tạo đối tượng WLSRun mới cho mỗi run
G4Run* WLSRunAction::GenerateRun()
{
  fRun = new WLSRun();
  return fRun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm BeginOfRunAction: mở file ghi dữ liệu phân tích khi bắt đầu run
void WLSRunAction::BeginOfRunAction(const G4Run*)
{
  G4AnalysisManager::Instance()->OpenFile("wls");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm EndOfRunAction: ghi và đóng file dữ liệu, in thống kê histogram, gọi EndOfRun của WLSRun
void WLSRunAction::EndOfRunAction(const G4Run*)
{
  auto analysisManager = G4AnalysisManager::Instance();
  if (analysisManager->GetH1(0)) {
    G4cout << G4endl << " ----> print histograms statistics ";
    if (isMaster) {
      G4cout << "for the entire run " << G4endl << G4endl;
    }
    else {
      G4cout << "for the local thread " << G4endl << G4endl;
    }

    // In ra giá trị trung bình và độ lệch chuẩn của số photon ghi nhận được trên mỗi event
    G4cout << " Mean number of photons detected/event: " << analysisManager->GetH1(2)->mean()
           << " rms = " << analysisManager->GetH1(2)->rms() << G4endl;
  }

  // Ghi dữ liệu vào file và đóng file
  analysisManager->Write();
  analysisManager->CloseFile();

  // Nếu là tiến trình master, in thống kê tổng kết của run
  if (isMaster) fRun->EndOfRun();
}