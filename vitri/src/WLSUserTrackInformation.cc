/// \file optical/wls/src/WLSUserTrackInformation.cc
/// \brief Implementation of the WLSUserTrackInformation class
//
//

#include "WLSUserTrackInformation.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Hàm thêm trạng thái (flag) cho track photon
// Trả về true nếu thêm thành công, false nếu không hợp lệ hoặc xung đột trạng thái
// - Không cho phép thêm trạng thái undefined hoặc trạng thái xung đột
// - Một số trạng thái chỉ được thêm khi track ở trạng thái phù hợp
G4bool WLSUserTrackInformation::AddStatusFlag(TrackStatus s)
{
  switch (s) {
    case left:
    case right:
      // Chỉ cho phép đặt trạng thái left hoặc right nếu track đang undefined
      if (fStatus == undefined) return fStatus |= s;
      return false;

    case EscapedFromSide:
    case EscapedFromReadOut:
      // Chỉ cho phép đặt trạng thái escaped nếu photon chưa ra khỏi sợi
      if ((fStatus == undefined) || (fStatus & OutsideOfFiber)) return false;
      return fStatus |= s;

    case ReflectedAtMirror:
    case ReflectedAtReadOut:
    case murderee:
      // Các trạng thái này có thể thêm bất cứ lúc nào
      return fStatus |= s;

    case InsideOfFiber:
      // Khi vào lại sợi quang, xóa các trạng thái đã thoát và đặt lại InsideOfFiber
      return (fStatus = (fStatus & ~(EscapedFromSide + EscapedFromReadOut + OutsideOfFiber)) | s);

    case OutsideOfFiber:
      // Khi ra khỏi sợi quang, xóa trạng thái InsideOfFiber và đặt OutsideOfFiber
      return (fStatus = (fStatus & ~InsideOfFiber) | s);

    default:
      // Không cho phép thêm trạng thái không xác định hoặc không hợp lệ
      return false;
  }
}