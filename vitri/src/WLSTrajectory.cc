/// \file optical/wls/src/WLSTrajectory.cc
/// \brief Implementation of the WLSTrajectory class
//
//

#include "WLSTrajectory.hh"
#include "WLSTrajectoryPoint.hh"

#include "G4AttDef.hh"
#include "G4AttDefStore.hh"
#include "G4AttValue.hh"
#include "G4Colour.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4Polyline.hh"
#include "G4Polymarker.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"

// Bộ cấp phát bộ nhớ động cho các đối tượng trajectory (theo từng thread)
G4ThreadLocal G4Allocator<WLSTrajectory>* WLSTrajectoryAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo từ track: lưu thông tin hạt, động lượng, ID, và điểm đầu tiên
WLSTrajectory::WLSTrajectory(const G4Track* aTrack)
{
  fParticleDefinition = aTrack->GetDefinition();
  fParticleName = fParticleDefinition->GetParticleName();
  fPDGCharge = fParticleDefinition->GetPDGCharge();
  fPDGEncoding = fParticleDefinition->GetPDGEncoding();
  fTrackID = aTrack->GetTrackID();
  fParentID = aTrack->GetParentID();
  fInitialMomentum = aTrack->GetMomentum();
  fpPointsContainer = new WLSTrajectoryPointContainer();
  // Thêm điểm trajectory đầu tiên (vị trí bắt đầu track)
  fpPointsContainer->push_back(new WLSTrajectoryPoint(aTrack));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm sao chép (copy constructor): sao chép toàn bộ thông tin và các điểm trajectory
WLSTrajectory::WLSTrajectory(WLSTrajectory& right) : G4VTrajectory()
{
  fParticleDefinition = right.fParticleDefinition;
  fParticleName = right.fParticleName;
  fPDGCharge = right.fPDGCharge;
  fPDGEncoding = right.fPDGEncoding;
  fTrackID = right.fTrackID;
  fParentID = right.fParentID;
  fInitialMomentum = right.fInitialMomentum;
  fpPointsContainer = new WLSTrajectoryPointContainer();

  for (size_t i = 0; i < right.fpPointsContainer->size(); ++i) {
    auto rightPoint = (WLSTrajectoryPoint*)((*(right.fpPointsContainer))[i]);
    fpPointsContainer->push_back(new WLSTrajectoryPoint(*rightPoint));
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm hủy: giải phóng bộ nhớ cho các điểm trajectory đã tạo
WLSTrajectory::~WLSTrajectory()
{
  for (size_t i = 0; i < fpPointsContainer->size(); ++i) {
    delete (*fpPointsContainer)[i];
  }
  fpPointsContainer->clear();
  delete fpPointsContainer;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm hiển thị trajectory ra output stream (có thể override để tùy biến)
void WLSTrajectory::ShowTrajectory(std::ostream& os) const
{
  // Gọi hàm mặc định của G4VTrajectory (có thể thay đổi nếu muốn)
  G4VTrajectory::ShowTrajectory(os);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Thêm một bước (step) mới vào trajectory (tạo điểm mới)
void WLSTrajectory::AppendStep(const G4Step* aStep)
{
  fpPointsContainer->push_back(new WLSTrajectoryPoint(aStep));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Lấy con trỏ tới định nghĩa hạt dựa trên tên hạt đã lưu
G4ParticleDefinition* WLSTrajectory::GetParticleDefinition()
{
  return (G4ParticleTable::GetParticleTable()->FindParticle(fParticleName));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Gộp trajectory thứ hai vào trajectory hiện tại (dùng cho track bị chia nhỏ)
void WLSTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  if (!secondTrajectory) return;

  auto second = (WLSTrajectory*)secondTrajectory;
  G4int ent = second->GetPointEntries();
  // Không gộp điểm đầu tiên của trajectory thứ hai (trùng với điểm cuối của trajectory trước)
  for (G4int i = 1; i < ent; ++i) {
    fpPointsContainer->push_back((*(second->fpPointsContainer))[i]);
  }
  delete (*second->fpPointsContainer)[0];
  second->fpPointsContainer->clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Định nghĩa các thuộc tính (attributes) cho trajectory để hiển thị hoặc phân tích
const std::map<G4String, G4AttDef>* WLSTrajectory::GetAttDefs() const
{
  G4bool isNew;
  std::map<G4String, G4AttDef>* store = G4AttDefStore::GetInstance("Trajectory", isNew);

  if (isNew) {
    G4String ID("ID");
    (*store)[ID] = G4AttDef(ID, "Track ID", "Bookkeeping", "", "G4int");

    G4String PID("PID");
    (*store)[PID] = G4AttDef(PID, "Parent ID", "Bookkeeping", "", "G4int");

    G4String PN("PN");
    (*store)[PN] = G4AttDef(PN, "Particle Name", "Physics", "", "G4String");

    G4String Ch("Ch");
    (*store)[Ch] = G4AttDef(Ch, "Charge", "Physics", "e+", "G4double");

    G4String PDG("PDG");
    (*store)[PDG] = G4AttDef(PDG, "PDG Encoding", "Physics", "", "G4int");

    G4String IMom("IMom");
    (*store)[IMom] = G4AttDef(IMom, "Momentum of track at start of trajectory", "Physics",
                              "G4BestUnit", "G4ThreeVector");

    G4String IMag("IMag");
    (*store)[IMag] = G4AttDef(IMag, "Magnitude of momentum of track at start of trajectory",
                              "Physics", "G4BestUnit", "G4double");

    G4String NTP("NTP");
    (*store)[NTP] = G4AttDef(NTP, "No. of points", "Bookkeeping", "", "G4int");
  }
  return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Tạo giá trị cho các thuộc tính trajectory (dùng cho hiển thị hoặc xuất dữ liệu)
std::vector<G4AttValue>* WLSTrajectory::CreateAttValues() const
{
  auto values = new std::vector<G4AttValue>;

  values->push_back(G4AttValue("ID", G4UIcommand::ConvertToString(fTrackID), ""));
  values->push_back(G4AttValue("PID", G4UIcommand::ConvertToString(fParentID), ""));
  values->push_back(G4AttValue("PN", fParticleName, ""));
  values->push_back(G4AttValue("Ch", G4UIcommand::ConvertToString(fPDGCharge), ""));
  values->push_back(G4AttValue("PDG", G4UIcommand::ConvertToString(fPDGEncoding), ""));
  values->push_back(G4AttValue("IMom", G4BestUnit(fInitialMomentum, "Energy"), ""));
  values->push_back(G4AttValue("IMag", G4BestUnit(fInitialMomentum.mag(), "Energy"), ""));
  values->push_back(G4AttValue("NTP", G4UIcommand::ConvertToString(GetPointEntries()), ""));

  return values;
}