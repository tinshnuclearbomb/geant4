/// \file optical/wls/include/WLSSteppingAction.hh
/// \brief Definition of the WLSSteppingAction class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSSteppingAction_h
#define WLSSteppingAction_h 1

#include "G4Types.hh"
#include "G4UserSteppingAction.hh"

class WLSDetectorConstruction;
class WLSSteppingActionMessenger;
class WLSEventAction;

class G4OpBoundaryProcess;
class G4Track;
class G4StepPoint;

class WLSSteppingAction : public G4UserSteppingAction
{
  public:
    WLSSteppingAction(WLSDetectorConstruction*, WLSEventAction*);
    ~WLSSteppingAction() override;

    void UserSteppingAction(const G4Step*) override;

    // Set the bounce limit, 0 for no limit
    void SetBounceLimit(G4int);

    G4int GetNumberOfBounces();
    G4int GetNumberOfClad1Bounces();
    G4int GetNumberOfClad2Bounces();
    G4int GetNumberOfWLSBounces();
    // return number of successful events and reset the counter
    G4int ResetSuccessCounter();

  private:
    // Artificially kill the photon after it has bounced more than this number
    G4int fBounceLimit = 100000;
    // number of photons that reach the end
    G4int fCounterEnd = 0;
    // number of photons that didn't make it to the end
    G4int fCounterMid = 0;
    // total number of bounces that a photon been through
    G4int fCounterBounce = 0;
    // number of bounces that a photon been through within the fibre
    G4int fCounterWLSBounce = 0;
    // number of bounces that a photon been through from Cladding 1 to 2
    G4int fCounterClad1Bounce = 0;
    // number of bounces that a photon been through from Cladding 2 to World
    G4int fCounterClad2Bounce = 0;

    // initial gamma of the photon
    G4double fInitGamma = -1.;
    // initial theta of the photon
    G4double fInitTheta = -1.;

    G4OpBoundaryProcess* fOpProcess = nullptr;

    WLSDetectorConstruction* fDetector = nullptr;
    WLSSteppingActionMessenger* fSteppingMessenger = nullptr;
    WLSEventAction* fEventAction = nullptr;

    inline void ResetCounters()
    {
      fCounterBounce = 0;
      fCounterWLSBounce = 0;
      fCounterClad1Bounce = 0;
      fCounterClad2Bounce = 0;
      fInitGamma = -1;
      fInitTheta = -1;
    }
};

#endif
