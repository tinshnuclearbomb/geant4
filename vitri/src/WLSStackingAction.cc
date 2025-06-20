/// \file optical/wls/src/WLSStackingAction.cc
/// \brief Implementation of the WLSStackingAction class
//
//

#include "WLSStackingAction.hh"

#include "G4OpticalPhoton.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm phân loại track mới: quyết định giữ lại, loại bỏ, hoặc xử lý đặc biệt các track mới sinh ra
G4ClassificationOfNewTrack WLSStackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
  G4ParticleDefinition* particleType = aTrack->GetDefinition();

  // Giữ lại hạt sơ cấp (primary particle)
  if (aTrack->GetParentID() == 0) return fUrgent;

  // Nếu là photon quang học thì tăng bộ đếm và giữ lại
  if (particleType == G4OpticalPhoton::OpticalPhotonDefinition()) {
    ++fPhotonCounter;
    return fUrgent;
  }
  else {
    // Nếu là hạt thứ cấp khác (không phải photon quang học), có thể loại bỏ (fKill)
    // return fKill;
  }
  // Mặc định giữ lại các track khác (nếu không muốn loại bỏ)
  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm được gọi khi chuyển sang stage mới trong event (sau khi xử lý tất cả track hiện tại)
void WLSStackingAction::NewStage()
{
  // Có thể in ra số photon quang học sinh ra trong event này nếu cần
  // G4cout << "Number of optical photons produced in this event : "
  //        << fPhotonCounter << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm chuẩn bị cho event mới: reset bộ đếm photon về 0
void WLSStackingAction::PrepareNewEvent()
{
  fPhotonCounter = 0;
}