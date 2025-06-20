/// \file optical/wls/src/WLSSteppingAction.cc
/// \brief Implementation of the WLSSteppingAction class
//
//

#include "WLSSteppingAction.hh"

#include "WLSDetectorConstruction.hh"
#include "WLSEventAction.hh"
#include "WLSPhotonDetSD.hh"
#include "WLSSteppingActionMessenger.hh"
#include "WLSUserTrackInformation.hh"

#include "G4OpBoundaryProcess.hh"
#include "G4OpticalPhoton.hh"
#include "G4ProcessManager.hh"
#include "G4Run.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4UImanager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ios.hh"

// Purpose: Save relevant information into User Track Information
// and count/record photon events at each step for statistics

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm khởi tạo: nhận con trỏ detector và event, khởi tạo messenger và reset bộ đếm
WLSSteppingAction::WLSSteppingAction(WLSDetectorConstruction* detector, WLSEventAction* event)
  : fDetector(detector), fEventAction(event)
{
  fSteppingMessenger = new WLSSteppingActionMessenger(this);
  ResetCounters();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm hủy: giải phóng bộ nhớ messenger
WLSSteppingAction::~WLSSteppingAction()
{
  delete fSteppingMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Đặt giới hạn số lần phản xạ cho photon trong sợi quang
void WLSSteppingAction::SetBounceLimit(G4int i)
{
  fBounceLimit = i;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Trả về số lần phản xạ tổng cộng trong event hiện tại
G4int WLSSteppingAction::GetNumberOfBounces()
{
  return fCounterBounce;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Trả về số lần phản xạ trên lớp bọc 1
G4int WLSSteppingAction::GetNumberOfClad1Bounces()
{
  return fCounterClad1Bounce;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Trả về số lần phản xạ trên lớp bọc 2
G4int WLSSteppingAction::GetNumberOfClad2Bounces()
{
  return fCounterClad2Bounce;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Trả về số lần phản xạ trên sợi WLS
G4int WLSSteppingAction::GetNumberOfWLSBounces()
{
  return fCounterWLSBounce;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Reset bộ đếm số photon thành công (thoát ở đầu sợi)
G4int WLSSteppingAction::ResetSuccessCounter()
{
  G4int temp = fCounterEnd;
  fCounterEnd = 0;
  return temp;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm chính: xử lý từng bước của mỗi track photon
void WLSSteppingAction::UserSteppingAction(const G4Step* theStep)
{
  G4Track* theTrack = theStep->GetTrack();
  auto trackInformation = (WLSUserTrackInformation*)theTrack->GetUserInformation();

  G4StepPoint* thePrePoint = theStep->GetPreStepPoint();
  G4StepPoint* thePostPoint = theStep->GetPostStepPoint();

  G4VPhysicalVolume* thePrePV = thePrePoint->GetPhysicalVolume();
  G4VPhysicalVolume* thePostPV = thePostPoint->GetPhysicalVolume();

  G4String thePrePVname = " ";
  G4String thePostPVname = " ";

  if (thePostPV) {
    thePrePVname = thePrePV->GetName();
    thePostPVname = thePostPV->GetName();
  }

  // Ghi nhận thông tin ban đầu của photon sơ cấp (nếu cần)
  if (theTrack->GetParentID() == 0) {
    if (theTrack->GetCurrentStepNumber() == 1) {
      // Có thể lưu thông tin vị trí, hướng, động lượng ban đầu nếu cần
    }
  }

  // Lấy trạng thái biên của photon (phản xạ, truyền qua, hấp thụ...)
  G4OpBoundaryProcessStatus theStatus = Undefined;

  static G4ThreadLocal G4ProcessManager* OpManager =
    G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

  if (OpManager) {
    G4int nproc = OpManager->GetPostStepProcessVector()->entries();
    G4ProcessVector* fPostStepDoItVector = OpManager->GetPostStepProcessVector(typeDoIt);

    for (G4int i = 0; i < nproc; ++i) {
      G4VProcess* fCurrentProcess = (*fPostStepDoItVector)[i];
      fOpProcess = dynamic_cast<G4OpBoundaryProcess*>(fCurrentProcess);
      if (fOpProcess) {
        theStatus = fOpProcess->GetStatus();
        break;
      }
    }
  }

  // Tính skewness của photon tại lần đầu gặp biên (nếu cần)
  if (fInitGamma == -1
      && (theStatus == TotalInternalReflection || theStatus == FresnelReflection
          || theStatus == FresnelRefraction)
      && trackInformation->IsStatus(InsideOfFiber))
  {
    G4double px = theTrack->GetVertexMomentumDirection().x();
    G4double py = theTrack->GetVertexMomentumDirection().y();
    G4double x = theTrack->GetPosition().x();
    G4double y = theTrack->GetPosition().y();

    fInitGamma = x * px + y * py;
    fInitGamma = fInitGamma / std::sqrt(px * px + py * py) / std::sqrt(x * x + y * y);
    fInitGamma = std::acos(fInitGamma * rad);

    if (fInitGamma / deg > 90.0) {
      fInitGamma = 180 * deg - fInitGamma;
    }
  }

  // Nếu photon thoát khỏi đầu đọc (readout) mà không gặp detector
  if (!thePostPV && trackInformation->IsStatus(EscapedFromReadOut)) {
    fEventAction->AddEscaped();
    ResetCounters();
    return;
  }

  // Xử lý các trạng thái biên của photon
  switch (theStatus) {
    // Photon truyền qua biên (ra khỏi sợi quang)
    case FresnelRefraction:
    case SameMaterial:
      fEventAction->AddExiting();

      if (thePostPVname == "WLSFiber" || thePostPVname == "Clad1" || thePostPVname == "Clad2") {
        if (trackInformation->IsStatus(OutsideOfFiber))
          trackInformation->AddStatusFlag(InsideOfFiber);
        // Photon vẫn còn trong sợi/quanh sợi
      }
      else if (trackInformation->IsStatus(InsideOfFiber)) {
        // Nếu vị trí z trùng với đầu sợi, photon thoát ở đầu
        if (theTrack->GetPosition().z() == fDetector->GetWLSFiberEnd()) {
          trackInformation->AddStatusFlag(EscapedFromReadOut);
          fCounterEnd++;
          fEventAction->AddEscapedEnd();
        }
        else  // Photon thoát ở cạnh bên sợi
        {
          trackInformation->AddStatusFlag(EscapedFromSide);
          trackInformation->SetExitPosition(theTrack->GetPosition());
          fCounterMid++;
          fEventAction->AddEscapedMid();
          ResetCounters();
        }
        trackInformation->AddStatusFlag(OutsideOfFiber);
        trackInformation->SetExitPosition(theTrack->GetPosition());
      }
      return;

    // Phản xạ toàn phần bên trong sợi quang
    case TotalInternalReflection:
      fEventAction->AddTIR();
      // Nếu vượt quá số lần phản xạ cho phép thì dừng track
      if (fBounceLimit > 0 && fCounterBounce >= fBounceLimit) {
        theTrack->SetTrackStatus(fStopAndKill);
        trackInformation->AddStatusFlag(murderee);
        ResetCounters();
        G4cout << "\n Bounce Limit Exceeded" << G4endl;
        return;
      }
      break;

    // Phản xạ Fresnel (thường gặp ở biên giữa các lớp)
    case FresnelReflection:
      fCounterBounce++;
      fEventAction->AddBounce();

      if (thePrePVname == "WLSFiber") {
        fCounterWLSBounce++;
        fEventAction->AddWLSBounce();
      }
      else if (thePrePVname == "Clad1") {
        fCounterClad1Bounce++;
        fEventAction->AddClad1Bounce();
      }
      else if (thePrePVname == "Clad2") {
        fCounterClad2Bounce++;
        fEventAction->AddClad1Bounce();
      }

      // Nếu phản xạ tại đầu đọc (readout)
      if (theTrack->GetPosition().z() == fDetector->GetWLSFiberEnd()) {
        if (!trackInformation->IsStatus(ReflectedAtReadOut)
            && trackInformation->IsStatus(InsideOfFiber))
        {
          trackInformation->AddStatusFlag(ReflectedAtReadOut);

          if (fDetector->IsPerfectFiber() && theStatus == TotalInternalReflection) {
            theTrack->SetTrackStatus(fStopAndKill);
            trackInformation->AddStatusFlag(murderee);
            ResetCounters();
            return;
          }
        }
      }
      return;

    // Phản xạ trên gương (mirror)
    case LambertianReflection:
    case LobeReflection:
    case SpikeReflection:
      fEventAction->AddReflected();
      // Nếu phản xạ trên gương thật sự
      if (thePostPVname == "Mirror") {
        trackInformation->AddStatusFlag(ReflectedAtMirror);
        fEventAction->AddMirror();
      }
      return;

    // Photon bị phát hiện bởi detector
    case Detection:
      // Khi photon chạm detector, dừng track và reset bộ đếm
      ResetCounters();
      theTrack->SetTrackStatus(fStopAndKill);
      return;

    default:
      break;
  }

  // Nếu photon bị hấp thụ trong sợi quang
  if (theTrack->GetTrackStatus() != fAlive && trackInformation->IsStatus(InsideOfFiber)) {
    ResetCounters();
    return;
  }
}