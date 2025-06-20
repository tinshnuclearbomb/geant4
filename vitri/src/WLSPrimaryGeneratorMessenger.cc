/// \file optical/wls/src/WLSPrimaryGeneratorMessenger.cc
/// \brief Implementation of the WLSPrimaryGeneratorMessenger class
//
//

#include "WLSPrimaryGeneratorMessenger.hh"
#include "WLSPrimaryGeneratorAction.hh"

#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIdirectory.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo: Thiết lập các lệnh UI cho phép điều khiển nguồn phát hạt sơ cấp
WLSPrimaryGeneratorMessenger::WLSPrimaryGeneratorMessenger(WLSPrimaryGeneratorAction* gun)
  : fAction(gun)
{
  // Tạo thư mục lệnh UI cho các lệnh liên quan đến nguồn phát hạt
  fGunDir = new G4UIdirectory("/WLS/gun/");
  fGunDir->SetGuidance("WLSPrimaryGenerator control");

  // Lệnh đặt phân cực tuyến tính cho photon quang học
  fSetPolarizationCmd = new G4UIcmdWithADoubleAndUnit("/WLS/gun/optPhotonPolar", this);
  fSetPolarizationCmd->SetGuidance("Set linear polarization");
  fSetPolarizationCmd->SetGuidance("  angle w.r.t. (k,n) plane");
  fSetPolarizationCmd->SetParameterName("angle", true); // Tham số "angle", có thể bỏ qua
  fSetPolarizationCmd->SetUnitCategory("Angle");
  fSetPolarizationCmd->SetDefaultValue(0.);
  fSetPolarizationCmd->AvailableForStates(G4State_Idle);

  // Lệnh đặt hằng số thời gian phát trễ (decay time constant) cho photon WLS
  fSetDecayTimeConstantCmd = new G4UIcmdWithADoubleAndUnit("/WLS/gun/setDecayTimeConstant", this);
  fSetDecayTimeConstantCmd->SetGuidance("Set the decay time constant");
  fSetDecayTimeConstantCmd->SetGuidance("for the starting time of each photon");
  fSetDecayTimeConstantCmd->SetParameterName("time_const", false);
  fSetDecayTimeConstantCmd->SetUnitCategory("Time");
  fSetDecayTimeConstantCmd->SetRange("time_const>=0");
  fSetDecayTimeConstantCmd->AvailableForStates(G4State_Idle);

  // Lệnh bật/tắt lấy mẫu năng lượng photon từ phổ phát xạ (hiện tại đang tắt)
  // fSetUseSampledEnergyCmd = new G4UIcmdWithABool("/WLS/gun/useSampledEnergy", this);
  // fSetUseSampledEnergyCmd->SetGuidance("Enable sampling of primary energy.");
  // fSetUseSampledEnergyCmd->SetGuidance("This is currently disabled.");
  // fSetUseSampledEnergyCmd->AvailableForStates();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm hủy: giải phóng bộ nhớ cho các lệnh UI đã tạo
WLSPrimaryGeneratorMessenger::~WLSPrimaryGeneratorMessenger()
{
  delete fGunDir;
  delete fSetPolarizationCmd;
  delete fSetDecayTimeConstantCmd;
  // delete fSetUseSampledEnergyCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm xử lý khi người dùng nhập lệnh UI mới
void WLSPrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String val)
{
  // Nếu lệnh là đặt phân cực photon quang học
  if (command == fSetPolarizationCmd)
    fAction->SetOptPhotonPolar(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  // Nếu lệnh là đặt hằng số thời gian phát trễ
  else if (command == fSetDecayTimeConstantCmd)
    fAction->SetDecayTimeConstant(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val));
  // Nếu lệnh là bật/tắt lấy mẫu năng lượng photon (hiện tại đang tắt)
  // else if ( command == fSetUseSampledEnergyCmd )
  //   fAction->SetUseSampledEnergy(G4UIcmdWithABool::GetNewBoolValue(val));
}