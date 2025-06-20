/// \file optical/wls/include/WLSTrajectoryPoint.hh
/// \brief Definition of the WLSTrajectoryPoint class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSTrajectoryPoint_h
#define WLSTrajectoryPoint_h 1

#include "G4Allocator.hh"
#include "G4StepStatus.hh"
#include "G4ThreeVector.hh"
#include "G4TrajectoryPoint.hh"
#include "globals.hh"

class G4Track;
class G4Step;
class G4VProcess;

class WLSTrajectoryPoint : public G4TrajectoryPoint
{
  public:
    WLSTrajectoryPoint() = default;
    WLSTrajectoryPoint(const G4Track*);
    WLSTrajectoryPoint(const G4Step*);
    WLSTrajectoryPoint(const WLSTrajectoryPoint& right);
    ~WLSTrajectoryPoint() override = default;

    inline void* operator new(size_t);
    inline void operator delete(void* aTrajectoryPoint);
    inline G4bool operator==(const WLSTrajectoryPoint& right) const { return (this == &right); };

    inline G4double GetTime() const { return fTime; };
    inline const G4ThreeVector GetMomentum() const { return fMomentum; };
    inline G4StepStatus GetStepStatus() const { return fStepStatus; };
    inline G4String GetVolumeName() const { return fVolumeName; };

    const std::map<G4String, G4AttDef>* GetAttDefs() const override;
    std::vector<G4AttValue>* CreateAttValues() const override;

  private:
    G4double fTime = 0.;
    G4ThreeVector fMomentum;
    G4StepStatus fStepStatus = fUndefined;
    G4String fVolumeName;
};

extern G4ThreadLocal G4Allocator<WLSTrajectoryPoint>* WLSTrajPointAllocator;

inline void* WLSTrajectoryPoint::operator new(size_t)
{
  if (!WLSTrajPointAllocator) WLSTrajPointAllocator = new G4Allocator<WLSTrajectoryPoint>;
  return (void*)WLSTrajPointAllocator->MallocSingle();
}

inline void WLSTrajectoryPoint::operator delete(void* aTrajectoryPoint)
{
  WLSTrajPointAllocator->FreeSingle((WLSTrajectoryPoint*)aTrajectoryPoint);
}

#endif
