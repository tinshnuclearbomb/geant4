/// \file optical/wls/src/WLSPhotonDetHit.cc
/// \brief Implementation of the WLSPhotonDetHit class
//
//
#include "WLSPhotonDetHit.hh"

#include "G4UnitsTable.hh"

// Bộ cấp phát bộ nhớ động cho các đối tượng hit (theo từng thread)
G4ThreadLocal G4Allocator<WLSPhotonDetHit>* WLSPhotonDetHitAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo mặc định: khởi tạo các biến vị trí về (0,0,0)
WLSPhotonDetHit::WLSPhotonDetHit()
{
  fPosArrive = G4ThreeVector(0., 0., 0.); // Vị trí photon đến detector
  fPosExit = G4ThreeVector(0., 0., 0.);   // Vị trí photon rời khỏi sợi quang
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo với tham số: gán các giá trị vị trí, thời gian, năng lượng
WLSPhotonDetHit::WLSPhotonDetHit(G4ThreeVector pExit, G4ThreeVector pArrive, G4double pTime,
                                 G4double pEnergy)
{
  fPosExit = pExit;         // Vị trí photon rời khỏi sợi quang
  fPosArrive = pArrive;     // Vị trí photon đến detector
  fArrivalTime = pTime;     // Thời gian photon đến detector
  fEnergy = pEnergy;        // Năng lượng photon tại detector
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm sao chép (copy constructor)
WLSPhotonDetHit::WLSPhotonDetHit(const WLSPhotonDetHit& right) : G4VHit()
{
  *this = right; // Sử dụng toán tử gán để sao chép toàn bộ dữ liệu
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Toán tử gán: sao chép các thuộc tính từ đối tượng khác
const WLSPhotonDetHit& WLSPhotonDetHit::operator=(const WLSPhotonDetHit& right)
{
  fPosExit = right.fPosExit;
  fPosArrive = right.fPosArrive;
  fArrivalTime = right.fArrivalTime;
  fEnergy = right.fEnergy;

  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Toán tử so sánh: kiểm tra hai hit có giống nhau không (so sánh từng trường)
G4bool WLSPhotonDetHit::operator==(const WLSPhotonDetHit& right) const
{
  return fPosExit == right.fPosExit && fPosArrive == right.fPosArrive
         && fArrivalTime == right.fArrivalTime && fEnergy == right.fEnergy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm in thông tin hit ra màn hình (dùng cho debug hoặc verbose)
void WLSPhotonDetHit::Print()
{
  G4cout << "Arrival time: " << std::setw(7) << G4BestUnit(fArrivalTime, "Time")
         << " Arrival position: (" << std::setw(7) << G4BestUnit(fPosArrive.x(), "Length") << ", "
         << std::setw(7) << G4BestUnit(fPosArrive.y(), "Length") << "); "
         << "Exit position: (" << std::setw(7) << G4BestUnit(fPosExit.x(), "Length") << ", "
         << std::setw(7) << G4BestUnit(fPosExit.y(), "Length") << "); "
         << "Energy: " << std::setw(7) << G4BestUnit(fEnergy, "Energy") << G4endl;
}