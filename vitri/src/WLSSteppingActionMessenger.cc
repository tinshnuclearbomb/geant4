/// \file optical/wls/src/WLSSteppingActionMessenger.cc
/// \brief Implementation of the WLSSteppingActionMessenger class
//
//
#include "WLSSteppingActionMessenger.hh"

#include "WLSSteppingAction.hh"

#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo: Thiết lập các lệnh UI cho phép điều khiển hành động stepping
WLSSteppingActionMessenger::WLSSteppingActionMessenger(WLSSteppingAction* steppingaction)
  : fSteppingAction(steppingaction)
{
  // Tạo thư mục lệnh UI cho các lệnh liên quan đến stepping
  fSteppingDir = new G4UIdirectory("/stepping/");
  fSteppingDir->SetGuidance("stepping control");

  // Lệnh đặt giới hạn số lần phản xạ tối đa cho photon trong sợi quang
  fSetBounceLimitCmd = new G4UIcmdWithAnInteger("/stepping/setBounceLimit", this);
  fSetBounceLimitCmd->SetGuidance("Chọn số lần phản xạ tối đa cho phép của photon trong sợi quang.");
  fSetBounceLimitCmd->SetGuidance("Đặt giá trị này bằng 0 nếu không muốn giới hạn.");
  fSetBounceLimitCmd->SetParameterName("limit", false);
  fSetBounceLimitCmd->SetRange("limit>=0");
  fSetBounceLimitCmd->AvailableForStates(G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm hủy: giải phóng bộ nhớ cho các lệnh UI đã tạo
WLSSteppingActionMessenger::~WLSSteppingActionMessenger()
{
  delete fSteppingDir;
  delete fSetBounceLimitCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm xử lý khi người dùng nhập lệnh UI mới
void WLSSteppingActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  // Nếu lệnh là đặt giới hạn số lần phản xạ, chuyển giá trị tới SteppingAction
  if (command == fSetBounceLimitCmd) {
    fSteppingAction->SetBounceLimit(G4UIcmdWithAnInteger::GetNewIntValue(newValue));
  }
}