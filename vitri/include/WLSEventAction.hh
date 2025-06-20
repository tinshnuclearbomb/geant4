/// \file optical/wls/include/WLSEventAction.hh
/// \brief Definition of the WLSEventAction class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSEventAction_h
#define WLSEventAction_h 1

#include "G4Types.hh"
#include "G4UserEventAction.hh"

class WLSEventActionMessenger;

class WLSEventAction : public G4UserEventAction
{
  public:
    WLSEventAction();
    ~WLSEventAction() override;

    void BeginOfEventAction(const G4Event*) override;
    void EndOfEventAction(const G4Event*) override;

    G4int GetEventNo();
    void SetEventVerbose(G4int);

    void AddTIR() { fNTIR += 1; };
    void AddExiting() { fNExiting += 1; };
    void AddEscapedEnd() { fEscapedEnd += 1; };
    void AddEscapedMid() { fEscapedMid += 1; };
    void AddBounce() { fBounce += 1; };
    void AddWLSBounce() { fWLSBounce += 1; };
    void AddClad1Bounce() { fClad1Bounce += 1; };
    void AddClad2Bounce() { fClad2Bounce += 1; };
    void AddReflected() { fReflected += 1; };
    void AddEscaped() { fEscaped += 1; };
    void AddMirror() { fMirror += 1; };

  private:
    WLSEventActionMessenger* fEventMessenger = nullptr;

    G4int fVerboseLevel = 0;

    G4int fMPPCCollID = 0;

    G4int fNTIR = 0;
    G4int fNExiting = 0;
    G4int fEscapedEnd = 0;
    G4int fEscapedMid = 0;
    G4int fBounce = 0;
    G4int fWLSBounce = 0;
    G4int fClad1Bounce = 0;
    G4int fClad2Bounce = 0;
    G4int fReflected = 0;
    G4int fEscaped = 0;
    G4int fMirror = 0;
};

#endif
