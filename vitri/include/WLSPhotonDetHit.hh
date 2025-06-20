/// \file optical/wls/include/WLSPhotonDetHit.hh
/// \brief Definition of the WLSPhotonDetHit class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSPhotonDetHit_h
#define WLSPhotonDetHit_h 1

#include "G4Allocator.hh"
#include "G4LogicalVolume.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4VHit.hh"
#include "G4VPhysicalVolume.hh"

class WLSPhotonDetHit : public G4VHit
{
  public:
    WLSPhotonDetHit();
    WLSPhotonDetHit(G4ThreeVector pExit, G4ThreeVector pArrive, G4double pTime, G4double pEnergy);
    ~WLSPhotonDetHit() override = default;

    WLSPhotonDetHit(const WLSPhotonDetHit& right);
    const WLSPhotonDetHit& operator=(const WLSPhotonDetHit& right);

    G4bool operator==(const WLSPhotonDetHit& right) const;

    inline void* operator new(size_t);
    inline void operator delete(void* aHit);

    inline void SetArrivalPos(G4ThreeVector xyz) { fPosArrive = xyz; }
    inline G4ThreeVector GetArrivalPos() { return fPosArrive; }

    inline void SetExitPos(G4ThreeVector xyz) { fPosExit = xyz; }
    inline G4ThreeVector GetExitPos() { return fPosExit; }

    inline void SetArrivalTime(G4double t) { fArrivalTime = t; }
    inline G4double GetArrivalTime() { return fArrivalTime; }

    inline void SetEnergy(G4double en) { fEnergy = en; }
    inline G4double GetEnergy() { return fEnergy; }

    void Print() override;

  private:
    // the arrival time of the photon
    G4double fArrivalTime = 0.;
    // where the photon hit the detector (detector's coordinate)
    G4ThreeVector fPosArrive;
    // where the photon exited the fiber (world's coordinate)
    G4ThreeVector fPosExit;
    // energy of photon
    G4double fEnergy = 0.;
};

//--------------------------------------------------
// Type Definitions
//--------------------------------------------------

typedef G4THitsCollection<WLSPhotonDetHit> WLSPhotonDetHitsCollection;

extern G4ThreadLocal G4Allocator<WLSPhotonDetHit>* WLSPhotonDetHitAllocator;

//--------------------------------------------------
// Operator Overloads
//--------------------------------------------------

inline void* WLSPhotonDetHit::operator new(size_t)
{
  if (!WLSPhotonDetHitAllocator) WLSPhotonDetHitAllocator = new G4Allocator<WLSPhotonDetHit>;
  return (void*)WLSPhotonDetHitAllocator->MallocSingle();
}

inline void WLSPhotonDetHit::operator delete(void* aHit)
{
  WLSPhotonDetHitAllocator->FreeSingle((WLSPhotonDetHit*)aHit);
}

#endif
