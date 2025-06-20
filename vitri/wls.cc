/// \file optical/wls/wls.cc
/// \brief Main program of the optical/wls example
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "FTFP_BERT.hh"
#include "WLSActionInitialization.hh"
#include "WLSDetectorConstruction.hh"

#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "G4RunManagerFactory.hh"
#include "G4Types.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char** argv)
{
  //Truyền tham số dòng lệnh, khởi tạo chế độ giao diện người dùng (UI)
  G4UIExecutive* ui = nullptr;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Tạo đối tượng quản lý chạy mô phỏng (RunManager)
  auto runManager = G4RunManagerFactory::CreateRunManager();

  // Thiết lập seed cho bộ sinh số ngẫu nhiên
  G4int seed = 123; // Giá trị mặc định
  if (argc > 2) seed = atoi(argv[argc - 1]); // Nếu truyền seed qua dòng lệnh, sử dụng giá trị đó

  // Chọn bộ sinh số ngẫu nhiên và đặt seed
  // G4Random::setTheEngine(new CLHEP::RanecuEngine); // Có thể chọn engine khác nếu muốn
  G4Random::setTheSeed(seed);

  // Khởi tạo hình học detector
  auto detector = new WLSDetectorConstruction();
  runManager->SetUserInitialization(detector);

  // Khởi tạo danh sách vật lý (physics list)
  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  // Thay thế mô-đun vật lý điện từ mặc định bằng option4 (chính xác hơn cho photon)
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  // Thêm mô-đun vật lý quang học (Optical Physics)
  auto opticalPhysics = new G4OpticalPhysics();

  // Lấy đối tượng tham số quang học toàn cục và bật tính năng gọi Sensitive Detector tại biên
  auto opticalParams = G4OpticalParameters::Instance();
  opticalParams->SetBoundaryInvokeSD(true);

  // Đăng ký mô-đun vật lý quang học vào danh sách vật lý
  physicsList->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physicsList);

  // Khởi tạo các hành động của người dùng (user action: phát hạt, ghi nhận, ...)
  runManager->SetUserInitialization(new WLSActionInitialization(detector));

  // Khởi tạo hệ thống hiển thị (Visualization)
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  // Lấy con trỏ tới trình quản lý giao diện người dùng
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (ui) {
    // Nếu chạy ở chế độ tương tác (interactive mode)
    // Nếu có giao diện đồ họa (GUI), thực thi macro giao diện
    if (ui->IsGUI()) UImanager->ApplyCommand("/control/execute gui.mac");
    // Bắt đầu phiên làm việc giao diện người dùng
    ui->SessionStart();
    delete ui; // Giải phóng bộ nhớ cho UI sau khi kết thúc phiên
  }
  else {
    // Nếu chạy ở chế độ batch (không tương tác), thực thi macro được truyền qua dòng lệnh
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }

  // Kết thúc chương trình, giải phóng bộ nhớ cho các đối tượng đã khởi tạo
  delete visManager;
  delete runManager;
  return 0;
}
