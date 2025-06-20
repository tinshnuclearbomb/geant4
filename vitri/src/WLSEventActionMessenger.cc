/// \file optical/wls/src/WLSEventActionMessenger.cc
/// \brief Implementation of the WLSEventActionMessenger class
//
//

#include "WLSEventActionMessenger.hh"
#include "WLSEventAction.hh"
#include "G4UIcmdWithAnInteger.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo: Thiết lập lệnh UI cho phép điều chỉnh mức độ chi tiết khi in thông tin event
WLSEventActionMessenger::WLSEventActionMessenger(WLSEventAction* eventaction)
  : fEventAction(eventaction)
{
  // Tạo lệnh UI để đặt mức độ verbose cho EventAction
  fSetVerboseCmd = new G4UIcmdWithAnInteger("/WLS/eventVerbose", this);
  fSetVerboseCmd->SetGuidance("Thiết lập mức độ chi tiết khi in thông tin event (verbose level).");
  fSetVerboseCmd->SetParameterName("level", true); // Tham số tên là "level", có thể bỏ qua
  fSetVerboseCmd->SetDefaultValue(0);              // Giá trị mặc định là 0 (ít chi tiết)
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm hủy: Giải phóng bộ nhớ cho lệnh UI đã tạo
WLSEventActionMessenger::~WLSEventActionMessenger()
{
  delete fSetVerboseCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm xử lý khi người dùng nhập lệnh UI mới
void WLSEventActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  // Nếu lệnh là đặt mức verbose, chuyển giá trị tới EventAction
  if (command == fSetVerboseCmd)
    fEventAction->SetEventVerbose(fSetVerboseCmd->GetNewIntValue(newValue));
}