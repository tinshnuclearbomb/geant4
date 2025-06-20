/// \file optical/wls/src/WLSTrajectoryPoint.cc
/// \brief Implementation of the WLSTrajectoryPoint class
//
//
#include "WLSTrajectoryPoint.hh"

#include "G4AttDef.hh"
#include "G4AttDefStore.hh"
#include "G4AttValue.hh"
#include "G4Step.hh"
#include "G4StepStatus.hh"
#include "G4Track.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4VProcess.hh"

// Bộ cấp phát bộ nhớ động cho các đối tượng trajectory point (theo từng thread)
G4ThreadLocal G4Allocator<WLSTrajectoryPoint>* WLSTrajPointAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo từ một bước (step): lưu vị trí, thời gian, động lượng, trạng thái bước và tên volume
WLSTrajectoryPoint::WLSTrajectoryPoint(const G4Step* aStep)
  : G4TrajectoryPoint(aStep->GetPostStepPoint()->GetPosition())
{
  auto postStepPoint = aStep->GetPostStepPoint();
  fTime = postStepPoint->GetGlobalTime();                // Thời gian toàn cục tại điểm này
  fMomentum = postStepPoint->GetMomentum();              // Động lượng tại điểm này
  fStepStatus = postStepPoint->GetStepStatus();          // Trạng thái bước (ví dụ: at boundary)
  if (postStepPoint->GetPhysicalVolume()) {
    fVolumeName = postStepPoint->GetPhysicalVolume()->GetName(); // Tên volume tại điểm này
  }
  else {
    fVolumeName = G4String(" ");
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo từ track: lưu vị trí, thời gian, động lượng, trạng thái bước và tên volume
WLSTrajectoryPoint::WLSTrajectoryPoint(const G4Track* aTrack)
  : G4TrajectoryPoint(aTrack->GetPosition())
{
  fTime = aTrack->GetGlobalTime();               // Thời gian toàn cục tại điểm bắt đầu track
  fMomentum = aTrack->GetMomentum();             // Động lượng tại điểm bắt đầu track
  fStepStatus = fUndefined;                      // Không có trạng thái bước ở điểm bắt đầu
  fVolumeName = aTrack->GetVolume()->GetName();  // Tên volume tại điểm bắt đầu track
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm sao chép (copy constructor): sao chép toàn bộ thông tin của điểm trajectory
WLSTrajectoryPoint::WLSTrajectoryPoint(const WLSTrajectoryPoint& right) : G4TrajectoryPoint(right)
{
  fTime = right.fTime;
  fMomentum = right.fMomentum;
  fStepStatus = right.fStepStatus;
  fVolumeName = right.fVolumeName;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Định nghĩa các thuộc tính (attributes) cho trajectory point để hiển thị hoặc phân tích
const std::map<G4String, G4AttDef>* WLSTrajectoryPoint::GetAttDefs() const
{
  G4bool isNew;
  std::map<G4String, G4AttDef>* store = G4AttDefStore::GetInstance("TrajectoryPoint", isNew);
  if (isNew) {
    G4String Pos("Pos");
    (*store)[Pos] = G4AttDef(Pos, "Position", "Physics", "G4BestUnit", "G4ThreeVector");

    G4String Time("Time");
    (*store)[Time] = G4AttDef(Time, "Time", "Physics", "G4BestUnit", "G4double");

    G4String Momentum("Momentum");
    (*store)[Momentum] = G4AttDef(Momentum, "Momentum", "Physics", "G4BestUnit", "G4ThreeVector");

    G4String StepStatus("StepStatus");
    (*store)[StepStatus] = G4AttDef(StepStatus, "StepStatus", "Physics", "", "G4StepStatus");

    G4String VolumeName("VolumeName");
    (*store)[VolumeName] = G4AttDef(VolumeName, "VolumeName", "Physics", "", "G4String");
  }
  return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Tạo giá trị cho các thuộc tính trajectory point (dùng cho hiển thị hoặc xuất dữ liệu)
std::vector<G4AttValue>* WLSTrajectoryPoint::CreateAttValues() const
{
  auto values = new std::vector<G4AttValue>;

  values->push_back(G4AttValue("Time", G4BestUnit(fTime, "Time"), ""));
  values->push_back(G4AttValue("Momentum", G4BestUnit(fMomentum, "Momentum"), ""));
  values->push_back(G4AttValue("StepStatus", G4UIcommand::ConvertToString(fStepStatus), ""));
  values->push_back(G4AttValue("VolumeName", fVolumeName, ""));

  return values;
}