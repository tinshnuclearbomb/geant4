/// \file optical/wls/src/WLSTrackingAction.cc
/// \brief Implementation of the WLSTrackingAction class
//
//

#include "WLSTrackingAction.hh"

#include "WLSTrajectory.hh"
#include "WLSUserTrackInformation.hh"

#include "G4Track.hh"
#include "G4TrackingManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm được gọi trước khi bắt đầu tracking một track mới
// Thiết lập trajectory và thông tin bổ sung cho track photon
void WLSTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  // Có thể bật lưu trajectory qua macro vis.mac nếu cần
  // fpTrackingManager->SetStoreTrajectory(true);

  // Sử dụng lớp trajectory tùy biến để lưu thông tin đường đi của track
  fpTrackingManager->SetTrajectory(new WLSTrajectory(aTrack));

  // Tạo đối tượng lưu thông tin bổ sung cho track (UserTrackInformation)
  auto trackInformation = new WLSUserTrackInformation();

  // Đánh dấu hướng truyền ban đầu của photon (right hoặc left)
  if (aTrack->GetMomentumDirection().z() > 0.0) {
    trackInformation->AddStatusFlag(right); // Photon truyền theo chiều dương trục z
  }
  else {
    trackInformation->AddStatusFlag(left);  // Photon truyền theo chiều âm trục z
  }

  // Kiểm tra xem photon có nằm trong sợi quang hoặc lớp bọc không
  G4String PVName = aTrack->GetVolume()->GetName();
  if (PVName == "WLSFiber" || PVName == "Clad1" || PVName == "Clad2")
    trackInformation->AddStatusFlag(InsideOfFiber);

  // Gán thông tin bổ sung cho track này
  fpTrackingManager->SetUserTrackInformation(trackInformation);
}