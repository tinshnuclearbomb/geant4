/// \file optical/wls/include/WLSSteppingActionMessenger.hh
/// \brief Definition of the WLSSteppingActionMessenger class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSSteppingActionMessenger_h
#define WLSSteppingActionMessenger_h 1

#include "G4UImessenger.hh"

class WLSSteppingAction;

class G4UIdirectory;
class G4UIcmdWithAnInteger;

class WLSSteppingActionMessenger : public G4UImessenger
{
  public:
    WLSSteppingActionMessenger(WLSSteppingAction*);
    ~WLSSteppingActionMessenger() override;

    void SetNewValue(G4UIcommand*, G4String) override;

  private:
    WLSSteppingAction* fSteppingAction = nullptr;
    G4UIdirectory* fSteppingDir = nullptr;
    G4UIcmdWithAnInteger* fSetBounceLimitCmd = nullptr;
};

#endif
