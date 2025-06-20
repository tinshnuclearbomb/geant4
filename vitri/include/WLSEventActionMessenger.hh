/// \file optical/wls/include/WLSEventActionMessenger.hh
/// \brief Definition of the WLSEventActionMessenger class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSEventActionMessenger_h
#define WLSEventActionMessenger_h 1

#include "G4UImessenger.hh"

class WLSEventAction;

class G4UIcmdWithAnInteger;

class WLSEventActionMessenger : public G4UImessenger
{
  public:
    WLSEventActionMessenger(WLSEventAction*);
    ~WLSEventActionMessenger() override;

    void SetNewValue(G4UIcommand*, G4String) override;

  private:
    WLSEventAction* fEventAction = nullptr;

    G4UIcmdWithAnInteger* fSetVerboseCmd = nullptr;
};

#endif
