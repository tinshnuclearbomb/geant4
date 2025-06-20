/// \file optical/wls/src/WLSRun.cc
/// \brief Implementation of the WLSRun class
//
//

#include "WLSRun.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm Merge: cộng dồn các biến thống kê từ một đối tượng WLSRun khác (dùng cho chạy song song)
void WLSRun::Merge(const G4Run* run)
{
  // Ép kiểu về WLSRun để truy cập các biến thống kê riêng
  const auto localRun = static_cast<const WLSRun*>(run);

  // Cộng dồn các biến thống kê (tổng và tổng bình phương) cho từng loại sự kiện
  fNTIR         += localRun->fNTIR;
  fNTIR2        += localRun->fNTIR2;
  fNExiting     += localRun->fNExiting;
  fNExiting2    += localRun->fNExiting2;
  fEscapedEnd   += localRun->fEscapedEnd;
  fEscapedEnd2  += localRun->fEscapedEnd2;
  fEscapedMid   += localRun->fEscapedMid;
  fEscapedMid2  += localRun->fEscapedMid2;
  fBounce       += localRun->fBounce;
  fBounce2      += localRun->fBounce2;
  fWLSBounce    += localRun->fWLSBounce;
  fWLSBounce2   += localRun->fWLSBounce2;
  fClad1Bounce  += localRun->fClad1Bounce;
  fClad1Bounce2 += localRun->fClad1Bounce2;
  fClad2Bounce  += localRun->fClad2Bounce;
  fClad2Bounce2 += localRun->fClad2Bounce2;
  fReflected    += localRun->fReflected;
  fReflected2   += localRun->fReflected2;
  fEscaped      += localRun->fEscaped;
  fEscaped2     += localRun->fEscaped2;
  fMirror       += localRun->fMirror;
  fMirror2      += localRun->fMirror2;
  fDetectorHits += localRun->fDetectorHits;
  fDetectorHits2+= localRun->fDetectorHits2;

  // Gọi hàm Merge của lớp cha G4Run để xử lý các thông tin chung
  G4Run::Merge(run);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm EndOfRun: tính toán giá trị trung bình, độ lệch chuẩn và in ra thống kê cuối run
void WLSRun::EndOfRun()
{
  if (numberOfEvent == 0) return;
  auto TotNbofEvents = G4double(numberOfEvent);

  // Tính trung bình và độ lệch chuẩn cho từng biến thống kê
  fNTIR = fNTIR / TotNbofEvents;
  fNTIR2 = fNTIR2 / TotNbofEvents;
  G4double rmsTIR = fNTIR2 - fNTIR * fNTIR;
  if (rmsTIR > 0.)
    rmsTIR = std::sqrt(rmsTIR);
  else
    rmsTIR = 0.;

  fNExiting = fNExiting / TotNbofEvents;
  fNExiting2 = fNExiting2 / TotNbofEvents;
  G4double rmsExiting = fNExiting2 - fNExiting * fNExiting;
  if (rmsExiting > 0.)
    rmsExiting = std::sqrt(rmsExiting);
  else
    rmsExiting = 0.;

  fEscapedEnd = fEscapedEnd / TotNbofEvents;
  fEscapedEnd2 = fEscapedEnd2 / TotNbofEvents;
  G4double rmsEscapedEnd = fEscapedEnd2 - fEscapedEnd * fEscapedEnd;
  if (rmsEscapedEnd > 0.)
    rmsEscapedEnd = std::sqrt(rmsEscapedEnd);
  else
    rmsEscapedEnd = 0.;

  fEscapedMid = fEscapedMid / TotNbofEvents;
  fEscapedMid2 = fEscapedMid2 / TotNbofEvents;
  G4double rmsEscapedMid = fEscapedMid2 - fEscapedMid * fEscapedMid;
  if (rmsEscapedMid > 0.)
    rmsEscapedMid = std::sqrt(rmsEscapedMid);
  else
    rmsEscapedMid = 0.;

  fBounce = fBounce / TotNbofEvents;
  fBounce2 = fBounce2 / TotNbofEvents;
  G4double rmsBounce = fBounce2 - fBounce * fBounce;
  if (rmsBounce > 0.)
    rmsBounce = std::sqrt(rmsBounce);
  else
    rmsBounce = 0.;

  fWLSBounce = fWLSBounce / TotNbofEvents;
  fWLSBounce2 = fWLSBounce2 / TotNbofEvents;
  G4double rmsWLSBounce = fWLSBounce2 - fWLSBounce * fWLSBounce;
  if (rmsWLSBounce > 0.)
    rmsWLSBounce = std::sqrt(rmsWLSBounce);
  else
    rmsWLSBounce = 0.;

  fClad1Bounce = fClad1Bounce / TotNbofEvents;
  fClad1Bounce2 = fClad1Bounce2 / TotNbofEvents;
  G4double rmsClad1Bounce = fClad1Bounce2 - fClad1Bounce * fClad1Bounce;
  if (rmsClad1Bounce > 0.)
    rmsClad1Bounce = std::sqrt(rmsClad1Bounce);
  else
    rmsClad1Bounce = 0.;

  fClad2Bounce = fClad2Bounce / TotNbofEvents;
  fClad2Bounce2 = fClad2Bounce2 / TotNbofEvents;
  G4double rmsClad2Bounce = fClad2Bounce2 - fClad2Bounce * fClad2Bounce;
  if (rmsClad2Bounce > 0.)
    rmsClad2Bounce = std::sqrt(rmsClad2Bounce);
  else
    rmsClad2Bounce = 0.;

  fReflected = fReflected / TotNbofEvents;
  fReflected2 = fReflected2 / TotNbofEvents;
  G4double rmsReflected = fReflected2 - fReflected * fReflected;
  if (rmsReflected > 0.)
    rmsReflected = std::sqrt(rmsReflected);
  else
    rmsReflected = 0.;

  fEscaped = fEscaped / TotNbofEvents;
  fEscaped2 = fEscaped2 / TotNbofEvents;
  G4double rmsEscaped = fEscaped2 - fEscaped * fEscaped;
  if (rmsEscaped > 0.)
    rmsEscaped = std::sqrt(rmsEscaped);
  else
    rmsEscaped = 0.;

  fMirror = fMirror / TotNbofEvents;
  fMirror2 = fMirror2 / TotNbofEvents;
  G4double rmsMirror = fMirror2 - fMirror * fMirror;
  if (rmsMirror > 0.)
    rmsMirror = std::sqrt(rmsMirror);
  else
    rmsMirror = 0.;

  fDetectorHits = fDetectorHits / TotNbofEvents;
  fDetectorHits2 = fDetectorHits2 / TotNbofEvents;
  G4double rmsDetectorHits = fDetectorHits2 - fDetectorHits * fDetectorHits;
  if (rmsDetectorHits > 0.)
    rmsDetectorHits = std::sqrt(rmsDetectorHits);
  else
    rmsDetectorHits = 0.;

  // In ra thống kê tổng kết cuối run
  G4int prec = G4cout.precision(3);
  G4cout << "\n ======================== run summary =====================\n";
  G4cout << "Average number per event:" << G4endl;
  G4cout << " TIR:          " << fNTIR << " +- " << rmsTIR << G4endl
         << " Exiting:      " << fNExiting << " +- " << rmsExiting << G4endl
         << " Escaped Mid:  " << fEscapedMid << " +- " << rmsEscapedMid << G4endl
         << " Escaped End:  " << fEscapedEnd << " +- " << rmsEscapedEnd << G4endl
         << " Bounced:      " << fBounce << " +- " << rmsBounce << G4endl
         << " WLS Bounce:   " << fWLSBounce << " +- " << rmsWLSBounce << G4endl
         << " Clad1 Bounce: " << fClad1Bounce << " +- " << rmsClad1Bounce << G4endl
         << " Clad2 Bounce: " << fClad2Bounce << " +- " << rmsClad2Bounce << G4endl
         << " Reflected:    " << fReflected << " +- " << rmsReflected << G4endl
         << " Escaped:      " << fEscaped << " +- " << rmsEscaped << G4endl
         << " Mirror:       " << fMirror << " +- " << rmsMirror << G4endl
         << " Detector hit: " << fDetectorHits << " +- " << rmsDetectorHits << G4endl;

  G4cout << G4endl;
  G4cout.precision(prec);
}