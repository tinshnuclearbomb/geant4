/// \file optical/wls/src/WLSMaterials.cc
/// \brief Implementation of the WLSMaterials class
//
//
#include "WLSMaterials.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

// Khởi tạo con trỏ singleton cho lớp WLSMaterials
WLSMaterials* WLSMaterials::fInstance = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo: tạo các vật liệu cần thiết cho mô phỏng
WLSMaterials::WLSMaterials()
{
  fNistMan = G4NistManager::Instance(); // Lấy đối tượng quản lý vật liệu chuẩn của Geant4
  fNistMan->SetVerbose(2);              // Đặt mức verbose cho NistManager

  CreateMaterials();                    // Tạo các vật liệu sử dụng trong mô phỏng
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm hủy: giải phóng bộ nhớ cho các vật liệu đã tạo
WLSMaterials::~WLSMaterials()
{
  delete fAir;
  delete fPMMA;
  delete fPethylene;
  delete fFPethylene;
  delete fPolystyrene;
  delete fSilicone;
  delete fCoating;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm singleton: trả về con trỏ duy nhất tới đối tượng WLSMaterials
WLSMaterials* WLSMaterials::GetInstance()
{
  if (!fInstance) {
    fInstance = new WLSMaterials();
  }
  return fInstance;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm lấy vật liệu theo tên (ưu tiên lấy từ NistManager)
G4Material* WLSMaterials::GetMaterial(const G4String material)
{
  G4Material* mat = fNistMan->FindOrBuildMaterial(material);

  if (!mat) mat = G4Material::GetMaterial(material);
  if (!mat) {
    G4ExceptionDescription ed;
    ed << "Material " << material << " not found!";
    G4Exception("WLSMaterials::GetMaterial", "", FatalException, ed);
  }

  return mat;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm tạo các vật liệu và bảng thuộc tính quang học cho từng vật liệu
void WLSMaterials::CreateMaterials()
{
  G4double density;
  G4int ncomponents;
  G4double fractionmass;
  std::vector<G4int> natoms;
  std::vector<G4double> fractionMass;
  std::vector<G4String> elements;

  // Định nghĩa các vật liệu sử dụng trong mô phỏng
  // =====================

  //--------------------------------------------------
  // Vacuum (chân không)
  //--------------------------------------------------
  fNistMan->FindOrBuildMaterial("G4_Galactic");

  //--------------------------------------------------
  // Air (không khí)
  //--------------------------------------------------
  fAir = fNistMan->FindOrBuildMaterial("G4_AIR");

  //--------------------------------------------------
  // WLSfiber PMMA (Polymethyl methacrylate)
  //--------------------------------------------------
  elements.push_back("C"); natoms.push_back(5);
  elements.push_back("H"); natoms.push_back(8);
  elements.push_back("O"); natoms.push_back(2);
  density = 1.190 * g / cm3;
  fPMMA = fNistMan->ConstructNewMaterial("PMMA", elements, natoms, density);
  elements.clear(); natoms.clear();

  //--------------------------------------------------
  // Cladding (polyethylene) - lớp bọc 1
  //--------------------------------------------------
  elements.push_back("C"); natoms.push_back(2);
  elements.push_back("H"); natoms.push_back(4);
  density = 1.200 * g / cm3;
  fPethylene = fNistMan->ConstructNewMaterial("Pethylene", elements, natoms, density);
  elements.clear(); natoms.clear();

  //--------------------------------------------------
  // Double Cladding (fluorinated polyethylene) - lớp bọc 2
  //--------------------------------------------------
  elements.push_back("C"); natoms.push_back(2);
  elements.push_back("H"); natoms.push_back(4);
  density = 1.400 * g / cm3;
  fFPethylene = fNistMan->ConstructNewMaterial("FPethylene", elements, natoms, density);
  elements.clear(); natoms.clear();

  //--------------------------------------------------
  // Polystyrene (nhựa phát sáng)
  //--------------------------------------------------
  elements.push_back("C"); natoms.push_back(8);
  elements.push_back("H"); natoms.push_back(8);
  density = 1.050 * g / cm3;
  fPolystyrene = fNistMan->ConstructNewMaterial("Polystyrene", elements, natoms, density);
  elements.clear(); natoms.clear();

  //--------------------------------------------------
  // Silicone (dùng làm lớp ghép quang học)
  //--------------------------------------------------
  elements.push_back("C"); natoms.push_back(2);
  elements.push_back("H"); natoms.push_back(6);
  density = 1.060 * g / cm3;
  fSilicone = fNistMan->ConstructNewMaterial("Silicone", elements, natoms, density);
  elements.clear(); natoms.clear();

  //--------------------------------------------------
  // Aluminium (nhôm, dùng cho gương hoặc detector)
  //--------------------------------------------------
  fNistMan->FindOrBuildMaterial("G4_Al");

  //--------------------------------------------------
  // TiO2 (Titanium dioxide, dùng cho lớp phủ ngoài)
  //--------------------------------------------------
  elements.push_back("Ti"); natoms.push_back(1);
  elements.push_back("O");  natoms.push_back(2);
  density = 4.26 * g / cm3;
  G4Material* TiO2 = fNistMan->ConstructNewMaterial("TiO2", elements, natoms, density);
  elements.clear(); natoms.clear();

  //--------------------------------------------------
  // Scintillator Coating - 15% TiO2 và 85% polystyrene theo khối lượng
  //--------------------------------------------------
  density = 1.52 * g / cm3;
  fCoating = new G4Material("Coating", density, ncomponents = 2);
  fCoating->AddMaterial(TiO2, fractionmass = 15 * perCent);
  fCoating->AddMaterial(fPolystyrene, fractionmass = 85 * perCent);

  //
  // ------------ Tạo và gán bảng thuộc tính quang học cho vật liệu ------------
  //

  // Dải năng lượng photon sử dụng cho các thuộc tính quang học
  std::vector<G4double> energy = {
    2.00 * eV, 2.03 * eV, 2.06 * eV, 2.09 * eV, 2.12 * eV, 2.15 * eV, 2.18 * eV, 2.21 * eV,
    2.24 * eV, 2.27 * eV, 2.30 * eV, 2.33 * eV, 2.36 * eV, 2.39 * eV, 2.42 * eV, 2.45 * eV,
    2.48 * eV, 2.51 * eV, 2.54 * eV, 2.57 * eV, 2.60 * eV, 2.63 * eV, 2.66 * eV, 2.69 * eV,
    2.72 * eV, 2.75 * eV, 2.78 * eV, 2.81 * eV, 2.84 * eV, 2.87 * eV, 2.90 * eV, 2.93 * eV,
    2.96 * eV, 2.99 * eV, 3.02 * eV, 3.05 * eV, 3.08 * eV, 3.11 * eV, 3.14 * eV, 3.17 * eV,
    3.20 * eV, 3.23 * eV, 3.26 * eV, 3.29 * eV, 3.32 * eV, 3.35 * eV, 3.38 * eV, 3.41 * eV,
    3.44 * eV, 3.47 * eV};

  std::vector<G4double> energySmall = {2.0 * eV, 3.47 * eV};

  //--------------------------------------------------
  // Air: chỉ số khúc xạ
  //--------------------------------------------------
  std::vector<G4double> refractiveIndex = {1.0, 1.0};
  auto mpt = new G4MaterialPropertiesTable();
  mpt->AddProperty("RINDEX", energySmall, refractiveIndex);
  fAir->SetMaterialPropertiesTable(mpt);

  //--------------------------------------------------
  // PMMA cho sợi quang WLS
  //--------------------------------------------------
  std::vector<G4double> refractiveIndexWLSfiber = {1.60, 1.60};
  std::vector<G4double> absWLSfiber = {
    5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m,
    5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m,
    5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m, 5.40 * m,
    5.40 * m, 5.40 * m, 1.10 * m, 1.10 * m, 1.10 * m, 1.10 * m, 1.10 * m, 1.10 * m, 1.10 * m,
    1. * mm,  1. * mm,  1. * mm,  1. * mm,  1. * mm,  1. * mm,  1. * mm,  1. * mm,  1. * mm,
    1. * mm,  1. * mm,  1. * mm,  1. * mm,  1. * mm};
  std::vector<G4double> emissionFib = {0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
                                       3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
                                       12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
                                       15.0, 9.00, 2.50, 1.00, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00,
                                       0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};
  auto mptWLSfiber = new G4MaterialPropertiesTable();
  mptWLSfiber->AddProperty("RINDEX", energySmall, refractiveIndexWLSfiber);
  mptWLSfiber->AddProperty("WLSABSLENGTH", energy, absWLSfiber);
  mptWLSfiber->AddProperty("WLSCOMPONENT", energy, emissionFib);
  mptWLSfiber->AddConstProperty("WLSTIMECONSTANT", 0.5 * ns);
  fPMMA->SetMaterialPropertiesTable(mptWLSfiber);

  //--------------------------------------------------
  // Polyethylene (cladding 1)
  //--------------------------------------------------
  std::vector<G4double> refractiveIndexClad1 = {1.49, 1.49};
  std::vector<G4double> absClad = {20.0 * m, 20.0 * m};
  auto mptClad1 = new G4MaterialPropertiesTable();
  mptClad1->AddProperty("RINDEX", energySmall, refractiveIndexClad1);
  mptClad1->AddProperty("ABSLENGTH", energySmall, absClad);
  fPethylene->SetMaterialPropertiesTable(mptClad1);

  //--------------------------------------------------
  // Fluorinated Polyethylene (cladding 2)
  //--------------------------------------------------
  std::vector<G4double> refractiveIndexClad2 = {1.42, 1.42};
  auto mptClad2 = new G4MaterialPropertiesTable();
  mptClad2->AddProperty("RINDEX", energySmall, refractiveIndexClad2);
  mptClad2->AddProperty("ABSLENGTH", energySmall, absClad);
  fFPethylene->SetMaterialPropertiesTable(mptClad2);

  //--------------------------------------------------
  // Silicone (lớp ghép quang học)
  //--------------------------------------------------
  std::vector<G4double> refractiveIndexSilicone = {1.46, 1.46};
  auto mptSilicone = new G4MaterialPropertiesTable();
  mptSilicone->AddProperty("RINDEX", energySmall, refractiveIndexSilicone);
  mptSilicone->AddProperty("ABSLENGTH", energySmall, absClad);
  fSilicone->SetMaterialPropertiesTable(mptSilicone);

  //--------------------------------------------------
  // Polystyrene (nhựa phát sáng)
  //--------------------------------------------------
  std::vector<G4double> refractiveIndexPS = {1.50, 1.50};
  std::vector<G4double> absPS = {2. * cm, 2. * cm};
  std::vector<G4double> scintilFast = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  auto mptPolystyrene = new G4MaterialPropertiesTable();
  mptPolystyrene->AddProperty("RINDEX", energySmall, refractiveIndexPS);
  mptPolystyrene->AddProperty("ABSLENGTH", energySmall, absPS);
  mptPolystyrene->AddProperty("SCINTILLATIONCOMPONENT1", energy, scintilFast);
  mptPolystyrene->AddConstProperty("SCINTILLATIONYIELD", 10. / keV);
  mptPolystyrene->AddConstProperty("RESOLUTIONSCALE", 1.0);
  mptPolystyrene->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 10. * ns);
  fPolystyrene->SetMaterialPropertiesTable(mptPolystyrene);

  // Đặt hằng số Birks cho nhựa phát sáng Polystyrene
  fPolystyrene->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
}

