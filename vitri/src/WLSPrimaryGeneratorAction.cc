/// \file optical/wls/src/WLSPrimaryGeneratorAction.cc
/// \brief Implementation of the WLSPrimaryGeneratorAction class
//
//

#include "WLSPrimaryGeneratorAction.hh"

#include "WLSDetectorConstruction.hh"
#include "WLSPrimaryGeneratorMessenger.hh"

#include "G4AutoLock.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

namespace
{
G4Mutex gen_mutex = G4MUTEX_INITIALIZER; // Mutex để đảm bảo thread-safe khi sinh photon
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Biến static để kiểm tra lần đầu sinh photon
G4bool WLSPrimaryGeneratorAction::fFirst = false;

// Hàm khởi tạo: khởi tạo nguồn phát hạt và messenger cho điều khiển UI
WLSPrimaryGeneratorAction::WLSPrimaryGeneratorAction(WLSDetectorConstruction* dc)
{
  fDetector = dc;

  // Sử dụng GeneralParticleSource để phát hạt (cấu hình qua macro)
  fParticleGun = new G4GeneralParticleSource();
  // Messenger cho phép điều khiển các tham số phát hạt qua UI
  fGunMessenger = new WLSPrimaryGeneratorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm hủy: giải phóng bộ nhớ các đối tượng đã cấp phát động
WLSPrimaryGeneratorAction::~WLSPrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fGunMessenger;
  if (fIntegralTable) {
    fIntegralTable->clearAndDestroy();
    delete fIntegralTable;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Đặt hằng số thời gian phát xạ trễ (decay time constant) cho photon WLS
void WLSPrimaryGeneratorAction::SetDecayTimeConstant(G4double time)
{
  fTimeConstant = time;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Xây dựng bảng tích phân phổ phát xạ (emission spectrum) cho từng vật liệu
void WLSPrimaryGeneratorAction::BuildEmissionSpectrum()
{
  if (fIntegralTable) return; // Nếu đã có bảng thì không cần xây lại

  const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();
  G4int numOfMaterials = G4Material::GetNumberOfMaterials();

  if (!fIntegralTable) fIntegralTable = new G4PhysicsTable(numOfMaterials);

  // Duyệt qua từng vật liệu, lấy phổ phát xạ WLSCOMPONENT nếu có
  for (G4int i = 0; i < numOfMaterials; ++i) {
    auto vec = new G4PhysicsFreeVector();

    G4MaterialPropertiesTable* MPT = (*theMaterialTable)[i]->GetMaterialPropertiesTable();

    if (MPT) {
      G4MaterialPropertyVector* theWLSVector = MPT->GetProperty("WLSCOMPONENT");

      if (theWLSVector) {
        G4double currentIN = (*theWLSVector)[0];
        if (currentIN >= 0.0) {
          G4double currentPM = theWLSVector->Energy(0);
          G4double currentCII = 0.0;
          vec->InsertValues(currentPM, currentCII);
          G4double prevPM = currentPM;
          G4double prevCII = currentCII;
          G4double prevIN = currentIN;

          // Tích phân phổ phát xạ để lấy bảng phân bố xác suất cộng dồn (CDF)
          for (size_t j = 1; j < theWLSVector->GetVectorLength(); ++j) {
            currentPM = theWLSVector->Energy(j);
            currentIN = (*theWLSVector)[j];
            currentCII = 0.5 * (prevIN + currentIN);
            currentCII = prevCII + (currentPM - prevPM) * currentCII;
            vec->InsertValues(currentPM, currentCII);
            prevPM = currentPM;
            prevCII = currentCII;
            prevIN = currentIN;
          }
        }
      }
    }
    fIntegralTable->insertAt(i, vec);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm sinh hạt sơ cấp cho mỗi event
void WLSPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // Chỉ xây dựng bảng phổ phát xạ một lần duy nhất
  if (!fFirst) {
    fFirst = true;
    BuildEmissionSpectrum();
  }

  // Nếu bật chế độ lấy năng lượng mẫu từ phổ phát xạ WLS
  if (fUseSampledEnergy) {
    const G4MaterialTable* theMaterialTable = G4Material::GetMaterialTable();

    G4double sampledEnergy = 3. * eV; // Giá trị mặc định

    // Duyệt qua các vật liệu, tìm PMMA và lấy năng lượng mẫu từ phổ phát xạ
    for (size_t j = 0; j < theMaterialTable->size(); ++j) {
      G4Material* fMaterial = (*theMaterialTable)[j];
      if (fMaterial->GetName() == "PMMA") {
        auto WLSIntensity = fMaterial->GetMaterialPropertiesTable()->GetProperty("WLSCOMPONENT");

        if (WLSIntensity) {
          auto WLSIntegral = (G4PhysicsFreeVector*)((*fIntegralTable)(fMaterial->GetIndex()));

          G4double CIImax = WLSIntegral->GetMaxValue();
          G4double CIIvalue = G4UniformRand() * CIImax;

          sampledEnergy = WLSIntegral->GetEnergy(CIIvalue);
        }
      }
    }

    // Đặt năng lượng cho nguồn phát hạt thông qua lệnh UI (không thread-safe)
    G4String cmd = "/gun/energy " + G4UIcommand::ConvertToString(sampledEnergy / eV) + " eV";
    G4UImanager::GetUIpointer()->ApplyCommand(cmd);
  }

  // Đoạn code dưới đây không thread-safe vì các thuộc tính của GPS là global
  G4AutoLock l(&gen_mutex);
  if (fParticleGun->GetParticleDefinition() == G4OpticalPhoton::Definition()) {
    SetOptPhotonPolar(); // Đặt phân cực ngẫu nhiên cho photon quang học
    SetOptPhotonTime();  // Đặt thời gian phát ngẫu nhiên theo hàm mũ
  }

  fParticleGun->GeneratePrimaryVertex(anEvent); // Sinh vertex sơ cấp cho event
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Đặt phân cực ngẫu nhiên cho photon quang học
void WLSPrimaryGeneratorAction::SetOptPhotonPolar()
{
  G4double angle = G4UniformRand() * 360.0 * deg;
  SetOptPhotonPolar(angle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Đặt phân cực photon quang học theo góc cho trước
void WLSPrimaryGeneratorAction::SetOptPhotonPolar(G4double angle)
{
  if (fParticleGun->GetParticleDefinition()->GetParticleName() != "opticalphoton") {
    G4cout << "-> warning from WLSPrimaryGeneratorAction::SetOptPhotonPolar()"
           << ":  the ParticleGun is not an opticalphoton" << G4endl;
    return;
  }

  // Xác định các vector trực giao với hướng truyền của photon
  G4ThreeVector normal(1., 0., 0.);
  G4ThreeVector kphoton = fParticleGun->GetParticleMomentumDirection();
  G4ThreeVector product = normal.cross(kphoton);
  G4double modul2 = product * product;

  G4ThreeVector e_perpend(0., 0., 1.);
  if (modul2 > 0.) e_perpend = (1. / std::sqrt(modul2)) * product;
  G4ThreeVector e_paralle = e_perpend.cross(kphoton);

  // Tính vector phân cực dựa trên góc
  G4ThreeVector polar = std::cos(angle) * e_paralle + std::sin(angle) * e_perpend;
  fParticleGun->SetParticlePolarization(polar);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Đặt thời gian phát photon quang học theo phân bố mũ (hàm decay)
void WLSPrimaryGeneratorAction::SetOptPhotonTime()
{
  G4double time = -std::log(G4UniformRand()) * fTimeConstant;
  fParticleGun->SetParticleTime(time);
}