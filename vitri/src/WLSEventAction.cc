//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file optical/wls/src/WLSEventAction.cc
/// \brief Implementation of the WLSEventAction class
//
//

#include "WLSEventAction.hh"

#include "WLSEventActionMessenger.hh"
#include "WLSPhotonDetHit.hh"
#include "WLSRun.hh"
#include "WLSRunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VVisManager.hh"
#include "Randomize.hh"

// Hàm khởi tạo: Khởi tạo messenger cho EventAction
WLSEventAction::WLSEventAction()
{
  fEventMessenger = new WLSEventActionMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm hủy: Giải phóng bộ nhớ messenger
WLSEventAction::~WLSEventAction()
{
  delete fEventMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm được gọi ở đầu mỗi event: reset các biến đếm thống kê
void WLSEventAction::BeginOfEventAction(const G4Event*)
{
  fNTIR = 0;         // Số lần phản xạ toàn phần bên trong (Total Internal Reflection)
  fNExiting = 0;     // Số lần hạt thoát ra ngoài
  fEscapedEnd = 0;   // Số lần hạt thoát ở đầu thanh
  fEscapedMid = 0;   // Số lần hạt thoát ở giữa thanh
  fBounce = 0;       // Số lần phản xạ thông thường
  fWLSBounce = 0;    // Số lần phản xạ trên sợi WLS
  fClad1Bounce = 0;  // Số lần phản xạ trên lớp bọc 1
  fClad2Bounce = 0;  // Số lần phản xạ trên lớp bọc 2
  fReflected = 0;    // Số lần bị phản xạ
  fEscaped = 0;      // Số lần thoát khỏi hệ
  fMirror = 0;       // Số lần bị gương phản xạ
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm được gọi ở cuối mỗi event: tổng hợp và lưu lại các thống kê
void WLSEventAction::EndOfEventAction(const G4Event* evt)
{
  // Lấy bộ quản lý detector nhạy (Sensitive Detector)
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String colName = "PhotonDetHitCollection";
  fMPPCCollID = SDman->GetCollectionID(colName);

  // Lấy bộ sưu tập các hit trong event này
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  WLSPhotonDetHitsCollection* mppcHC = nullptr;

  if (HCE) {
    if (fMPPCCollID >= 0) {
      mppcHC = (WLSPhotonDetHitsCollection*)(HCE->GetHC(fMPPCCollID));
    }
  }

  // Đếm số photon ghi nhận được tại detector trong event này
  G4int n_hit = 0;
  if (mppcHC) {
    n_hit = mppcHC->entries();
  }

  // Ghi dữ liệu vào các histogram (năng lượng, thời gian đến, số lượng hit)
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillH1(2, mppcHC ? mppcHC->entries() : 0);
  if (mppcHC) {
    for (size_t i = 0; i < mppcHC->entries(); ++i) {
      auto pdHit = (*mppcHC)[i];
      analysisManager->FillH1(0, pdHit->GetEnergy());
      analysisManager->FillH1(1, pdHit->GetArrivalTime());
    }
  }

  // Nếu bật chế độ verbose, in ra các thống kê chi tiết của event này
  if (fVerboseLevel > 1) {
    G4cout << "-------------------------------------" << G4endl
           << " In this event, number of:" << G4endl
           << "  TIR:           " << fNTIR << G4endl
           << "  Exiting:       " << fNExiting << G4endl
           << "  Escaped Mid:   " << fEscapedMid << G4endl
           << "  Escaped End:   " << fEscapedEnd << G4endl
           << "  Bounced:       " << fBounce << G4endl
           << "  WLS Bounce:    " << fWLSBounce << G4endl
           << "  Clad1 Bounce:  " << fClad1Bounce << G4endl
           << "  Clad2 Bounce:  " << fClad2Bounce << G4endl
           << "  Reflected:     " << fReflected << G4endl
           << "  Escaped:       " << fEscaped << G4endl
           << "  Mirror:        " << fMirror << G4endl
           << "  Detector hit:  " << n_hit << G4endl;
  }

  // Lấy đối tượng Run hiện tại và cộng dồn các thống kê vào tổng của Run
  auto run = static_cast<WLSRun*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  run->AddTIR(fNTIR);
  run->AddExiting(fNExiting);
  run->AddEscapedEnd(fEscapedEnd);
  run->AddEscapedMid(fEscapedMid);
  run->AddBounce(fBounce);
  run->AddClad1Bounce(fClad1Bounce);
  run->AddClad2Bounce(fClad2Bounce);
  run->AddReflected(fReflected);
  run->AddEscaped(fEscaped);
  run->AddMirror(fMirror);
  run->AddDetectorHits(n_hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm trả về số thứ tự event hiện tại
G4int WLSEventAction::GetEventNo()
{
  return fpEventManager->GetConstCurrentEvent()->GetEventID();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm thiết lập mức độ chi tiết khi in thông tin event
void WLSEventAction::SetEventVerbose(G4int level)
{
  fVerboseLevel = level;
}