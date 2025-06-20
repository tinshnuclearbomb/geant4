#include "WLSDetectorConstruction.hh"
#include "WLSEventAction.hh"
#include "WLSPrimaryGeneratorAction.hh"
#include "WLSRunAction.hh"
#include "WLSStackingAction.hh"
#include "WLSSteppingAction.hh"
#include "WLSTrackingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo của lớp WLSActionInitialization
// Nhận vào con trỏ tới đối tượng cấu trúc máy dò (detector)
WLSActionInitialization::WLSActionInitialization(WLSDetectorConstruction* det)
  : G4VUserActionInitialization(), fDetector(det)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm thiết lập action cho master thread (chỉ dùng RunAction)
void WLSActionInitialization::BuildForMaster() const
{
  SetUserAction(new WLSRunAction()); // Thiết lập hành động cho quá trình chạy
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm thiết lập các action cho worker thread
void WLSActionInitialization::Build() const
{
  // Thiết lập hành động sinh hạt sơ cấp, truyền vào thông tin máy dò
  SetUserAction(new WLSPrimaryGeneratorAction(fDetector));

  // Thiết lập hành động cho quá trình chạy
  SetUserAction(new WLSRunAction());

  // Tạo và thiết lập hành động cho từng sự kiện
  auto eventAction = new WLSEventAction();
  SetUserAction(eventAction);

  // Thiết lập hành động theo dõi hạt
  SetUserAction(new WLSTrackingAction());

  // Thiết lập hành động cho từng bước di chuyển của hạt, truyền vào thông tin máy dò và sự kiện
  SetUserAction(new WLSSteppingAction(fDetector, eventAction));

  // Thiết lập hành động quản lý ngăn xếp các hạt
  SetUserAction(new WLSStackingAction());
}