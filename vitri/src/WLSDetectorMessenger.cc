/
/// \file optical/wls/src/WLSDetectorMessenger.cc
/// \brief Implementation of the WLSDetectorMessenger class
//
//
#include "WLSDetectorMessenger.hh"

#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIdirectory.hh"

// Hàm khởi tạo: thiết lập các lệnh UI cho phép điều khiển tham số hình học của máy dò
WLSDetectorMessenger::WLSDetectorMessenger(WLSDetectorConstruction* det) : fDetector(det)
{
  // Tạo thư mục lệnh UI gốc cho detector
  fDetDir = new G4UIdirectory("/WLS/");
  fDetDir->SetGuidance(" Geometry Setup ");

  // Lệnh chọn hình dạng của PhotonDet (Circle hoặc Square)
  fSetPhotonDetGeometryCmd = new G4UIcmdWithAString("/WLS/setPhotonDetGeometry", this);
  fSetPhotonDetGeometryCmd->SetGuidance("Select the geometry of the PhotonDet detector");
  fSetPhotonDetGeometryCmd->SetGuidance("Only Accepts 'Circle' and 'Square'");
  fSetPhotonDetGeometryCmd->SetCandidates("Circle Square");
  fSetPhotonDetGeometryCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetPhotonDetGeometryCmd->SetToBeBroadcasted(false);

  // Lệnh chọn số lớp bọc cho sợi quang
  fSetNumOfCladLayersCmd = new G4UIcmdWithAnInteger("/WLS/setNumOfLayers", this);
  fSetNumOfCladLayersCmd->SetGuidance("Select the number of cladding layers");
  fSetNumOfCladLayersCmd->SetGuidance("Maximum number is 2");
  fSetNumOfCladLayersCmd->SetParameterName("numberOfLayers", false);
  fSetNumOfCladLayersCmd->SetRange("numberOfLayers>=0 && numberOfLayers<=2");
  fSetNumOfCladLayersCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetNumOfCladLayersCmd->SetToBeBroadcasted(false);

  // Lệnh đặt độ nhám bề mặt giữa Clad1 và WLS Fiber
  fSetSurfaceRoughnessCmd = new G4UIcmdWithADouble("/WLS/setSurfaceRoughness", this);
  fSetSurfaceRoughnessCmd->SetGuidance("Set the roughness between Clad1 and WLS Fiber");
  fSetSurfaceRoughnessCmd->SetParameterName("roughness", false);
  fSetSurfaceRoughnessCmd->SetRange("roughness>0 && roughness<=1");
  fSetSurfaceRoughnessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetSurfaceRoughnessCmd->SetToBeBroadcasted(false);

  // Lệnh đặt tỉ lệ giữa trục x và y của sợi quang
  fSetXYRatioCmd = new G4UIcmdWithADouble("/WLS/setXYRatio", this);
  fSetXYRatioCmd->SetGuidance("Set the ratio between x and y axis (x/y)");
  fSetXYRatioCmd->SetParameterName("ratio", false);
  fSetXYRatioCmd->SetRange("ratio>0 && ratio<=1");
  fSetXYRatioCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetXYRatioCmd->SetToBeBroadcasted(false);

  // Lệnh đặt độ bóng của gương
  fSetMirrorPolishCmd = new G4UIcmdWithADouble("/WLS/setMirrorPolish", this);
  fSetMirrorPolishCmd->SetGuidance("Set the polish of the mirror");
  fSetMirrorPolishCmd->SetParameterName("polish", false);
  fSetMirrorPolishCmd->SetRange("polish>0 && polish<=1");
  fSetMirrorPolishCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetMirrorPolishCmd->SetToBeBroadcasted(false);

  // Lệnh đặt độ phản xạ của gương
  fSetMirrorReflectivityCmd = new G4UIcmdWithADouble("/WLS/setMirrorReflectivity", this);
  fSetMirrorReflectivityCmd->SetGuidance("Set the reflectivity of the mirror");
  fSetMirrorReflectivityCmd->SetParameterName("reflectivity", false);
  fSetMirrorReflectivityCmd->SetRange("reflectivity>=0 && reflectivity<=1");
  fSetMirrorReflectivityCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetMirrorReflectivityCmd->SetToBeBroadcasted(false);

  // Lệnh đặt độ bóng của PhotonDet
  fSetPhotonDetPolishCmd = new G4UIcmdWithADouble("/WLS/setPhotonDetPolish", this);
  fSetPhotonDetPolishCmd->SetGuidance("Set the polish of the mirror");
  fSetPhotonDetPolishCmd->SetParameterName("polish", false);
  fSetPhotonDetPolishCmd->SetRange("polish>0 && polish<=1");
  fSetPhotonDetPolishCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetPhotonDetPolishCmd->SetToBeBroadcasted(false);

  // Lệnh đặt độ phản xạ của PhotonDet
  fSetPhotonDetReflectivityCmd = new G4UIcmdWithADouble("/WLS/setPhotonDetReflectivity", this);
  fSetPhotonDetReflectivityCmd->SetGuidance("Set the reflectivity of the mirror");
  fSetPhotonDetReflectivityCmd->SetParameterName("reflectivity", false);
  fSetPhotonDetReflectivityCmd->SetRange("reflectivity>=0 && reflectivity<=1");
  fSetPhotonDetReflectivityCmd->AvailableForStates(G4State_PreInit);
  fSetPhotonDetReflectivityCmd->SetToBeBroadcasted(false);

  // Lệnh đặt chiều dài nửa sợi quang WLS
  fSetWLSLengthCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setWLSLength", this);
  fSetWLSLengthCmd->SetGuidance("Set the half length of the WLS fiber");
  fSetWLSLengthCmd->SetParameterName("length", false);
  fSetWLSLengthCmd->SetRange("length>0.");
  fSetWLSLengthCmd->SetUnitCategory("Length");
  fSetWLSLengthCmd->SetDefaultUnit("mm");
  fSetWLSLengthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetWLSLengthCmd->SetToBeBroadcasted(false);

  // Lệnh đặt bán kính sợi quang WLS
  fSetWLSRadiusCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setWLSRadius", this);
  fSetWLSRadiusCmd->SetGuidance("Set the radius of the WLS fiber");
  fSetWLSRadiusCmd->SetParameterName("radius", false);
  fSetWLSRadiusCmd->SetRange("radius>0.");
  fSetWLSRadiusCmd->SetUnitCategory("Length");
  fSetWLSRadiusCmd->SetDefaultUnit("mm");
  fSetWLSRadiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetWLSRadiusCmd->SetToBeBroadcasted(false);

  // Lệnh đặt bán kính lớp bọc 1
  fSetClad1RadiusCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setClad1Radius", this);
  fSetClad1RadiusCmd->SetGuidance("Set the radius of Cladding 1");
  fSetClad1RadiusCmd->SetParameterName("radius", false);
  fSetClad1RadiusCmd->SetRange("radius>0.");
  fSetClad1RadiusCmd->SetUnitCategory("Length");
  fSetClad1RadiusCmd->SetDefaultUnit("mm");
  fSetClad1RadiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetClad1RadiusCmd->SetToBeBroadcasted(false);

  // Lệnh đặt bán kính lớp bọc 2
  fSetClad2RadiusCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setClad2Radius", this);
  fSetClad2RadiusCmd->SetGuidance("Set the radius of Cladding 2");
  fSetClad2RadiusCmd->SetParameterName("radius", false);
  fSetClad2RadiusCmd->SetRange("radius>0.");
  fSetClad2RadiusCmd->SetUnitCategory("Length");
  fSetClad2RadiusCmd->SetDefaultUnit("mm");
  fSetClad2RadiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetClad2RadiusCmd->SetToBeBroadcasted(false);

  // Lệnh đặt chiều dài nửa của PhotonDet
  fSetPhotonDetHalfLengthCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setPhotonDetHalfLength", this);
  fSetPhotonDetHalfLengthCmd->SetGuidance("Set the half length of PhotonDet detector");
  fSetPhotonDetHalfLengthCmd->SetParameterName("halfL", false);
  fSetPhotonDetHalfLengthCmd->SetRange("halfL>0.");
  fSetPhotonDetHalfLengthCmd->SetUnitCategory("Length");
  fSetPhotonDetHalfLengthCmd->SetDefaultUnit("mm");
  fSetPhotonDetHalfLengthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetPhotonDetHalfLengthCmd->SetToBeBroadcasted(false);

  // Lệnh đặt khoảng cách giữa PhotonDet và đầu sợi quang
  fSetGapCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setGap", this);
  fSetGapCmd->SetGuidance("Set the distance between PhotonDet and fiber end");
  fSetGapCmd->SetParameterName("theta", false);
  fSetGapCmd->SetUnitCategory("Length");
  fSetGapCmd->SetDefaultUnit("mm");
  fSetGapCmd->SetRange("theta>=0.");
  fSetGapCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetGapCmd->SetToBeBroadcasted(false);

  // Lệnh đặt góc lệch của PhotonDet so với trục z
  fSetPhotonDetAlignmentCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setAlignment", this);
  fSetPhotonDetAlignmentCmd->SetGuidance("Set the deviation of PhotonDet from z axis");
  fSetPhotonDetAlignmentCmd->SetParameterName("theta", false);
  fSetPhotonDetAlignmentCmd->SetUnitCategory("Angle");
  fSetPhotonDetAlignmentCmd->SetDefaultUnit("deg");
  fSetPhotonDetAlignmentCmd->SetRange("theta>-90. && theta<90.");
  fSetPhotonDetAlignmentCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetPhotonDetAlignmentCmd->SetToBeBroadcasted(false);

  // Lệnh bật/tắt gương ở đầu sợi quang
  fSetMirrorCmd = new G4UIcmdWithABool("/WLS/setMirror", this);
  fSetMirrorCmd->SetGuidance("Place a mirror at the end of the fiber");
  fSetMirrorCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetMirrorCmd->SetToBeBroadcasted(false);

  // Lệnh đặt chiều dài thanh nhựa nhấp nháy
  fSetBarLengthCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setBarLength", this);
  fSetBarLengthCmd->SetGuidance("Set the length of the scintillator bar");
  fSetBarLengthCmd->SetParameterName("length", false);
  fSetBarLengthCmd->SetRange("length>0.");
  fSetBarLengthCmd->SetUnitCategory("Length");
  fSetBarLengthCmd->SetDefaultUnit("mm");
  fSetBarLengthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetBarLengthCmd->SetToBeBroadcasted(false);

  // Lệnh đặt cạnh đáy của thanh nhựa nhấp nháy
  fSetBarBaseCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setBarBase", this);
  fSetBarBaseCmd->SetGuidance("Set the side length of the scintillator bar");
  fSetBarBaseCmd->SetParameterName("length", false);
  fSetBarBaseCmd->SetRange("length>0.");
  fSetBarBaseCmd->SetUnitCategory("Length");
  fSetBarBaseCmd->SetDefaultUnit("mm");
  fSetBarBaseCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetBarBaseCmd->SetToBeBroadcasted(false);

  // Lệnh đặt bán kính lỗ cho sợi quang
  fSetHoleRadiusCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setHoleRadius", this);
  fSetHoleRadiusCmd->SetGuidance("Set the radius of the fiber hole");
  fSetHoleRadiusCmd->SetParameterName("radius", false);
  fSetHoleRadiusCmd->SetRange("radius>0.");
  fSetHoleRadiusCmd->SetUnitCategory("Length");
  fSetHoleRadiusCmd->SetDefaultUnit("mm");
  fSetHoleRadiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetHoleRadiusCmd->SetToBeBroadcasted(false);

  // Lệnh đặt độ dày lớp phủ ngoài thanh nhựa
  fSetCoatingThicknessCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setCoatingThickness", this);
  fSetCoatingThicknessCmd->SetGuidance("Set thickness of the coating on the bars");
  fSetCoatingThicknessCmd->SetParameterName("thick", false);
  fSetCoatingThicknessCmd->SetUnitCategory("Length");
  fSetCoatingThicknessCmd->SetDefaultUnit("mm");
  fSetCoatingThicknessCmd->SetRange("thick>=0.");
  fSetCoatingThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetCoatingThicknessCmd->SetToBeBroadcasted(false);

  // Lệnh đặt bán kính trong của lớp phủ bo góc
  fSetCoatingRadiusCmd = new G4UIcmdWithADoubleAndUnit("/WLS/setCoatingRadius", this);
  fSetCoatingRadiusCmd->SetGuidance("Set inner radius of the corner bar coating");
  fSetCoatingRadiusCmd->SetParameterName("cradius", false);
  fSetCoatingRadiusCmd->SetUnitCategory("Length");
  fSetCoatingRadiusCmd->SetDefaultUnit("mm");
  fSetCoatingRadiusCmd->SetRange("cradius>=0.");
  fSetCoatingRadiusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetCoatingRadiusCmd->SetToBeBroadcasted(false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm hủy: giải phóng bộ nhớ cho tất cả các lệnh UI đã tạo
WLSDetectorMessenger::~WLSDetectorMessenger()
{
  delete fDetDir;

  delete fSetBarLengthCmd;
  delete fSetBarBaseCmd;
  delete fSetClad1RadiusCmd;
  delete fSetClad2RadiusCmd;
  delete fSetCoatingThicknessCmd;
  delete fSetCoatingRadiusCmd;
  delete fSetGapCmd;
  delete fSetHoleRadiusCmd;
  delete fSetMirrorCmd;
  delete fSetMirrorPolishCmd;
  delete fSetMirrorReflectivityCmd;
  delete fSetNumOfCladLayersCmd;
  delete fSetPhotonDetAlignmentCmd;
  delete fSetPhotonDetGeometryCmd;
  delete fSetPhotonDetHalfLengthCmd;
  delete fSetPhotonDetPolishCmd;
  delete fSetPhotonDetReflectivityCmd;
  delete fSetSurfaceRoughnessCmd;
  delete fSetWLSLengthCmd;
  delete fSetWLSRadiusCmd;
  delete fSetXYRatioCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm xử lý khi người dùng nhập lệnh UI mới
void WLSDetectorMessenger::SetNewValue(G4UIcommand* command, G4String val)
{
  // Kiểm tra lệnh nào được gọi và chuyển giá trị tới đối tượng detector tương ứng
  if (command == fSetPhotonDetGeometryCmd) {
    fDetector->SetPhotonDetGeometry(val);
  }
  else if (command == fSetNumOfCladLayersCmd) {
    fDetector->SetNumberOfCladding(G4UIcmdWithAnInteger::GetNewIntValue(val));
  }
  else if (command == fSetSurfaceRoughnessCmd) {
    fDetector->SetSurfaceRoughness(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if (command == fSetXYRatioCmd) {
    fDetector->SetXYRatio(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if (command == fSetMirrorPolishCmd) {
    fDetector->SetMirrorPolish(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if (command == fSetMirrorReflectivityCmd) {
    fDetector->SetMirrorReflectivity(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if (command == fSetPhotonDetPolishCmd) {
    fDetector->SetPhotonDetPolish(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if (command == fSetPhotonDetReflectivityCmd) {
    fDetector->SetPhotonDetReflectivity(G4UIcmdWithADouble::GetNewDoubleValue(val));
  }
  else if (command == fSetWLSLengthCmd) {
    fDetector->SetWLSLength(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetWLSRadiusCmd) {
    fDetector->SetWLSRadius(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetClad1RadiusCmd) {
    fDetector->SetClad1Radius(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetClad2RadiusCmd) {
    fDetector->SetClad2Radius(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetPhotonDetHalfLengthCmd) {
    fDetector->SetPhotonDetHalfLength(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetGapCmd) {
    fDetector->SetGap(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetPhotonDetAlignmentCmd) {
    fDetector->SetPhotonDetAlignment(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetMirrorCmd) {
    fDetector->SetMirror(G4UIcmdWithABool::GetNewBoolValue(val));
  }
  else if (command == fSetBarLengthCmd) {
    fDetector->SetBarLength(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetBarBaseCmd) {
    fDetector->SetBarBase(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetHoleRadiusCmd) {
    fDetector->SetHoleRadius(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetCoatingThicknessCmd) {
    fDetector->SetCoatingThickness(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
  else if (command == fSetCoatingRadiusCmd) {
    fDetector->SetCoatingRadius(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  }
}