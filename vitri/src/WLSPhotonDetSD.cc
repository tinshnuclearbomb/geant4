/// \file optical/wls/src/WLSPhotonDetSD.cc
/// \brief Implementation of the WLSPhotonDetSD class
//
//
#include "WLSPhotonDetSD.hh"

#include "WLSPhotonDetHit.hh"
#include "WLSUserTrackInformation.hh"

#include "G4OpticalPhoton.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"

// Hàm khởi tạo: Đăng ký tên bộ sưu tập hit cho detector nhạy này
WLSPhotonDetSD::WLSPhotonDetSD(G4String name) : G4VSensitiveDetector(name)
{
  // Đặt tên bộ sưu tập hit sẽ được tạo cho mỗi event
  collectionName.insert("PhotonDetHitCollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo bộ sưu tập hit cho mỗi event
void WLSPhotonDetSD::Initialize(G4HCofThisEvent* HCE)
{
  // Tạo bộ sưu tập hit mới cho detector này
  fPhotonDetHitCollection =
    new WLSPhotonDetHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Lấy ID của bộ sưu tập hit (chỉ lấy một lần duy nhất)
  if (fHCID < 0) {
    fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fPhotonDetHitCollection);
  }
  // Thêm bộ sưu tập hit vào danh sách các hit của event hiện tại
  HCE->AddHitsCollection(fHCID, fPhotonDetHitCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm xử lý từng bước (step) của hạt trong detector nhạy
G4bool WLSPhotonDetSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  if (!aStep) return false;
  G4Track* theTrack = aStep->GetTrack();

  // Kiểm tra xem hạt này có phải là photon quang học không
  if (theTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
    return false;
  }

  // Lấy thông tin tại điểm sau bước (post-step)
  G4StepPoint* thePostPoint = aStep->GetPostStepPoint();

  // Lấy thông tin bổ sung của track (ví dụ: vị trí thoát khỏi sợi quang)
  auto trackInformation = (WLSUserTrackInformation*)theTrack->GetUserInformation();

  // Lấy thông tin về vị trí và hệ tọa độ của detector
  auto theTouchable = (G4TouchableHistory*)(thePostPoint->GetTouchable());

  // Lấy vị trí photon rời khỏi sợi quang (global)
  G4ThreeVector photonExit = trackInformation->GetExitPosition();
  // Lấy vị trí photon đến detector (global)
  G4ThreeVector photonArrive = thePostPoint->GetPosition();
  // Thời gian photon đến detector
  G4double arrivalTime = theTrack->GetGlobalTime();
  // Năng lượng photon tại detector
  G4double energy = theTrack->GetTotalEnergy();

  // Chuyển vị trí photon đến từ hệ global sang hệ local của detector
  photonArrive = theTouchable->GetHistory()->GetTopTransform().TransformPoint(photonArrive);

  // Tạo đối tượng hit mới và thêm vào bộ sưu tập hit
  fPhotonDetHitCollection->insert(
    new WLSPhotonDetHit(photonExit, photonArrive, arrivalTime, energy));

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm được gọi khi kết thúc event: in thông tin các hit nếu bật verbose
void WLSPhotonDetSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel > 1) {
    G4int nofHits = fPhotonDetHitCollection->entries();
    G4cout << G4endl << "-------->Hits Collection: in this event there are " << nofHits
           << " hits in the photon detector: " << G4endl;
    for (G4int i = 0; i < nofHits; i++)
      (*fPhotonDetHitCollection)[i]->Print();
  }
}