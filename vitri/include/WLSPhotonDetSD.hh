/// \file optical/wls/include/WLSPhotonDetSD.hh
/// \brief Definition of the WLSPhotonDetSD class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSPhotonDetSD_h
#define WLSPhotonDetSD_h 1

#include "WLSPhotonDetHit.hh"

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;

class WLSPhotonDetSD : public G4VSensitiveDetector
{
  public:
    WLSPhotonDetSD(G4String);
    ~WLSPhotonDetSD() override = default;

    void Initialize(G4HCofThisEvent*) override;

    G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;

    void EndOfEvent(G4HCofThisEvent*) override;

  private:
    WLSPhotonDetHitsCollection* fPhotonDetHitCollection = nullptr;
    G4int fHCID = -1;
};

#endif
