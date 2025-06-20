//
// Triển khai lớp WLSDetectorConstruction - xây dựng hình học máy dò cho mô phỏng Geant4.
//

#include "WLSDetectorConstruction.hh"
#include "WLSDetectorMessenger.hh"
#include "WLSMaterials.hh"
#include "WLSPhotonDetSD.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4EllipticalTube.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SolidStore.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"
#include "globals.hh"

// Hàm khởi tạo: thiết lập các giá trị mặc định và tạo messenger để điều khiển tham số hình học
WLSDetectorConstruction::WLSDetectorConstruction() : fVisAttributes()
{
  fDetectorMessenger = new WLSDetectorMessenger(this);

  fMPPCHalfL = fWLSfiberRY;
  fClrfiberZ = fMPPCZ + 10. * nm;
  fHoleLength = fBarLength;
}

// Hàm hủy: giải phóng bộ nhớ cho các đối tượng đã cấp phát động
WLSDetectorConstruction::~WLSDetectorConstruction()
{
  delete fDetectorMessenger;
  delete fMaterials;
  for (auto visAttributes : fVisAttributes) {
    delete visAttributes;
  }
}

// Hàm xây dựng hình học chính của máy dò
G4VPhysicalVolume* WLSDetectorConstruction::Construct()
{
  // Nếu đã có hình học cũ, dọn dẹp trước khi xây dựng lại
  if (fPhysiWorld) {
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    G4LogicalSkinSurface::CleanSurfaceTable();
    G4LogicalBorderSurface::CleanSurfaceTable();
  }

  fMaterials = WLSMaterials::GetInstance(); // Lấy vật liệu
  UpdateGeometryParameters();               // Cập nhật các thông số hình học

  return ConstructDetector();               // Xây dựng các thành phần máy dò
}

// Hàm xây dựng các thành phần chi tiết của máy dò
G4VPhysicalVolume* WLSDetectorConstruction::ConstructDetector()
{
  // 1. Tạo thế giới (World)
  auto air = FindMaterial("G4_AIR");
  G4VSolid* solidWorld = new G4Box("World", fWorldSizeX, fWorldSizeY, fWorldSizeZ);
  fLogicWorld = new G4LogicalVolume(solidWorld, air, "World");
  fPhysiWorld = new G4PVPlacement(nullptr, G4ThreeVector(), fLogicWorld, "World", nullptr, false, 0);

  // 2. Tạo lớp phủ ngoài (Extrusion)
  auto coating = FindMaterial("Coating");
  G4VSolid* solidExtrusion = new G4Box("Extrusion", GetBarBase() / 2., GetBarBase() / 2., GetBarLength() / 2.);
  auto logicExtrusion = new G4LogicalVolume(solidExtrusion, coating, "Extrusion");

  // Tạo bề mặt quang học cho lớp phủ
  auto TiO2Surface = new G4OpticalSurface("TiO2Surface", glisur, ground, dielectric_metal, fExtrusionPolish);
  auto TiO2SurfaceProperty = new G4MaterialPropertiesTable();
  std::vector<G4double> p_TiO2 = {2.00 * eV, 3.47 * eV};
  std::vector<G4double> refl_TiO2 = {fExtrusionReflectivity, fExtrusionReflectivity};
  std::vector<G4double> effi_TiO2 = {0., 0.};
  TiO2SurfaceProperty->AddProperty("REFLECTIVITY", p_TiO2, refl_TiO2);
  TiO2SurfaceProperty->AddProperty("EFFICIENCY", p_TiO2, effi_TiO2);
  TiO2Surface->SetMaterialPropertiesTable(TiO2SurfaceProperty);

  new G4PVPlacement(nullptr, G4ThreeVector(), logicExtrusion, "Extrusion", fLogicWorld, false, 0);
  new G4LogicalSkinSurface("TiO2Surface", logicExtrusion, TiO2Surface);

  // 3. Tạo thanh nhựa phát sáng (Scintillator)
  auto polystyrene = FindMaterial("Polystyrene");
  G4VSolid* solidScintillator = new G4Box("Scintillator",
    GetBarBase() / 2. - GetCoatingThickness() - GetCoatingRadius(),
    GetBarBase() / 2. - GetCoatingThickness() - GetCoatingRadius(),
    GetBarLength() / 2.);
  auto logicScintillator = new G4LogicalVolume(solidScintillator, polystyrene, "Scintillator");
  new G4PVPlacement(nullptr, G4ThreeVector(), logicScintillator, "Scintillator", logicExtrusion, false, 0);

  // 4. Tạo các cạnh bo tròn và mặt bên nếu có lớp phủ bo tròn
  G4LogicalVolume* logicScintSide = nullptr;
  G4LogicalVolume* logicScintCrnr = nullptr;
  if (GetCoatingRadius() > 0.) {
    // Tạo hình học cho mặt bên và góc bo tròn
    G4VSolid* solidScintside = new G4Box("SideOfBar",
      GetBarBase() / 2. - GetCoatingThickness() - GetCoatingRadius(),
      GetCoatingRadius() / 2., GetBarLength() / 2.);
    G4VSolid* solidScintcrnr = new G4Tubs("CrnrOfBar", 0.0 * cm, GetCoatingRadius(),
      GetBarLength() / 2., 0. * deg, 90. * deg);

    logicScintSide = new G4LogicalVolume(solidScintside, polystyrene, "SideOfBar");
    logicScintCrnr = new G4LogicalVolume(solidScintcrnr, polystyrene, "CrnrOfBar");

    // Đặt các mặt bên và góc vào hình học
    G4double pos = GetBarBase() / 2. - GetCoatingThickness() - GetCoatingRadius() / 2.;
    new G4PVPlacement(nullptr, G4ThreeVector(0., -pos, 0.), logicScintSide, "SideOfBar", logicExtrusion, false, 0);
    new G4PVPlacement(nullptr, G4ThreeVector(0., pos, 0.), logicScintSide, "SideOfBar", logicExtrusion, false, 1);

    auto rot1 = new G4RotationMatrix();
    rot1->rotateZ(-90. * deg);
    new G4PVPlacement(rot1, G4ThreeVector(pos, 0., 0.), logicScintSide, "SideOfBar", logicExtrusion, false, 2);
    new G4PVPlacement(rot1, G4ThreeVector(-pos, 0., 0.), logicScintSide, "SideOfBar", logicExtrusion, false, 3);

    pos = GetBarBase() / 2. - GetCoatingThickness() - GetCoatingRadius();
    new G4PVPlacement(nullptr, G4ThreeVector(pos, pos, 0.), logicScintCrnr, "CrnrOfBar", logicExtrusion, false, 0);
    new G4PVPlacement(rot1, G4ThreeVector(-pos, pos, 0.), logicScintCrnr, "CrnrOfBar", logicExtrusion, false, 1);

    auto rot2 = new G4RotationMatrix();
    rot2->rotateZ(-180. * deg);
    new G4PVPlacement(rot2, G4ThreeVector(-pos, -pos, 0.), logicScintCrnr, "CrnrOfBar", logicExtrusion, false, 2);

    auto rot3 = new G4RotationMatrix();
    rot3->rotateZ(-270. * deg);
    new G4PVPlacement(rot3, G4ThreeVector(pos, -pos, 0.), logicScintCrnr, "CrnrOfBar", logicExtrusion, false, 3);
  }

  // 5. Tạo lỗ cho sợi quang nếu cần
  if (GetFiberRadius() < GetHoleRadius()) {
    G4VSolid* solidHole = new G4Tubs("Hole", 0., GetHoleRadius(), GetHoleLength() / 2., 0. * deg, 360. * deg);
    fLogicHole = new G4LogicalVolume(solidHole, air, "Hole");
    fPhysiHole = new G4PVPlacement(nullptr, G4ThreeVector(), fLogicHole, "Hole", logicScintillator, false, 0);
  }

  // 6. Tạo sợi quang và các lớp bọc (cladding)
  if (!(fLogicHole) || !(fPhysiHole)) {
    G4ExceptionDescription ed;
    ed << "The Fiber Hole has not been constructed";
    G4Exception("WLSDetectorConstruction", "wls001", FatalException, ed);
  }
  G4LogicalVolume* logicPlacement = fLogicHole;
  G4VPhysicalVolume* physiPlacement = fPhysiHole;

  // Tạo bề mặt quang học cho sợi quang nếu cần
  G4OpticalSurface* opSurface = nullptr;
  if (fSurfaceRoughness < 1.)
    opSurface = new G4OpticalSurface("RoughSurface", glisur, ground, dielectric_dielectric, fSurfaceRoughness);

  G4LogicalVolume* logicWLSfiber = nullptr;
  G4LogicalVolume* logicClad1 = nullptr;
  G4LogicalVolume* logicClad2 = nullptr;
  G4VPhysicalVolume* physiClad1 = nullptr;
  G4VPhysicalVolume* physiClad2 = nullptr;

  auto fpethylene = FindMaterial("FPethylene");
  auto pethylene = FindMaterial("Pethylene");
  auto pmma = FindMaterial("PMMA");

  // Xây dựng các lớp bọc (cladding) và sợi quang tùy theo số lớp
  switch (fNumOfCladLayers) {
    case 2:
      // Lớp bọc thứ 2
      G4VSolid* solidClad2 = (fXYRatio == 1.)
        ? static_cast<G4VSolid*>(new G4Tubs("Clad2", 0., fClad2RX, fClad2Z, 0., twopi))
        : static_cast<G4VSolid*>(new G4EllipticalTube("Clad2", fClad2RX, fClad2RY, fClad2Z));
      logicClad2 = new G4LogicalVolume(solidClad2, fpethylene, "Clad2");
      physiClad2 = new G4PVPlacement(nullptr, G4ThreeVector(0.0, 0.0, fWLSfiberOrigin), logicClad2, "Clad2", logicPlacement, false, 0);
      if (opSurface) {
        new G4LogicalBorderSurface("surfaceClad2Out", physiClad2, physiPlacement, opSurface);
        new G4LogicalBorderSurface("surfaceClad2In", physiPlacement, physiClad2, opSurface);
      }
      logicPlacement = logicClad2;
      physiPlacement = physiClad2;
      [[fallthrough]];
    case 1:
      // Lớp bọc thứ 1
      G4VSolid* solidClad1 = (fXYRatio == 1.)
        ? static_cast<G4VSolid*>(new G4Tubs("Clad1", 0., fClad1RX, fClad1Z, 0., twopi))
        : static_cast<G4VSolid*>(new G4EllipticalTube("Clad1", fClad1RX, fClad1RY, fClad1Z));
      logicClad1 = new G4LogicalVolume(solidClad1, pethylene, "Clad1");
      physiClad1 = new G4PVPlacement(nullptr, G4ThreeVector(0., 0., fWLSfiberOrigin), logicClad1, "Clad1", logicPlacement, false, 0);
      if (opSurface) {
        new G4LogicalBorderSurface("surfaceClad1Out", physiClad1, physiPlacement, opSurface);
        new G4LogicalBorderSurface("surfaceClad1In", physiPlacement, physiClad1, opSurface);
      }
      logicPlacement = logicClad1;
      physiPlacement = physiClad1;
      [[fallthrough]];
    default:
      // Sợi quang WLS
      G4VSolid* solidWLSfiber = (fXYRatio == 1.)
        ? static_cast<G4VSolid*>(new G4Tubs("WLSFiber", 0., fWLSfiberRX, fWLSfiberZ, 0., twopi))
        : static_cast<G4VSolid*>(new G4EllipticalTube("WLSFiber", fWLSfiberRX, fWLSfiberRY, fWLSfiberZ));
      logicWLSfiber = new G4LogicalVolume(solidWLSfiber, pmma, "WLSFiber");
      logicWLSfiber->SetUserLimits(new G4UserLimits(DBL_MAX, DBL_MAX, 10. * ms));
      G4VPhysicalVolume* physiWLSfiber = new G4PVPlacement(nullptr, G4ThreeVector(0., 0., fWLSfiberOrigin), logicWLSfiber, "WLSFiber", logicPlacement, false, 0);
      if (opSurface) {
        new G4LogicalBorderSurface("surfaceWLSOut", physiWLSfiber, physiPlacement, opSurface);
        new G4LogicalBorderSurface("surfaceWLSIn", physiPlacement, physiWLSfiber, opSurface);
      }
  }

  // 7. Tạo gương phản xạ ở một đầu sợi quang nếu được bật
  G4LogicalVolume* logicMirror = nullptr;
  auto aluminum = FindMaterial("G4_Al");
  if (fMirrorToggle) {
    G4VSolid* solidMirror = new G4Box("Mirror", fMirrorRmax, fMirrorRmax, fMirrorZ);
    logicMirror = new G4LogicalVolume(solidMirror, aluminum, "Mirror");
    auto mirrorSurface = new G4OpticalSurface("MirrorSurface", glisur, ground, dielectric_metal, fMirrorPolish);
    auto mirrorSurfaceProperty = new G4MaterialPropertiesTable();
    std::vector<G4double> p_mirror = {2.00 * eV, 3.47 * eV};
    std::vector<G4double> refl_mirror = {fMirrorReflectivity, fMirrorReflectivity};
    std::vector<G4double> effi_mirror = {0., 0.};
    mirrorSurfaceProperty->AddProperty("REFLECTIVITY", p_mirror, refl_mirror);
    mirrorSurfaceProperty->AddProperty("EFFICIENCY", p_mirror, effi_mirror);
    mirrorSurface->SetMaterialPropertiesTable(mirrorSurfaceProperty);
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., fMirrorOrigin), logicMirror, "Mirror", fLogicWorld, false, 0);
    new G4LogicalSkinSurface("MirrorSurface", logicMirror, mirrorSurface);
  }

  // 8. Tạo lớp ghép nối (Coupling Layer) ở đầu đọc
  G4VSolid* solidCouple = new G4Box("Couple", fCoupleRX, fCoupleRY, fCoupleZ);
  auto logicCouple = new G4LogicalVolume(solidCouple, air, "Couple");
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., fCoupleOrigin), logicCouple, "Couple", fLogicWorld, false, 0);

  // 9. Tạo lớp clear fiber (ClearFiber) để tránh tiếp xúc trực tiếp giữa dielectrics và kim loại
  if (fMPPCTheta > std::atan(fMPPCDist / fMPPCHalfL)) {
    fMPPCTheta = 0.;
    fMPPCOriginX = std::sin(fMPPCTheta) * (fMPPCDist + fClrfiberZ);
    fMPPCOriginZ = -fCoupleZ + std::cos(fMPPCTheta) * (fMPPCDist + fClrfiberZ);
    G4ExceptionDescription ed;
    ed << "Invalid alignment.  Alignment reset to 0.";
    G4Exception("WLSDetectorConstruction", "wls002", JustWarning, ed);
  }
  G4VSolid* solidClrfiber;
  if (fMPPCShape == "Square") {
    solidClrfiber = new G4Box("ClearFiber", fClrfiberHalfL, fClrfiberHalfL, fClrfiberZ);
  } else {
    solidClrfiber = new G4Tubs("ClearFiber", 0., fClrfiberHalfL, fClrfiberZ, 0., twopi);
  }
  auto logicClrfiber = new G4LogicalVolume(solidClrfiber, air, "ClearFiber");
  new G4PVPlacement(new G4RotationMatrix(CLHEP::HepRotationY(-fMPPCTheta)),
    G4ThreeVector(fMPPCOriginX, 0.0, fMPPCOriginZ), logicClrfiber, "ClearFiber", logicCouple, false, 0);

  // 10. Tạo detector nhạy (PhotonDet)
  G4VSolid* solidPhotonDet = nullptr;
  if (fMPPCShape == "Square")
    solidPhotonDet = new G4Box("PhotonDet", fMPPCHalfL, fMPPCHalfL, fMPPCZ);
  else
    solidPhotonDet = new G4Tubs("PhotonDet", 0., fMPPCHalfL, fMPPCZ, 0., twopi);
  auto logicPhotonDet = new G4LogicalVolume(solidPhotonDet, aluminum, "PhotonDet_LV");
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), logicPhotonDet, "PhotonDet", logicClrfiber, false, 0);

  // Thiết lập bề mặt quang học cho PhotonDet
  auto photonDetSurface = new G4OpticalSurface("PhotonDetSurface", glisur, ground, dielectric_metal, fMPPCPolish);
  auto photonDetSurfaceProperty = new G4MaterialPropertiesTable();
  std::vector<G4double> p_mppc = {2.00 * eV, 3.47 * eV};
  std::vector<G4double> refl_mppc = {fMPPCReflectivity, fMPPCReflectivity};
  std::vector<G4double> effi_mppc = {1., 1.};
  photonDetSurfaceProperty->AddProperty("REFLECTIVITY", p_mppc, refl_mppc);
  photonDetSurfaceProperty->AddProperty("EFFICIENCY", p_mppc, effi_mppc);
  photonDetSurface->SetMaterialPropertiesTable(photonDetSurfaceProperty);
  new G4LogicalSkinSurface("PhotonDetSurface", logicPhotonDet, photonDetSurface);

  // 11. Thiết lập thuộc tính hiển thị cho các thành phần (visualization)
  // (Các đoạn này giúp hiển thị màu sắc, độ trong suốt của từng thành phần khi xem hình học)
  auto visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  visAttributes->SetVisibility(false);
  fLogicWorld->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2, 0.5));
  visAttributes->SetVisibility(true);
  logicExtrusion->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.9));
  visAttributes->SetVisibility(true);
  logicScintillator->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(0.0, 0.8, 0.2, 0.2));
  visAttributes->SetVisibility(true);
  logicScintSide->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(0.0, 0.8, 0.2, 0.2));
  visAttributes->SetVisibility(true);
  logicScintCrnr->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(0.4, 0.0, 0.0, 0.5));
  visAttributes->SetVisibility(true);
  fLogicHole->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  if (logicClad1 != nullptr) {
    visAttributes = new G4VisAttributes(G4Colour(0.0, 0.8, 0.5, 0.5));
    visAttributes->SetVisibility(true);
    logicClad1->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);
  }

  if (logicClad2 != nullptr) {
    visAttributes = new G4VisAttributes(G4Colour(0.0, 0.5, 0.8, 0.5));
    visAttributes->SetVisibility(true);
    logicClad2->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);
  }

  visAttributes = new G4VisAttributes(G4Colour(0.8, 0.8, 1.0));
  visAttributes->SetVisibility(true);
  logicWLSfiber->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  if (fMirrorToggle == true) {
    visAttributes = new G4VisAttributes(G4Colour(0.3, 0.3, 1.0, 0.3));
    visAttributes->SetVisibility(true);
    logicMirror->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);
  }

  visAttributes = new G4VisAttributes(G4Colour(0.0, 0.0, 0.5, 0.5));
  visAttributes->SetVisibility(true);
  logicCouple->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(0.3, 0.3, 0.3, 0.5));
  visAttributes->SetVisibility(true);
  logicClrfiber->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.8));
  visAttributes->SetVisibility(true);
  logicPhotonDet->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  return fPhysiWorld;
}

// Thiết lập detector nhạy cho PhotonDet
void WLSDetectorConstruction::ConstructSDandField()
{
  if (!fmppcSD.Get()) {
    G4String mppcSDName = "WLS/PhotonDet";
    auto mppcSD = new WLSPhotonDetSD(mppcSDName);
    G4SDManager::GetSDMpointer()->AddNewDetector(mppcSD);
    fmppcSD.Put(mppcSD);
  }
  SetSensitiveDetector("PhotonDet_LV", fmppcSD.Get(), true);
}

// Cập nhật các thông số hình học dựa trên các biến cấu hình hiện tại
void WLSDetectorConstruction::UpdateGeometryParameters()
{
  fWLSfiberRX = fXYRatio * fWLSfiberRY;
  fClad1RX = fWLSfiberRX + 0.03 * fWLSfiberRX;
  fClad1RY = fWLSfiberRY + 0.03 * fWLSfiberRY;
  fClad1Z = fWLSfiberZ;
  fClad2RX = fClad1RX + 0.03 * fWLSfiberRX;
  fClad2RY = fClad1RY + 0.03 * fWLSfiberRY;
  fClad2Z = fWLSfiberZ;
  fWorldSizeX = fClad2RX + fMPPCDist + fMPPCHalfL + 1. * cm;
  fWorldSizeY = fClad2RY + fMPPCDist + fMPPCHalfL + 1. * cm;
  fWorldSizeZ = fWLSfiberZ + fMPPCDist + fMPPCHalfL + 1. * cm;
  fCoupleRX = fWorldSizeX;
  fCoupleRY = fWorldSizeY;
  fCoupleZ = (fWorldSizeZ - fWLSfiberZ) / 2.;
  fClrfiberHalfL = fMPPCHalfL;
  fMirrorRmax = fClad2RY;
  fCoupleOrigin = fWLSfiberOrigin + fWLSfiberZ + fCoupleZ;
  fMirrorOrigin = fWLSfiberOrigin - fWLSfiberZ - fMirrorZ;
  fMPPCOriginX = std::sin(fMPPCTheta) * (fMPPCDist + fClrfiberZ);
  fMPPCOriginZ = -fCoupleZ + std::cos(fMPPCTheta) * (fMPPCDist + fClrfiberZ);
}

// Các hàm setter cho phép thay đổi các thông số hình học từ bên ngoài (ví dụ: qua macro)
void WLSDetectorConstruction::SetPhotonDetGeometry(G4String shape) { if (shape == "Circle" || shape == "Square") fMPPCShape = shape; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetNumberOfCladding(G4int num) { fNumOfCladLayers = num; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetWLSLength(G4double length) { fWLSfiberZ = length; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetWLSRadius(G4double radius) { fWLSfiberRY = radius; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetClad1Radius(G4double radius) { fClad1RY = radius; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetClad2Radius(G4double radius) { fClad2RY = radius; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetPhotonDetHalfLength(G4double halfL) { fMPPCHalfL = halfL; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetGap(G4double gap) { fMPPCDist = gap; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetPhotonDetAlignment(G4double theta) { fMPPCTheta = theta; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetSurfaceRoughness(G4double roughness) { fSurfaceRoughness = roughness; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetMirrorPolish(G4double polish) { fMirrorPolish = polish; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetMirrorReflectivity(G4double reflectivity) { fMirrorReflectivity = reflectivity; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetPhotonDetPolish(G4double polish) { fMPPCPolish = polish; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetPhotonDetReflectivity(G4double reflectivity) { fMPPCReflectivity = reflectivity; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetMirror(G4bool flag) { fMirrorToggle = flag; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetXYRatio(G4double r) { fXYRatio = r; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetBarLength(G4double length) { fBarLength = length; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetBarBase(G4double side) { fBarBase = side; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetHoleRadius(G4double radius) { fHoleRadius = radius; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetCoatingThickness(G4double thick) { fCoatingThickness = thick; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }
void WLSDetectorConstruction::SetCoatingRadius(G4double radius) { fCoatingRadius = radius; G4RunManager::GetRunManager()->GeometryHasBeenModified(); }

// Các hàm getter trả về thông số hình học hiện tại
G4double WLSDetectorConstruction::GetWLSFiberLength() { return fWLSfiberZ; }
G4double WLSDetectorConstruction::GetBarLength() { return fBarLength; }
G4double WLSDetectorConstruction::GetBarBase() { return fBarBase; }
G4double WLSDetectorConstruction::GetHoleRadius() { return fHoleRadius; }
G4double WLSDetectorConstruction::GetHoleLength() { return fHoleLength; }
G4double WLSDetectorConstruction::GetFiberRadius() { return GetWLSFiberRMax(); }
G4double WLSDetectorConstruction::GetCoatingThickness() { return fCoatingThickness; }
G4double WLSDetectorConstruction::GetCoatingRadius() { return fCoatingRadius; }
G4double WLSDetectorConstruction::GetWLSFiberEnd() { return fWLSfiberOrigin + fWLSfiberZ; }
G4double WLSDetectorConstruction::GetWLSFiberRMax() {
  if (fNumOfCladLayers == 2) return fClad2RY;
  if (fNumOfCladLayers == 1) return fClad1RY;
  return fWLSfiberRY;
}
G4double WLSDetectorConstruction::GetSurfaceRoughness() { return fSurfaceRoughness; }

// Kiểm tra xem sợi quang có lý tưởng không (bề mặt nhẵn, tròn đều, gương hoàn hảo)
G4bool WLSDetectorConstruction::IsPerfectFiber()
{
  return fSurfaceRoughness == 1. && fXYRatio == 1.
         && (!fMirrorToggle || (fMirrorPolish == 1. && fMirrorReflectivity == 1.));
}

// Hàm tìm vật liệu theo tên
G4Material* WLSDetectorConstruction::FindMaterial(G4String name)
{
  G4Material* material = G4Material::GetMaterial(name, true);
  return material;
}