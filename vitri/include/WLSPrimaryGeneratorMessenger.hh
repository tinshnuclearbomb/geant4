/// \file optical/wls/include/WLSPrimaryGeneratorMessenger.hh
/// \brief Definition of the WLSPrimaryGeneratorMessenger class
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef WLSPrimaryGeneratorMessenger_h
#define WLSPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"

class WLSPrimaryGeneratorAction;

class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;

class WLSPrimaryGeneratorMessenger : public G4UImessenger
{
  public:
    WLSPrimaryGeneratorMessenger(WLSPrimaryGeneratorAction*);
    ~WLSPrimaryGeneratorMessenger() override;

    void SetNewValue(G4UIcommand*, G4String) override;

  private:
    WLSPrimaryGeneratorAction* fAction = nullptr;

    G4UIdirectory* fGunDir = nullptr;

    G4UIcmdWithADoubleAndUnit* fSetPolarizationCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetDecayTimeConstantCmd = nullptr;
};

#endif
